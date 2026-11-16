#include "editor/ClangdClient.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonValue>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QUrl>

namespace {

constexpr int kRequestTimeoutMs = 5000;

void trace(const QString & text)
{
    if(!qEnvironmentVariableIsSet("EASYLVGL_CLANGD_TRACE")) return;
    QFile file(QDir::tempPath() + "/lvgl-clangd.log");
    if(file.open(QIODevice::WriteOnly | QIODevice::Append)) file.write(text.toUtf8() + "\n");
}

QString positionUri(const QString & path)
{
    return QUrl::fromLocalFile(QFileInfo(path).absoluteFilePath()).toString(QUrl::FullyEncoded);
}

} // namespace

ClangdClient::ClangdClient(QObject * parent) : QObject(parent)
{
    m_process.setProcessChannelMode(QProcess::SeparateChannels);
    connect(&m_process, &QProcess::readyReadStandardOutput, this, &ClangdClient::readOutput);
    connect(&m_process, &QProcess::readyReadStandardError, this, [this] {
        const QString text = QString::fromUtf8(m_process.readAllStandardError()).trimmed();
        if(!text.isEmpty()) emit logMessage(text);
    });
    connect(&m_process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError) {
        if(m_process.error() != QProcess::FailedToStart) return;
        emit logMessage("clangd 启动失败：" + m_process.errorString());
    });
}

ClangdClient::~ClangdClient()
{
    stop();
}

QString ClangdClient::findClangd()
{
    const QString configured = qEnvironmentVariable("EASYLVGL_CLANGD");
    if(!configured.isEmpty() && QFileInfo::exists(configured)) return configured;

    const QString path = QStandardPaths::findExecutable("clangd");
    if(!path.isEmpty()) return path;

    const QString appDir         = QCoreApplication::applicationDirPath();
    const QStringList candidates = {
        QDir(appDir).absoluteFilePath("../../../../toolchain/clangd/bin/clangd.exe"),
        QDir(appDir).absoluteFilePath("../../../toolchain/clangd/bin/clangd.exe"),
        QDir(appDir).absoluteFilePath("../../../../../toolchain/clangd/bin/clangd.exe"),
        "E:/QT/QTCreator10.0.2/bin/clang/bin/clangd.exe",
        "E:/QT/QT5.12.2/Tools/QtCreator/bin/clang/bin/clangd.exe",
        "C:/Qt/Tools/QtCreator/bin/clang/bin/clangd.exe",
    };
    for(const QString & candidate : candidates) {
        if(QFileInfo::exists(candidate)) return candidate;
    }
    return {};
}

QString ClangdClient::pathToUri(const QString & path)
{
    return positionUri(path);
}

QString ClangdClient::uriToPath(const QString & uri)
{
    const QString local = QUrl(uri).toLocalFile();
    if(!local.isEmpty()) return local;
    QString fallback = uri;
    if(fallback.startsWith("file://")) fallback.remove(0, 7);
    return QUrl::fromPercentEncoding(fallback.toUtf8());
}

QString ClangdClient::languageId(const QString & path)
{
    const QString suffix = QFileInfo(path).suffix().toLower();
    if(suffix == "c") return "c";
    if(suffix == "h") return "c";
    if(suffix == "hpp" || suffix == "hxx" || suffix == "hh") return "cpp";
    if(suffix == "cc" || suffix == "cxx") return "cpp";
    return "cpp";
}

bool ClangdClient::start(const QString & projectDir, const QString & buildDir, const QString & compiler,
                         QString * error)
{
    stop();

    m_projectDir = QFileInfo(projectDir).absoluteFilePath();
    m_buildDir   = buildDir.isEmpty() ? QString() : QFileInfo(buildDir).absoluteFilePath();
    m_compiler   = compiler;
    m_root       = m_projectDir;
    m_ready      = false;
    m_buffer.clear();

    const QString clangd = findClangd();
    if(clangd.isEmpty()) {
        if(error) *error = "找不到 clangd。";
        return false;
    }

    QStringList arguments{"--background-index", "--log=error", "--pch-storage=memory"};
    if(!m_buildDir.isEmpty()) arguments << "--compile-commands-dir=" + QDir::toNativeSeparators(m_buildDir);
    if(!m_compiler.isEmpty()) arguments << "--query-driver=" + QDir::toNativeSeparators(m_compiler);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if(!m_compiler.isEmpty()) {
        const QString compilerDir = QFileInfo(m_compiler).absolutePath();
        if(!compilerDir.isEmpty()) env.insert("PATH", compilerDir + QDir::listSeparator() + env.value("PATH"));
    }

    m_process.setProcessEnvironment(env);
    m_process.setWorkingDirectory(m_projectDir);
    m_process.setProgram(clangd);
    m_process.setArguments(arguments);
    m_process.start();
    trace("start " + clangd + " " + arguments.join(" "));
    if(!m_process.waitForStarted(kRequestTimeoutMs)) {
        if(error) *error = m_process.errorString();
        return false;
    }

    initialize();
    return true;
}

