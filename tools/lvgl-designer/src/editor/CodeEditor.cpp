#include "CodeEditor.hpp"

#include "ClangdClient.hpp"

#include <QApplication>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPointer>
#include <QPushButton>
#include <QRegularExpression>
#include <QSaveFile>
#include <QSettings>
#include <QSet>
#include <QScrollBar>
#include <QSyntaxHighlighter>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextEdit>
#include <QTimer>
#include <QToolButton>
#include <QToolTip>
#include <QWheelEvent>

namespace {

void editorTrace(const QString & text)
{
    if(!qEnvironmentVariableIsSet("EASYLVGL_CLANGD_TRACE")) return;
    QFile file(QDir::tempPath() + "/lvgl-editor.log");
    if(file.open(QIODevice::WriteOnly | QIODevice::Append)) file.write(text.toUtf8() + "\n");
}

bool decodeLocation(const QJsonValue & value, QString & path, int & line, int & column)
{
    if(!value.isObject()) return false;
    QJsonObject object = value.toObject();
    if(object.contains("location")) object = object["location"].toObject();
    QString uri = object["uri"].toString();
    QJsonObject range = object["range"].toObject();
    if(uri.isEmpty()) {
        uri = object["targetUri"].toString();
        range = object["targetSelectionRange"].toObject();
        if(range.isEmpty()) range = object["targetRange"].toObject();
    }
    if(uri.isEmpty()) return false;
    path = ClangdClient::uriToPath(uri);
    const QJsonObject start = range["start"].toObject();
    line = start["line"].toInt() + 1;
    column = start["character"].toInt() + 1;
    return !path.isEmpty();
}

bool isSourcePath(const QString & path)
{
    const QString suffix = QFileInfo(path).suffix().toLower();
    return suffix == "c" || suffix == "cpp" || suffix == "cc" || suffix == "cxx";
}

class CppHighlighter : public QSyntaxHighlighter {
  public:
    explicit CppHighlighter(QTextDocument * parent) : QSyntaxHighlighter(parent)
    {
        m_keywords = QSet<QString>{
            "alignas",  "alignof",  "and",        "asm",        "auto",       "break",
            "case",     "catch",    "class",      "concept",    "const",      "consteval",
            "constexpr", "constinit", "continue", "co_await",   "co_return",  "co_yield",
            "decltype", "default",  "delete",     "do",         "else",       "enum",
            "explicit", "export",   "extern",     "false",      "final",      "for",
            "friend",   "goto",     "if",         "inline",     "mutable",    "namespace",
            "new",      "noexcept", "not",        "nullptr",    "operator",   "or",
            "override", "private",  "protected",  "public",     "register",   "requires",
            "return",   "sizeof",   "static",     "static_assert", "struct",  "switch",
            "template", "this",     "throw",      "true",       "try",        "typedef",
            "typename", "union",    "using",      "virtual",    "volatile",   "while",
        };
        m_types = QSet<QString>{
            "bool",     "char",     "char8_t",  "char16_t", "char32_t", "double",
            "float",    "int",      "int8_t",   "int16_t",  "int32_t",  "int64_t",
            "long",     "short",    "signed",   "size_t",   "ssize_t",  "uint8_t",
            "uint16_t", "uint32_t", "uint64_t", "unsigned", "void",     "wchar_t",
            "lv_obj_t", "lv_event_t", "lv_display_t", "lv_indev_t",
        };

        m_keyword.setForeground(QColor(0x56, 0x9C, 0xD6));
        m_keyword.setFontWeight(QFont::DemiBold);
        m_type.setForeground(QColor(0x4E, 0xC9, 0xB0));
        m_preprocessor.setForeground(QColor(0xC5, 0x86, 0xC0));
        m_macro.setForeground(QColor(0xBD, 0x8D, 0xD4));
        m_string.setForeground(QColor(0xCE, 0x91, 0x78));
        m_number.setForeground(QColor(0xB5, 0xCE, 0xA8));
        m_function.setForeground(QColor(0xDC, 0xDC, 0xAA));
        m_comment.setForeground(QColor(0x6A, 0x99, 0x55));
    }

