#pragma once

#include <QPlainTextEdit>
#include <QJsonObject>
#include <QString>

class ClangdClient;
class QContextMenuEvent;
class QLineEdit;
class QListWidget;
class QTimer;
class QWheelEvent;
class QWidget;

class CodeEditor : public QPlainTextEdit {
    Q_OBJECT
  public:
    explicit CodeEditor(QWidget * parent = nullptr);
    ~CodeEditor() override;

    bool loadFile(const QString & path, QString * error = nullptr);
    bool save();
    bool saveAs(const QString & path);
    QString filePath() const
    {
        return m_path;
    }

    void setLanguageClient(ClangdClient * client);
    void setProjectRoot(const QString & root);
    void gotoLine(int line, int column = 0);
    void setFontPointSize(int size);
    int fontPointSize() const;

  signals:
    void saved(const QString & path);
    void definitionTarget(const QString & path, int line, int column);
    void navigationBackRequested();
    void navigationForwardRequested();
    void fontSizeChanged(int size);

  private:
    class LineNumberArea;

    void lineNumberAreaPaintEvent(QPaintEvent * event);
    int lineNumberAreaWidth() const;
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect & rect, int dy);
    void updateExtraSelections();
    void updateTabStopDistance();

    void scheduleDocumentSync();
    void requestCompletion();
    void showCompletion(const QJsonObject & message);
    void acceptCompletion();
    void hideCompletion();
    bool completionVisible() const;
    void requestDefinition();
    void handleDefinition(const QJsonObject & message);
    void requestDefinitionFallback();
    void handleReferences(const QJsonObject & message);
    void handleWorkspaceSymbols(const QJsonObject & message);
    void searchProjectDefinition();
    QString identifierAtCursor() const;

    void toggleComment();
    void indentSelection(bool outdent);
    void openFindBar(bool replace);
    void positionFindBar();
    void findNext(bool backward);
    void replaceCurrent();
    void replaceAll();
    void duplicateLine();
    void deleteLine();

    void keyPressEvent(QKeyEvent * event) override;
    void resizeEvent(QResizeEvent * event) override;
    void mousePressEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    void wheelEvent(QWheelEvent * event) override;
    bool viewportEvent(QEvent * event) override;
    void contextMenuEvent(QContextMenuEvent * event) override;
    void zoomFont(int steps);

    QString m_path;
    QString m_projectRoot;
    QString m_definitionPath;
    QString m_definitionName;
    int m_definitionLine = 0;
    int m_definitionCharacter = 0;
    class QSyntaxHighlighter * m_highlighter = nullptr;
    ClangdClient * m_client                  = nullptr;
    QTimer * m_syncTimer                     = nullptr;
    QListWidget * m_completion               = nullptr;
    QWidget * m_findBar                      = nullptr;
    QLineEdit * m_findEdit                   = nullptr;
    QLineEdit * m_replaceEdit                = nullptr;
    QWidget * m_lineNumberArea               = nullptr;
    bool m_replaceMode                       = false;
    bool m_pendingDefinition                 = false;
};
