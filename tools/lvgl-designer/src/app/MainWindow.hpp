#pragma once
#include <QMainWindow>
#include <QDockWidget>
#include <QUndoStack>
#include <QMenuBar>
#include <QToolBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QShortcut>
#include <QProcess>
#include <QDir>
#include <QFileInfo>
#include <QLabel>
#include <QCloseEvent>
#include <QTimer>
#include <optional>
#include <memory>
#include <map>
#include <string>
#include <utility>
#include <functional>
#include <vector>
#include "model/LuiDocument.hpp"
#include "canvas/SDL2Widget.hpp"
#include "canvas/LvglRenderer.hpp"
#include "panels/ComponentPanel.hpp"
#include "panels/PropertyPanel.hpp"
#include "panels/TreePanel.hpp"
#include "panels/ProjectPanel.hpp"
#include "app/SettingsDialog.hpp"
#include "panels/OutputPanel.hpp"

namespace designer {

class MainWindow : public QMainWindow {
    Q_OBJECT

    LuiDocument m_doc;
    SDL2Widget* m_lvglWidget;
    LvglRenderer m_renderer;
    ComponentPanel* m_compPanel;
    PropertyPanel* m_propPanel;
    TreePanel* m_treePanel;
    ProjectPanel* m_projectPanel;
    OutputPanel* m_buildPanel;   // 编译/构建输出
    OutputPanel* m_runPanel;     // 运行输出
    QDockWidget* m_buildDock;
    QDockWidget* m_runDock;
    QProcess* m_runProc = nullptr;
    QUndoStack* m_undoStack;
    std::string m_selectedId;
    std::vector<std::string> m_multiSelectedIds;

    /* ---- 工程（组件化） ---- */
    QString m_projectFile;                                     // 工程清单路径
    std::vector<std::unique_ptr<LuiDocument>> m_projectDocs;   // 工程内其他文档
    std::map<std::string, LuiDocument*> m_componentDocs;       // 名字 -> 文档
    std::vector<std::pair<std::string, QString>> m_projectComponents;  // (名字, 路径)

    QDockWidget* m_compDock;
    QDockWidget* m_projectDock;
    QDockWidget* m_treeDock;
    QDockWidget* m_propDock;
    QLabel* m_statusInfo;
    std::optional<WidgetNode> m_clipboard;
    std::string m_moveResizeId;
    QPointF m_moveResizeOldPos;
    QSizeF m_moveResizeOldSize;
    bool m_rebuildScheduled = false;

public:
    explicit MainWindow(QWidget* parent = nullptr);
    bool openFile(const QString& path);
    void loadProject(const QString& manifest);
    void onNewProject();
    void onNewComponent();
    void onNewScreen();
    void onAddProjectFiles();
    void removeFromProject(const QString& path);
    void refreshProjectPanel();
    bool updateManifestFiles(const std::function<void(QJsonArray&)>& fn);

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void setupMenus();
    void setupToolbar();
    void setupDocks();
    void setupConnections();
    void updateTitle();
    void updateStatusBar();
    void rebuildFromDocument();

    void onNew();
    void onOpen();
    void onSave();
    void onSaveAs();
    void onClose();
    void onExit();
    void onCut();
    void onCopy();
    void onPaste();
    void onSelectAll();
    void onAddImageResource();
    void onDelete();
    void onCanvasSize();
    void onCompile();
    void onAbout();
    void onBringToFront();
    void scheduleRebuild();
    void onSendToBack();
    void onContextMenu(const QPoint& pos);
    bool confirmDiscard();

    void onOpenProject();
    void onSettings();
    void onBuildProject();
    void onRunProject();
    void onApplyLayoutVBox();
    void onApplyLayoutHBox();
    void onApplyLayoutGrid();
    void onApplyLayoutStacked();
    void onBreakLayout();
    void applyLayoutToSelection(const std::string& layoutType);
};

}