  protected:
    void highlightBlock(const QString & text) override
    {
        int state = previousBlockState();
        int i     = 0;

        if(state == StateBlockComment) {
            const int end = text.indexOf("*/");
            if(end < 0) {
                setFormat(0, text.length(), m_comment);
                setCurrentBlockState(StateBlockComment);
                return;
            }
            setFormat(0, end + 2, m_comment);
            i     = end + 2;
            state = StateNormal;
        } else if(state == StateString) {
            const int end = findStringEnd(text, 0, '"');
            if(end < 0) {
                setFormat(0, text.length(), m_string);
                setCurrentBlockState(StateString);
                return;
            }
            setFormat(0, end + 1, m_string);
            i     = end + 1;
            state = StateNormal;
        }

        const int firstNonSpace = firstNonSpaceIndex(text);
        bool expectTypeName     = false;

        while(i < text.length()) {
            const QChar c = text.at(i);
            const QChar n = i + 1 < text.length() ? text.at(i + 1) : QChar();

            if(c == '/' && n == '/') {
                setFormat(i, text.length() - i, m_comment);
                break;
            }
            if(c == '/' && n == '*') {
                const int end = text.indexOf("*/", i + 2);
                if(end < 0) {
                    setFormat(i, text.length() - i, m_comment);
                    setCurrentBlockState(StateBlockComment);
                    return;
                }
                setFormat(i, end + 2 - i, m_comment);
                i = end + 2;
                continue;
            }
            if(c == '#' && i == firstNonSpace) {
                setFormat(i, text.length() - i, m_preprocessor);
                break;
            }
            if(c == '"' || c == '\'') {
                const QChar quote = c;
                int end           = findStringEnd(text, i + 1, quote);
                if(end < 0) {
                    setFormat(i, text.length() - i, m_string);
                    setCurrentBlockState(quote == '"' ? StateString : StateNormal);
                    return;
                }
                setFormat(i, end + 1 - i, m_string);
                i = end + 1;
                continue;
            }
            if(isIdentifierStart(c)) {
                const int begin = i;
                while(i < text.length() && isIdentifierChar(text.at(i))) ++i;
                const QString word = text.mid(begin, i - begin);

                if(expectTypeName) {
                    setFormat(begin, i - begin, m_type);
                    expectTypeName = false;
                    continue;
                }

                if(m_keywords.contains(word)) {
                    setFormat(begin, i - begin, m_keyword);
                    expectTypeName = word == "class" || word == "struct" || word == "enum" ||
                                     word == "namespace" || word == "using";
                    continue;
                }
                if(m_types.contains(word)) {
                    setFormat(begin, i - begin, m_type);
                    continue;
                }
                if(isMacroName(word)) {
                    setFormat(begin, i - begin, m_macro);
                    continue;
                }
                int next = i;
                while(next < text.length() && text.at(next).isSpace()) ++next;
                if(next < text.length() && text.at(next) == '(') {
                    setFormat(begin, i - begin, m_function);
                } else if(begin == 0 || !text.at(begin - 1).isLetterOrNumber()) {
                    if(!word.isEmpty() && word.at(0).isUpper()) setFormat(begin, i - begin, m_type);
                }
                continue;
            }
            if(c.isDigit()) {
                const int begin = i;
                if(c == '0' && i + 1 < text.length() &&
                   (text.at(i + 1) == 'x' || text.at(i + 1) == 'X')) {
                    i += 2;
                    while(i < text.length() && (text.at(i).isDigit() ||
                          (text.at(i).toLower() >= 'a' && text.at(i).toLower() <= 'f') ||
                          text.at(i) == '\'')) ++i;
                } else if(c == '0' && i + 1 < text.length() &&
                          (text.at(i + 1) == 'b' || text.at(i + 1) == 'B')) {
                    i += 2;
                    while(i < text.length() && (text.at(i) == '0' || text.at(i) == '1' ||
                          text.at(i) == '\'')) ++i;
                } else {
                    while(i < text.length() && (text.at(i).isDigit() || text.at(i) == '.' ||
                          text.at(i) == '\'')) ++i;
                }
                while(i < text.length() && (text.at(i) == 'u' || text.at(i) == 'U' ||
                      text.at(i) == 'l' || text.at(i) == 'L' || text.at(i) == 'f' ||
                      text.at(i) == 'F')) ++i;
                setFormat(begin, i - begin, m_number);
                continue;
            }
            ++i;
        }
        setCurrentBlockState(StateNormal);
    }

  private:
    enum State { StateNormal = 0, StateBlockComment = 1, StateString = 2 };

    static bool isIdentifierStart(QChar c)
    {
        return c.isLetter() || c == '_';
    }

    static bool isIdentifierChar(QChar c)
    {
        return c.isLetterOrNumber() || c == '_';
    }

    static int firstNonSpaceIndex(const QString & text)
    {
        for(int i = 0; i < text.length(); ++i)
            if(!text.at(i).isSpace()) return i;
        return text.length();
    }

    static int findStringEnd(const QString & text, int start, QChar quote)
    {
        bool escaped = false;
        for(int i = start; i < text.length(); ++i) {
            const QChar c = text.at(i);
            if(escaped) {
                escaped = false;
                continue;
            }
            if(c == '\\') {
                escaped = true;
                continue;
            }
            if(c == quote) return i;
        }
        return -1;
    }

    static bool isMacroName(const QString & word)
    {
        if(word.startsWith("LV_")) return true;
        if(word.length() < 2) return false;
        for(QChar c : word)
            if(!c.isUpper() && !c.isDigit() && c != '_') return false;
        return true;
    }

    QSet<QString> m_keywords;
    QSet<QString> m_types;
    QTextCharFormat m_keyword;
    QTextCharFormat m_type;
    QTextCharFormat m_preprocessor;
    QTextCharFormat m_macro;
    QTextCharFormat m_string;
    QTextCharFormat m_number;
    QTextCharFormat m_function;
    QTextCharFormat m_comment;
};

} // namespace

