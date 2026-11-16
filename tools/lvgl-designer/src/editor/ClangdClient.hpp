#pragma once

#include <QByteArray>
#include <QHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QProcess>
#include <QString>

#include <functional>

class ClangdClient : public QObject {
    Q_OBJECT
  public:
    using ResultHandler = std::function<void(const QJsonObject &)>;

    explicit ClangdClient(QObject * parent = nullptr);
    ~ClangdClient() override;

    bool start(const QString & projectDir, const QString & buildDir, const QString & compiler,
               QString * error = nullptr);
    void stop();
    bool isRunning() const;
    bool isReady() const
    {
        return m_ready;
    }
    QString rootPath() const
    {
        return m_root;
    }

    void didOpen(const QString & path, const QString & text);
    void didChange(const QString & path, const QString & text);
    void didClose(const QString & path);

    void completion(const QString & path, int line, int character, ResultHandler handler);
    void definition(const QString & path, int line, int character, ResultHandler handler);
    void declaration(const QString & path, int line, int character, ResultHandler handler);
    void references(const QString & path, int line, int character, ResultHandler handler);
    void workspaceSymbol(const QString & query, ResultHandler handler);
    void hover(const QString & path, int line, int character, ResultHandler handler);

    static QString uriToPath(const QString & uri);

  signals:
    void ready();
    void diagnostics(const QString & path, const QJsonArray & diagnostics);
    void logMessage(const QString & text);

  private:
    void initialize();
    void send(const QJsonObject & message);
    void notify(const QString & method, const QJsonObject & params);
    void request(const QString & method, const QJsonObject & params, ResultHandler handler);
    void readOutput();
    void handleMessage(const QJsonObject & message);
    void flushPendingDocuments();
    void flushPendingRequests();

    static QString findClangd();
    static QString pathToUri(const QString & path);
    static QString languageId(const QString & path);

    QProcess m_process;
    QByteArray m_buffer;
    QHash<int, ResultHandler> m_handlers;
    QHash<QString, QPair<QString, QString>> m_pendingDocuments;
    QHash<QString, int> m_versions;
    QList<std::function<void()>> m_pendingRequests;
    QString m_root;
    QString m_projectDir;
    QString m_buildDir;
    QString m_compiler;
    int m_nextId = 1;
    bool m_ready = false;
};
