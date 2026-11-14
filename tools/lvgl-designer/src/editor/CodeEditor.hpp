#pragma once

#include <QPlainTextEdit>
#include <QString>

class CodeEditor : public QPlainTextEdit {
    Q_OBJECT
public:
    explicit CodeEditor(QWidget* parent = nullptr);

    bool loadFile(const QString& path, QString* error = nullptr);
    bool save();
    bool saveAs(const QString& path);
    QString filePath() const { return m_path; }

signals:
    void saved(const QString& path);

private:
    QString m_path;
    class QSyntaxHighlighter* m_highlighter = nullptr;
};