class CodeEditor::LineNumberArea : public QWidget {
  public:
    explicit LineNumberArea(CodeEditor * editor) : QWidget(editor), m_editor(editor)
    {}

    QSize sizeHint() const override
    {
        return QSize(m_editor->lineNumberAreaWidth(), 0);
    }

  protected:
    void paintEvent(QPaintEvent * event) override
    {
        m_editor->lineNumberAreaPaintEvent(event);
    }

  private:
    CodeEditor * m_editor;
};

CodeEditor::CodeEditor(QWidget * parent) : QPlainTextEdit(parent)
{
    setLineWrapMode(QPlainTextEdit::NoWrap);
    setTabStopDistance(fontMetrics().horizontalAdvance(' ') * 4);
    QFont mono = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    mono.setPointSize(QSettings().value("editor/fontPointSize", 10).toInt());
    setFont(mono);
    QPalette colors = palette();
    colors.setColor(QPalette::Base, QColor(0x1e, 0x1e, 0x1e));
    colors.setColor(QPalette::Text, QColor(0xd4, 0xd4, 0xd4));
    colors.setColor(QPalette::Highlight, QColor(0x2f, 0x5d, 0x8a));
    colors.setColor(QPalette::HighlightedText, Qt::white);
    setPalette(colors);

    m_lineNumberArea = new LineNumberArea(this);
    connect(this, &QPlainTextEdit::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &CodeEditor::updateExtraSelections);
    connect(this, &QPlainTextEdit::textChanged, this, [this] {
        updateExtraSelections();
        scheduleDocumentSync();
    });

    m_syncTimer = new QTimer(this);
    m_syncTimer->setSingleShot(true);
    m_syncTimer->setInterval(180);
    connect(m_syncTimer, &QTimer::timeout, this, [this] {
        if(m_client && !m_path.isEmpty()) m_client->didChange(m_path, toPlainText());
    });

    updateLineNumberAreaWidth(0);
    updateTabStopDistance();
    updateExtraSelections();
}

CodeEditor::~CodeEditor()
{
    if(m_client && !m_path.isEmpty()) m_client->didClose(m_path);
}

bool CodeEditor::loadFile(const QString & path, QString * error)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)) {
        if(error) *error = file.errorString();
        return false;
    }
    if(m_client && !m_path.isEmpty()) m_client->didClose(m_path);
    QString text = QString::fromUtf8(file.readAll());
    text.replace("\r\n", "\n");
    text.replace('\r', '\n');
    setPlainText(text);
    m_path = QFileInfo(path).absoluteFilePath();

    if(m_highlighter) {
        delete m_highlighter;
        m_highlighter = nullptr;
    }
    const QString suffix = QFileInfo(path).suffix().toLower();
    if(suffix == "h" || suffix == "hpp" || suffix == "hh" || suffix == "hxx" || suffix == "c" || suffix == "cpp" ||
       suffix == "cc" || suffix == "cxx" || suffix == "py") {
        m_highlighter = new CppHighlighter(document());
    }
    document()->setModified(false);
    if(m_client && !m_path.isEmpty()) m_client->didOpen(m_path, text);
    updateExtraSelections();
    return true;
}

bool CodeEditor::save()
{
    if(m_path.isEmpty()) return false;
    QSaveFile file(m_path);
    if(!file.open(QIODevice::WriteOnly)) return false;
    file.write(toPlainText().toUtf8());
    if(!file.commit()) return false;
    document()->setModified(false);
    emit saved(m_path);
    return true;
}

bool CodeEditor::saveAs(const QString & path)
{
    QFileInfo info(path);
    m_path = info.absoluteFilePath();
    return save();
}

void CodeEditor::setLanguageClient(ClangdClient * client)
{
    m_client = client;
    if(m_client && !m_path.isEmpty()) m_client->didOpen(m_path, toPlainText());
    if(m_client && m_pendingDefinition) {
        m_pendingDefinition = false;
        QTimer::singleShot(0, this, [this] { requestDefinition(); });
    }
}

void CodeEditor::setProjectRoot(const QString & root)
{
    m_projectRoot = QFileInfo(root).absoluteFilePath();
}

QString CodeEditor::identifierAtCursor() const
{
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    QString word = cursor.selectedText();
    if(!word.isEmpty()) return word;

    const QString line = textCursor().block().text();
    auto isWord        = [](QChar ch) {
        return ch.isLetterOrNumber() || ch == '_';
    };
    int position = textCursor().positionInBlock();
    if(position > 0 && (position >= line.length() || !isWord(line.at(position))) &&
       isWord(line.at(position - 1)))
        --position;
    int start = position;
    while(start > 0 && isWord(line.at(start - 1))) --start;
    int end = position;
    while(end < line.length() && isWord(line.at(end))) ++end;
    return line.mid(start, end - start);
}