void ClangdClient::initialize()
{
    QJsonObject completionItem{{"snippetSupport", true}};
    QJsonObject completion{{"completionItem", completionItem}};
    QJsonObject hover{{"contentFormat", QJsonArray{"markdown", "plaintext"}}};
    QJsonObject textDocument{{"completion", completion},
                             {"definition", QJsonObject()},
                             {"hover", hover},
                             {"publishDiagnostics", QJsonObject()},
                             {"documentSymbol", QJsonObject()}};
    QJsonObject params{
        {"processId", QCoreApplication::applicationPid()},
        {"rootUri", pathToUri(m_projectDir)},
        {"capabilities", QJsonObject{{"textDocument", textDocument}}},
        {"workspaceFolders",
         QJsonArray{QJsonObject{{"uri", pathToUri(m_projectDir)}, {"name", QFileInfo(m_projectDir).fileName()}}}},
    };

    trace("send initialize");
    request("initialize", params, [this](const QJsonObject & message) {
        if(message.contains("error")) {
            emit logMessage(
                "clangd initialize 失败：" +
                QString::fromUtf8(QJsonDocument(message["error"].toObject()).toJson(QJsonDocument::Compact)));
            return;
        }
        m_ready = true;
        trace("initialized");
        notify("initialized", QJsonObject());
        flushPendingDocuments();
        flushPendingRequests();
        emit ready();
    });
}

void ClangdClient::stop()
{
    if(m_process.state() != QProcess::NotRunning) {
        if(m_ready) {
            QJsonObject shutdown{{"jsonrpc", "2.0"}, {"id", m_nextId++}, {"method", "shutdown"}};
            send(shutdown);
            notify("exit", QJsonObject());
        }
        m_process.waitForFinished(500);
        if(m_process.state() != QProcess::NotRunning) m_process.kill();
    }
    m_ready = false;
    m_handlers.clear();
    m_pendingDocuments.clear();
    m_pendingRequests.clear();
    m_versions.clear();
    m_buffer.clear();
}

bool ClangdClient::isRunning() const
{
    return m_process.state() != QProcess::NotRunning;
}

void ClangdClient::send(const QJsonObject & message)
{
    if(m_process.state() != QProcess::Running) return;
    const QByteArray payload = QJsonDocument(message).toJson(QJsonDocument::Compact);
    m_process.write("Content-Length: " + QByteArray::number(payload.size()) + "\r\n\r\n" + payload);
}

void ClangdClient::notify(const QString & method, const QJsonObject & params)
{
    send(QJsonObject{{"jsonrpc", "2.0"}, {"method", method}, {"params", params}});
}

void ClangdClient::request(const QString & method, const QJsonObject & params, ResultHandler handler)
{
    if(!isRunning()) return;
    const int id = m_nextId++;
    trace("request " + method + " id=" + QString::number(id));
    m_handlers.insert(id, std::move(handler));
    send(QJsonObject{{"jsonrpc", "2.0"}, {"id", id}, {"method", method}, {"params", params}});
}

void ClangdClient::didOpen(const QString & path, const QString & text)
{
    const QString absolute = QFileInfo(path).absoluteFilePath();
    trace("didOpen " + absolute + " ready=" + QString::number(m_ready));
    if(!m_ready) {
        m_pendingDocuments.insert(absolute, {text, languageId(absolute)});
        return;
    }
    m_versions.insert(absolute, 1);
    notify("textDocument/didOpen", QJsonObject{{"textDocument", QJsonObject{{"uri", pathToUri(absolute)},
                                                                            {"languageId", languageId(absolute)},
                                                                            {"version", 1},
                                                                            {"text", text}}}});
}

void ClangdClient::didChange(const QString & path, const QString & text)
{
    const QString absolute = QFileInfo(path).absoluteFilePath();
    if(!m_ready) {
        if(m_pendingDocuments.contains(absolute)) m_pendingDocuments[absolute].first = text;
        return;
    }
    const int version = m_versions.value(absolute, 1) + 1;
    m_versions.insert(absolute, version);
    notify("textDocument/didChange",
           QJsonObject{{"textDocument", QJsonObject{{"uri", pathToUri(absolute)}, {"version", version}}},
                       {"contentChanges", QJsonArray{QJsonObject{{"text", text}}}}});
}

void ClangdClient::didClose(const QString & path)
{
    const QString absolute = QFileInfo(path).absoluteFilePath();
    m_pendingDocuments.remove(absolute);
    m_versions.remove(absolute);
    if(m_ready)
        notify("textDocument/didClose", QJsonObject{{"textDocument", QJsonObject{{"uri", pathToUri(absolute)}}}});
}

void ClangdClient::flushPendingDocuments()
{
    const auto pending = m_pendingDocuments;
    m_pendingDocuments.clear();
    for(auto it = pending.begin(); it != pending.end(); ++it) didOpen(it.key(), it.value().first);
}

void ClangdClient::flushPendingRequests()
{
    const auto pending = m_pendingRequests;
    m_pendingRequests.clear();
    for(const auto & request : pending) request();
}

