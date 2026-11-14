#include "CodeEditor.hpp"

#include <QFile>
#include <QFileInfo>
#include <QFontDatabase>
#include <QRegularExpression>
#include <QSaveFile>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextDocument>

namespace {

class CppHighlighter : public QSyntaxHighlighter {
public:
    explicit CppHighlighter(QTextDocument* parent) : QSyntaxHighlighter(parent) {
        QTextCharFormat keyword;
        keyword.setForeground(QColor(0x56, 0x9C, 0xD6));
        keyword.setFontWeight(QFont::Bold);
        const QStringList keywords = {
            "alignas", "alignof", "and", "asm", "auto", "break", "case", "catch", "class",
            "const", "constexpr", "continue", "default", "delete", "do", "else", "enum",
            "explicit", "export", "extern", "false", "for", "friend", "goto", "if", "inline",
            "mutable", "namespace", "new", "noexcept", "not", "nullptr", "operator", "or",
            "private", "protected", "public", "register", "return", "sizeof", "static",
            "struct", "switch", "template", "this", "throw", "true", "try", "typedef",
            "typename", "union", "using", "virtual", "volatile", "while", "override", "final",
        };
        for (const QString& word : keywords)
            m_rules.push_back({QRegularExpression("\\b" + word + "\\b"), keyword});

        QTextCharFormat type;
        type.setForeground(QColor(0x4E, 0xC9, 0xB0));
        const QStringList types = {
            "bool", "char", "double", "float", "int", "long", "short", "signed", "unsigned",
            "void", "wchar_t", "size_t", "int8_t", "int16_t", "int32_t", "int64_t",
            "uint8_t", "uint16_t", "uint32_t", "uint64_t", "lv_obj_t", "lv_event_t",
        };
        for (const QString& word : types)
            m_rules.push_back({QRegularExpression("\\b" + word + "\\b"), type});

        QTextCharFormat preprocessor;
        preprocessor.setForeground(QColor(0xC5, 0x86, 0xC0));
        m_rules.push_back({QRegularExpression("^\\s*#\\s*\\w+"), preprocessor});

        QTextCharFormat number;
        number.setForeground(QColor(0xB5, 0xCE, 0xA8));
        m_rules.push_back({QRegularExpression("\\b(0[xX][0-9A-Fa-f]+|\\d+(\\.\\d+)?)\\b"), number});

        QTextCharFormat string;
        string.setForeground(QColor(0xCE, 0x91, 0x78));
        m_rules.push_back({QRegularExpression("\"(?:\\\\.|[^\"\\\\])*\""), string});
        m_rules.push_back({QRegularExpression("'(?:\\\\.|[^'\\\\])*'"), string});

        QTextCharFormat function;
        function.setForeground(QColor(0xDC, 0xDC, 0xAA));
        m_rules.push_back({QRegularExpression("\\b[A-Za-z_]\\w*(?=\\s*\\()"), function});

        m_commentFormat.setForeground(QColor(0x6A, 0x99, 0x55));
        m_blockStart = QRegularExpression("/\\*");
        m_blockEnd = QRegularExpression("\\*/");
    }

protected:
    void highlightBlock(const QString& text) override {
        for (const auto& rule : m_rules) {
            auto it = rule.pattern.globalMatch(text);
            while (it.hasNext()) {
                auto match = it.next();
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }

        setCurrentBlockState(0);
        int start = 0;
        if (previousBlockState() != 1) {
            auto match = m_blockStart.match(text);
            start = match.hasMatch() ? match.capturedStart() : -1;
        }
        while (start >= 0) {
            auto endMatch = m_blockEnd.match(text, start);
            int length = 0;
            if (!endMatch.hasMatch()) {
                setCurrentBlockState(1);
                length = text.length() - start;
            } else {
                length = endMatch.capturedEnd() - start;
            }
            setFormat(start, length, m_commentFormat);
            if (!endMatch.hasMatch()) break;
            auto next = m_blockStart.match(text, start + length);
            start = next.hasMatch() ? next.capturedStart() : -1;
        }

        int lineComment = text.indexOf("//");
        if (lineComment >= 0) setFormat(lineComment, text.length() - lineComment, m_commentFormat);
    }

private:
    struct Rule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<Rule> m_rules;
    QRegularExpression m_blockStart;
    QRegularExpression m_blockEnd;
    QTextCharFormat m_commentFormat;
};

}  // namespace

CodeEditor::CodeEditor(QWidget* parent) : QPlainTextEdit(parent) {
    setLineWrapMode(QPlainTextEdit::NoWrap);
    setTabStopDistance(fontMetrics().horizontalAdvance(' ') * 4);
    QFont mono = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    mono.setPointSize(10);
    setFont(mono);
    QPalette colors = palette();
    colors.setColor(QPalette::Base, QColor(0x1e, 0x1e, 0x1e));
    colors.setColor(QPalette::Text, QColor(0xd4, 0xd4, 0xd4));
    colors.setColor(QPalette::Highlight, QColor(0x2f, 0x5d, 0x8a));
    colors.setColor(QPalette::HighlightedText, Qt::white);
    setPalette(colors);
    new CppHighlighter(document());
}

bool CodeEditor::loadFile(const QString& path, QString* error) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        if (error) *error = file.errorString();
        return false;
    }
    setPlainText(QString::fromUtf8(file.readAll()));
    m_path = QFileInfo(path).absoluteFilePath();
    document()->setModified(false);
    return true;
}

bool CodeEditor::save() {
    if (m_path.isEmpty()) return false;
    QSaveFile file(m_path);
    if (!file.open(QIODevice::WriteOnly)) return false;
    file.write(toPlainText().toUtf8());
    if (!file.commit()) return false;
    document()->setModified(false);
    return true;
}

bool CodeEditor::saveAs(const QString& path) {
    QFileInfo info(path);
    m_path = info.absoluteFilePath();
    return save();
}