void CodeEditor::requestCompletion()
{
    editorTrace("requestCompletion client=" + QString::number(m_client != nullptr) + " path=" + m_path);
    if(!m_client || m_path.isEmpty()) return;
    m_client->didChange(m_path, toPlainText());
    const QTextCursor cursor = textCursor();
    QPointer<CodeEditor> self(this);
    m_client->completion(m_path, cursor.blockNumber(), cursor.positionInBlock(), [self](const QJsonObject & message) {
        if(self) self->showCompletion(message);
    });
}

void CodeEditor::showCompletion(const QJsonObject & message)
{
    if(!m_completion) {
        m_completion = new QListWidget(this);
        m_completion->setFocusPolicy(Qt::NoFocus);
        m_completion->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
        m_completion->setUniformItemSizes(true);
        m_completion->setStyleSheet("QListWidget { background:#252525; color:#e0e0e0; border:1px solid #555; "
                                    "padding:2px; } QListWidget::item { padding:3px 8px; } "
                                    "QListWidget::item:selected { background:#2f5d8a; color:white; }");
        connect(m_completion, &QListWidget::itemClicked, this, [this](QListWidgetItem *) { acceptCompletion(); });
    }

    const QJsonValue result = message["result"];
    QJsonArray items;
    if(result.isArray())
        items = result.toArray();
    else if(result.isObject())
        items = result.toObject()["items"].toArray();

    m_completion->clear();
    for(const QJsonValue & value : items) {
        const QJsonObject item = value.toObject();
        const QString label    = item["label"].toString();
        if(label.isEmpty()) continue;
        QString insert = item["textEdit"].toObject()["newText"].toString();
        if(insert.isEmpty()) insert = item["insertText"].toString();
        if(insert.isEmpty()) insert = label;
        insert.replace(QRegularExpression("\\$\\{\\d+:([^}]*)\\}"), "\\1");
        insert.remove(QRegularExpression("\\$\\d+"));
        auto * row = new QListWidgetItem(label, m_completion);
        row->setData(Qt::UserRole, insert);
        if(m_completion->count() >= 100) break;
    }

    if(m_completion->count() == 0) {
        editorTrace("completion empty");
        hideCompletion();
        return;
    }
    editorTrace("completion items=" + QString::number(m_completion->count()));

    const QRect rect = cursorRect();
    const QPoint pos = viewport()->mapToGlobal(rect.bottomLeft()) + QPoint(0, 4);
    m_completion->move(pos);
    m_completion->resize(qMin(460, qMax(260, width() - 40)), qMin(260, m_completion->count() * 26 + 8));
    m_completion->setCurrentRow(0);
    m_completion->show();
    m_completion->raise();
}

void CodeEditor::acceptCompletion()
{
    if(!m_completion || !m_completion->currentItem()) return;
    const QString insert = m_completion->currentItem()->data(Qt::UserRole).toString();
    QTextCursor cursor   = textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    cursor.insertText(insert);
    setTextCursor(cursor);
    hideCompletion();
}

void CodeEditor::hideCompletion()
{
    if(m_completion) m_completion->hide();
}

bool CodeEditor::completionVisible() const
{
    return m_completion && m_completion->isVisible();
}

void CodeEditor::requestDefinition()
{
    editorTrace("requestDefinition client=" + QString::number(m_client != nullptr) +
                " identifier=" + identifierAtCursor());
    if(!m_client || m_path.isEmpty()) {
        m_pendingDefinition = !m_path.isEmpty();
        QToolTip::showText(mapToGlobal(cursorRect().bottomLeft()),
                           "C++ 索引准备中，稍后会自动跳转", this);
        return;
    }
    if(!m_client->isReady()) {
        QToolTip::showText(mapToGlobal(cursorRect().bottomLeft()), "clangd 正在索引...", this);
    }
    const QTextCursor cursor = textCursor();
    m_definitionPath      = m_path;
    m_definitionLine      = cursor.blockNumber();
    m_definitionCharacter = cursor.positionInBlock();
    m_definitionName      = identifierAtCursor();
    QPointer<CodeEditor> self(this);
    m_client->definition(m_path, m_definitionLine, m_definitionCharacter,
                         [self](const QJsonObject & message) {
        if(self) self->handleDefinition(message);
    });
}

void CodeEditor::handleDefinition(const QJsonObject & message)
{
    editorTrace("handleDefinition result=" +
                QString::fromUtf8(QJsonDocument(message["result"].toObject()).toJson(
                    QJsonDocument::Compact)));
    QJsonValue result = message["result"];
    if(result.isArray()) {
        const QJsonArray locations = result.toArray();
        if(locations.isEmpty()) {
            requestDefinitionFallback();
            return;
        }
        result = locations.first();
    }
    QString path;
    int line   = 0;
    int column = 0;
    if(!decodeLocation(result, path, line, column)) {
        requestDefinitionFallback();
        return;
    }
    const bool currentIsHeader = QFileInfo(m_definitionPath).suffix().toLower() == "h" ||
                                 QFileInfo(m_definitionPath).suffix().toLower() == "hpp";
    if(path == m_definitionPath && currentIsHeader) {
        requestDefinitionFallback();
        return;
    }
    emit definitionTarget(path, line, column);
}