void ClangdClient::completion(const QString & path, int line, int character, ResultHandler handler)
{
    trace("completion ready=" + QString::number(m_ready) + " line=" + QString::number(line) +
          " char=" + QString::number(character));
    if(!m_ready) {
        if(isRunning()) {
            m_pendingRequests.append([this, path, line, character, handler] {
                completion(path, line, character, handler);
            });
        }
        return;
    }
    request("textDocument/completion",
            QJsonObject{{"textDocument", QJsonObject{{"uri", pathToUri(path)}}},
                        {"position", QJsonObject{{"line", line}, {"character", character}}}},
            std::move(handler));
}

void ClangdClient::definition(const QString & path, int line, int character, ResultHandler handler)
{
    trace("definition ready=" + QString::number(m_ready) + " line=" + QString::number(line) +
          " char=" + QString::number(character));
    if(!m_ready) {
        if(isRunning()) {
            m_pendingRequests.append([this, path, line, character, handler] {
                definition(path, line, character, handler);
            });
        }
        return;
    }
    request("textDocument/definition",
            QJsonObject{{"textDocument", QJsonObject{{"uri", pathToUri(path)}}},
                        {"position", QJsonObject{{"line", line}, {"character", character}}}},
            std::move(handler));
}

void ClangdClient::declaration(const QString & path, int line, int character, ResultHandler handler)
{
    if(!m_ready) {
        if(isRunning()) {
            m_pendingRequests.append([this, path, line, character, handler] {
                declaration(path, line, character, handler);
            });
        }
        return;
    }
    request("textDocument/declaration",
            QJsonObject{{"textDocument", QJsonObject{{"uri", pathToUri(path)}}},
                        {"position", QJsonObject{{"line", line}, {"character", character}}}},
            std::move(handler));
}

void ClangdClient::references(const QString & path, int line, int character, ResultHandler handler)
{
    if(!m_ready) {
        if(isRunning()) {
            m_pendingRequests.append([this, path, line, character, handler] {
                references(path, line, character, handler);
            });
        }
        return;
    }
    request("textDocument/references",
            QJsonObject{{"textDocument", QJsonObject{{"uri", pathToUri(path)}}},
                        {"position", QJsonObject{{"line", line}, {"character", character}}},
                        {"context", QJsonObject{{"includeDeclaration", true}}}},
            std::move(handler));
}

void ClangdClient::workspaceSymbol(const QString & query, ResultHandler handler)
{
    if(!m_ready) {
        if(isRunning()) {
            m_pendingRequests.append([this, query, handler] {
                workspaceSymbol(query, handler);
            });
        }
        return;
    }
    request("workspace/symbol", QJsonObject{{"query", query}}, std::move(handler));
}

void ClangdClient::hover(const QString & path, int line, int character, ResultHandler handler)
{
    if(!m_ready) {
        if(isRunning()) {
            m_pendingRequests.append([this, path, line, character, handler] {
                hover(path, line, character, handler);
            });
        }
        return;
    }
    request("textDocument/hover",
            QJsonObject{{"textDocument", QJsonObject{{"uri", pathToUri(path)}}},
                        {"position", QJsonObject{{"line", line}, {"character", character}}}},
            std::move(handler));
}

void ClangdClient::readOutput()
{
    m_buffer += m_process.readAllStandardOutput();
    while(true) {
        const int headerEnd = m_buffer.indexOf("\r\n\r\n");
        if(headerEnd < 0) return;

        int contentLength       = -1;
        const QByteArray header = m_buffer.left(headerEnd);
        for(const QByteArray & line : header.split('\n')) {
            const QByteArray trimmed = line.trimmed();
            if(trimmed.toLower().startsWith("content-length:")) contentLength = trimmed.mid(15).trimmed().toInt();
        }
        if(contentLength < 0) {
            m_buffer.remove(0, headerEnd + 4);
            continue;
        }
        const int messageStart = headerEnd + 4;
        if(m_buffer.size() < messageStart + contentLength) return;

        const QByteArray payload = m_buffer.mid(messageStart, contentLength);
        m_buffer.remove(0, messageStart + contentLength);
        QJsonParseError parseError;
        const QJsonDocument document = QJsonDocument::fromJson(payload, &parseError);
        if(parseError.error != QJsonParseError::NoError || !document.isObject()) continue;
        handleMessage(document.object());
    }
}

void ClangdClient::handleMessage(const QJsonObject & message)
{
    if(message.contains("id") && (message.contains("result") || message.contains("error"))) {
        const int id = message["id"].toInt();
        trace("response id=" + QString::number(id));
        const auto handler = m_handlers.take(id);
        if(handler) handler(message);
        return;
    }

    const QString method     = message["method"].toString();
    const QJsonObject params = message["params"].toObject();
    if(method == "textDocument/publishDiagnostics") {
        emit diagnostics(uriToPath(params["uri"].toString()), params["diagnostics"].toArray());
    } else if(method == "window/logMessage" || method == "window/showMessage") {
        emit logMessage(params["message"].toString());
    }
}