void CodeEditor::requestDefinitionFallback()
{
    if(!m_client || m_definitionName.isEmpty()) {
        QToolTip::showText(mapToGlobal(cursorRect().bottomLeft()), "未找到定义", this);
        return;
    }
    QPointer<CodeEditor> self(this);
    m_client->references(m_definitionPath, m_definitionLine, m_definitionCharacter,
                         [self](const QJsonObject & message) {
        if(self) self->handleReferences(message);
    });
}

void CodeEditor::handleReferences(const QJsonObject & message)
{
    const QJsonArray locations = message["result"].toArray();
    for(const QJsonValue & value : locations) {
        QString path;
        int line   = 0;
        int column = 0;
        if(!decodeLocation(value, path, line, column)) continue;
        if(!isSourcePath(path)) continue;
        if(path == m_definitionPath) continue;
        emit definitionTarget(path, line, column);
        return;
    }
    QPointer<CodeEditor> self(this);
    m_client->workspaceSymbol(m_definitionName, [self](const QJsonObject & message) {
        if(self) self->handleWorkspaceSymbols(message);
    });
}

void CodeEditor::handleWorkspaceSymbols(const QJsonObject & message)
{
    const QJsonArray symbols = message["result"].toArray();
    for(const QJsonValue & value : symbols) {
        const QJsonObject symbol = value.toObject();
        QString path;
        int line   = 0;
        int column = 0;
        if(!decodeLocation(symbol["location"], path, line, column)) continue;
        if(!isSourcePath(path)) continue;
        emit definitionTarget(path, line, column);
        return;
    }
    searchProjectDefinition();
}

void CodeEditor::searchProjectDefinition()
{
    if(m_projectRoot.isEmpty() || m_definitionName.isEmpty()) {
        QToolTip::showText(mapToGlobal(cursorRect().bottomLeft()), "未找到定义", this);
        return;
    }
    const QRegularExpression functionPattern(
        "\\b" + QRegularExpression::escape(m_definitionName) + "\\s*\\(");
    const QStringList filters{"*.c", "*.cpp", "*.cc", "*.cxx"};
    QDirIterator it(m_projectRoot, filters, QDir::Files, QDirIterator::Subdirectories);
    while(it.hasNext()) {
        const QString path = it.next();
        const QString relative = QDir(m_projectRoot).relativeFilePath(path);
        if(relative.contains("build") || relative.contains("toolchain") ||
           relative.contains("ui_generated") || relative.contains("/dist/"))
            continue;
        QFile file(path);
        if(!file.open(QIODevice::ReadOnly)) continue;
        if(file.size() > 4 * 1024 * 1024) continue;
        const QStringList lines = QString::fromUtf8(file.readAll()).split('\n');
        for(int i = 0; i < lines.size(); ++i) {
            const QString & line = lines.at(i);
            const auto match = functionPattern.match(line);
            if(!match.hasMatch()) continue;
            const QString trimmed = line.trimmed();
            if(trimmed.startsWith("//")) continue;
            const bool looksLikeDefinition =
                line.contains('{') || line.contains("::") ||
                (i + 1 < lines.size() && lines.at(i + 1).contains('{'));
            if(!looksLikeDefinition) continue;
            emit definitionTarget(path, i + 1, match.capturedStart() + 1);
            return;
        }
    }
    QToolTip::showText(mapToGlobal(cursorRect().bottomLeft()), "未找到定义", this);
}

void CodeEditor::gotoLine(int line, int column)
{
    const QTextBlock block = document()->findBlockByNumber(qMax(0, line - 1));
    if(!block.isValid()) return;
    QTextCursor cursor(block);
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, qMax(0, column - 1));
    setTextCursor(cursor);
    centerCursor();
    setFocus();
}

void CodeEditor::scheduleDocumentSync()
{
    if(m_syncTimer) m_syncTimer->start();
}

void CodeEditor::toggleComment()
{
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();
    const int start = cursor.selectionStart();
    const int end   = cursor.selectionEnd();
    QTextCursor first(document()->findBlock(start));
    QTextCursor last(document()->findBlock(end));
    QTextCursor it    = first;
    bool allCommented = true;
    while(true) {
        const QString text = it.block().text().trimmed();
        if(!text.startsWith("//")) {
            allCommented = false;
            break;
        }
        if(it.block() == last.block()) break;
        it.movePosition(QTextCursor::NextBlock);
    }
    it = first;
    while(true) {
        const QString blockText = it.block().text();
        const int firstNonSpace = blockText.indexOf(QRegularExpression("\\S"));
        if(firstNonSpace >= 0) {
            QTextCursor edit = it;
            edit.setPosition(it.block().position() + firstNonSpace);
            if(allCommented) {
                if(blockText.mid(firstNonSpace).startsWith("//")) {
                    edit.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 2);
                    edit.removeSelectedText();
                }
            } else {
                edit.insertText("//");
            }
        }
        if(it.block() == last.block()) break;
        it.movePosition(QTextCursor::NextBlock);
    }
    cursor.endEditBlock();
    setTextCursor(cursor);
}

void CodeEditor::indentSelection(bool outdent)
{
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();
    const int start = cursor.selectionStart();
    const int end   = cursor.selectionEnd();
    QTextCursor first(document()->findBlock(start));
    QTextCursor last(document()->findBlock(end));
    QTextCursor it = first;
    while(true) {
        QTextCursor edit = it;
        edit.movePosition(QTextCursor::StartOfBlock);
        if(outdent) {
            for(int i = 0; i < 4; ++i) {
                if(edit.block().text().mid(edit.positionInBlock()).startsWith(' '))
                    edit.deleteChar();
                else
                    break;
            }
        } else {
            edit.insertText("    ");
        }
        if(it.block() == last.block()) break;
        it.movePosition(QTextCursor::NextBlock);
    }
    cursor.endEditBlock();
    setTextCursor(cursor);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent * event)
{
    QPainter painter(m_lineNumberArea);
    painter.fillRect(event->rect(), QColor(0x25, 0x25, 0x25));

    QTextBlock block      = firstVisibleBlock();
    int blockNumber       = block.blockNumber();
    int top               = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom            = top + qRound(blockBoundingRect(block).height());
    const int currentLine = textCursor().blockNumber();

    while(block.isValid() && top <= event->rect().bottom()) {
        if(block.isVisible() && bottom >= event->rect().top()) {
            painter.setPen(blockNumber == currentLine ? QColor(0xe8, 0xe8, 0xe8) : QColor(0x8a, 0x8a, 0x8a));
            painter.drawText(0, top, m_lineNumberArea->width() - 8, fontMetrics().height(), Qt::AlignRight,
                             QString::number(blockNumber + 1));
        }
        block  = block.next();
        top    = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

int CodeEditor::lineNumberAreaWidth() const
{
    int digits = 1;
    int blocks = qMax(1, blockCount());
    while(blocks >= 10) {
        blocks /= 10;
        ++digits;
    }
    return fontMetrics().horizontalAdvance('9') * digits + 14;
}

void CodeEditor::updateLineNumberAreaWidth(int)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect & rect, int dy)
{
    if(dy)
        m_lineNumberArea->scroll(0, dy);
    else
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());
    if(rect.contains(viewport()->rect())) updateLineNumberAreaWidth(0);
}

void CodeEditor::updateExtraSelections()
{
    QList<QTextEdit::ExtraSelection> selections;

    QTextEdit::ExtraSelection current;
    current.format.setBackground(QColor(0x26, 0x26, 0x26));
    current.format.setProperty(QTextFormat::FullWidthSelection, true);
    current.cursor = textCursor();
    current.cursor.clearSelection();
    selections << current;

    const QString text = toPlainText();
    const int position = textCursor().position();
    int bracketPos     = -1;
    QChar bracket;
    for(int offset : {0, -1}) {
        const int p = position + offset;
        if(p < 0 || p >= text.length()) continue;
        if(QString("()[]{}").contains(text.at(p))) {
            bracketPos = p;
            bracket    = text.at(p);
            break;
        }
    }
    if(bracketPos >= 0) {
        const QChar bracketChar = bracket;
        const bool closing      = bracketChar == ')' || bracketChar == ']' || bracketChar == '}';
        const QChar matchChar   = closing ? (bracketChar == ')'   ? '('
                                             : bracketChar == ']' ? '['
                                                                  : '{')
                                          : (bracketChar == '('   ? ')'
                                             : bracketChar == '[' ? ']'
                                                                  : '}');
        const int direction     = closing ? -1 : 1;
        int depth               = 0;
        for(int p = bracketPos; p >= 0 && p < text.length(); p += direction) {
            if(text.at(p) == bracketChar)
                ++depth;
            else if(text.at(p) == matchChar) {
                --depth;
                if(depth == 0) {
                    for(int target : {bracketPos, p}) {
                        QTextEdit::ExtraSelection match;
                        match.format.setBackground(QColor(0x3c, 0x5a, 0x78));
                        QTextCursor c(document());
                        c.setPosition(target);
                        c.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
                        match.cursor = c;
                        selections << match;
                    }
                    break;
                }
            }
        }
    }
    setExtraSelections(selections);
}

void CodeEditor::updateTabStopDistance()
{
    setTabStopDistance(fontMetrics().horizontalAdvance(' ') * 4);
}

void CodeEditor::openFindBar(bool replace)
{
    if(!m_findBar) {
        m_findBar     = new QWidget(this);
        auto * layout = new QHBoxLayout(m_findBar);
        layout->setContentsMargins(6, 4, 6, 4);
        layout->setSpacing(4);
        m_findEdit = new QLineEdit(m_findBar);
        m_findEdit->setPlaceholderText("查找");
        m_findEdit->setMinimumWidth(180);
        m_replaceEdit = new QLineEdit(m_findBar);
        m_replaceEdit->setPlaceholderText("替换为");
        m_replaceEdit->setMinimumWidth(150);
        auto * prev             = new QPushButton("上一个", m_findBar);
        auto * next             = new QPushButton("下一个", m_findBar);
        auto * replaceOne       = new QPushButton("替换", m_findBar);
        auto * replaceAllButton = new QPushButton("全部", m_findBar);
        auto * close            = new QToolButton(m_findBar);
        close->setText("×");
        layout->addWidget(m_findEdit);
        layout->addWidget(m_replaceEdit);
        layout->addWidget(prev);
        layout->addWidget(next);
        layout->addWidget(replaceOne);
        layout->addWidget(replaceAllButton);
        layout->addWidget(close);
        m_findBar->setStyleSheet("QWidget { background:#2b2b2b; border:1px solid #555; } "
                                 "QLineEdit { background:#1f1f1f; border:1px solid #444; padding:3px; } "
                                 "QPushButton, QToolButton { background:#333; border:1px solid #4a4a4a; "
                                 "padding:3px 8px; }");
        connect(m_findEdit, &QLineEdit::returnPressed, this, [this] { findNext(false); });
        connect(prev, &QPushButton::clicked, this, [this] { findNext(true); });
        connect(next, &QPushButton::clicked, this, [this] { findNext(false); });
        connect(replaceOne, &QPushButton::clicked, this, [this] { replaceCurrent(); });
        connect(replaceAllButton, &QPushButton::clicked, this, [this] { replaceAll(); });
        connect(close, &QToolButton::clicked, this, [this] { m_findBar->hide(); });
    }
    m_replaceMode = replace;
    m_replaceEdit->setVisible(replace);
    m_findBar->adjustSize();
    positionFindBar();
    m_findBar->show();
    m_findBar->raise();
    m_findEdit->setFocus();
    m_findEdit->selectAll();
}

void CodeEditor::positionFindBar()
{
    if(!m_findBar) return;
    m_findBar->adjustSize();
    m_findBar->move(qMax(8, width() - m_findBar->width() - 18), 8);
}

void CodeEditor::findNext(bool backward)
{
    if(!m_findEdit) return;
    const QString needle = m_findEdit->text();
    if(needle.isEmpty()) return;
    QTextDocument::FindFlags flags;
    if(backward) flags |= QTextDocument::FindBackward;
    QTextCursor found = document()->find(needle, textCursor(), flags);
    if(found.isNull()) {
        QTextCursor start(document());
        start.movePosition(backward ? QTextCursor::End : QTextCursor::Start);
        found = document()->find(needle, start, flags);
    }
    if(!found.isNull()) {
        setTextCursor(found);
        centerCursor();
    }
}

void CodeEditor::replaceCurrent()
{
    if(!m_findEdit || !m_replaceEdit) return;
    QTextCursor cursor = textCursor();
    if(cursor.hasSelection() && cursor.selectedText() == m_findEdit->text()) {
        cursor.insertText(m_replaceEdit->text());
    }
    findNext(false);
}

void CodeEditor::replaceAll()
{
    if(!m_findEdit || !m_replaceEdit) return;
    const QString needle = m_findEdit->text();
    if(needle.isEmpty()) return;
    QTextCursor cursor(document());
    cursor.beginEditBlock();
    while(true) {
        QTextCursor found = document()->find(needle, cursor);
        if(found.isNull()) break;
        found.insertText(m_replaceEdit->text());
        cursor = found;
    }
    cursor.endEditBlock();
}

void CodeEditor::keyPressEvent(QKeyEvent * event)
{
    editorTrace("key=" + QString::number(event->key()) + " text=" + event->text() +
                " mods=" + QString::number(static_cast<int>(event->modifiers())));
    if(completionVisible()) {
        if(event->key() == Qt::Key_Escape) {
            hideCompletion();
            return;
        }
        if(event->key() == Qt::Key_Down || event->key() == Qt::Key_Up) {
            const int row = m_completion->currentRow() + (event->key() == Qt::Key_Down ? 1 : -1);
            if(row >= 0 && row < m_completion->count()) m_completion->setCurrentRow(row);
            return;
        }
        if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Tab) {
            acceptCompletion();
            return;
        }
        hideCompletion();
    }

    if(event->key() == Qt::Key_F12) {
        requestDefinition();
        return;
    }
    if(event->matches(QKeySequence::Find)) {
        openFindBar(false);
        return;
    }
    if(event->matches(QKeySequence::Replace)) {
        openFindBar(true);
        return;
    }
    if(event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_Slash) {
        toggleComment();
        return;
    }
    if(event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_Space) {
        requestCompletion();
        return;
    }
    if(event->key() == Qt::Key_Tab) {
        if(textCursor().hasSelection())
            indentSelection(false);
        else {
            const int column = textCursor().positionInBlock();
            insertPlainText(QString(4 - column % 4, ' '));
        }
        return;
    }
    if(event->key() == Qt::Key_Backtab) {
        indentSelection(true);
        return;
    }
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        const QString line = textCursor().block().text();
        QString indent;
        for(QChar ch : line) {
            if(ch == ' ' || ch == '\t')
                indent += ch;
            else
                break;
        }
        const int pos = textCursor().positionInBlock();
        if(pos > 0 && line.at(pos - 1) == '{') indent += "    ";
        insertPlainText("\n" + indent);
        ensureCursorVisible();
        return;
    }
    if(event->key() == Qt::Key_Backspace) {
        QTextCursor cursor = textCursor();
        if(!cursor.hasSelection()) {
            const QString before = cursor.block().text().left(cursor.positionInBlock());
            if(!before.isEmpty() && before.trimmed().isEmpty()) {
                const int remove = qMin(4, static_cast<int>(before.length()));
                for(int i = 0; i < remove; ++i) cursor.deletePreviousChar();
                return;
            }
        }
    }

    QPlainTextEdit::keyPressEvent(event);
    if(event->text() == "." || event->text() == ">") {
        QTimer::singleShot(80, this, [this] { requestCompletion(); });
    }
}

void CodeEditor::resizeEvent(QResizeEvent * event)
{
    QPlainTextEdit::resizeEvent(event);
    const QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
    positionFindBar();
}

void CodeEditor::mousePressEvent(QMouseEvent * event)
{
    if(event->button() == Qt::BackButton) {
        emit navigationBackRequested();
        return;
    }
    if(event->button() == Qt::ForwardButton) {
        emit navigationForwardRequested();
        return;
    }
    QPlainTextEdit::mousePressEvent(event);
}

void CodeEditor::mouseReleaseEvent(QMouseEvent * event)
{
    if(event->button() == Qt::LeftButton && (event->modifiers() & Qt::ControlModifier)) {
        requestDefinition();
        return;
    }
    QPlainTextEdit::mouseReleaseEvent(event);
}

void CodeEditor::contextMenuEvent(QContextMenuEvent * event)
{
    QMenu * menu = createStandardContextMenu();
    menu->addSeparator();
    menu->addAction("转到定义\tF12", this, &CodeEditor::requestDefinition);
    menu->addAction("注释/取消注释\tCtrl+/", this, &CodeEditor::toggleComment);
    menu->addAction("复制当前行", this, &CodeEditor::duplicateLine);
    menu->addAction("删除当前行", this, &CodeEditor::deleteLine);
    menu->addSeparator();
    menu->addAction("查找\tCtrl+F", this, [this] { openFindBar(false); });
    menu->addAction("替换\tCtrl+H", this, [this] { openFindBar(true); });
    if(!m_path.isEmpty()) menu->addAction("保存\tCtrl+S", this, [this] { save(); });
    menu->exec(event->globalPos());
    delete menu;
    event->accept();
}

void CodeEditor::duplicateLine()
{
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();
    const QString line = cursor.block().text();
    cursor.movePosition(QTextCursor::EndOfBlock);
    cursor.insertText("\n" + line);
    cursor.endEditBlock();
    setTextCursor(cursor);
}

void CodeEditor::deleteLine()
{
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();
    cursor.select(QTextCursor::BlockUnderCursor);
    cursor.removeSelectedText();
    if(cursor.block().next().isValid()) cursor.deleteChar();
    cursor.endEditBlock();
    setTextCursor(cursor);
}

void CodeEditor::wheelEvent(QWheelEvent * event)
{
    if(event->modifiers() & Qt::ControlModifier) {
        int delta = event->angleDelta().y();
        if(delta == 0) delta = event->pixelDelta().y();
        if(delta != 0) zoomFont(delta > 0 ? 1 : -1);
        event->accept();
        return;
    }
    QPlainTextEdit::wheelEvent(event);
}

bool CodeEditor::viewportEvent(QEvent * event)
{
    if(event->type() == QEvent::Wheel) {
        auto * wheel = static_cast<QWheelEvent *>(event);
        if(wheel->modifiers() & Qt::ControlModifier) {
            int delta = wheel->angleDelta().y();
            if(delta == 0) delta = wheel->pixelDelta().y();
            if(delta != 0) zoomFont(delta > 0 ? 1 : -1);
            return true;
        }
    }
    return QPlainTextEdit::viewportEvent(event);
}

void CodeEditor::zoomFont(int steps)
{
    const int newSize = qBound(7, fontPointSize() + steps, 32);
    if(newSize == fontPointSize()) return;
    setFontPointSize(newSize);
    emit fontSizeChanged(newSize);
}

void CodeEditor::setFontPointSize(int size)
{
    const int newSize = qBound(7, size, 32);
    if(newSize == fontPointSize()) return;

    QFont current = font();
    current.setPointSize(newSize);
    setFont(current);
    updateTabStopDistance();
    updateLineNumberAreaWidth(0);

    const QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
    QSettings().setValue("editor/fontPointSize", newSize);
    ensureCursorVisible();
}

int CodeEditor::fontPointSize() const
{
    return font().pointSize() > 0 ? font().pointSize() : 10;
}
