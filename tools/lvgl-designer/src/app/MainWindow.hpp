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
#include <vector>
#include "model/LuiDocument.hpp"
#include "canvas/SDL2Widget.hpp"
#include "canvas/LvglRenderer.hpp"
#include "panels/ComponentPanel.hpp"
#include "panels/PropertyPanel.hpp"
#include "panels/TreePanel.hpp"

namespace designer {

class MainWindow : public QMainWindow {
    Q_OBJECT

    LuiDocument m_doc;
    SDL2Widget* m_lvglWidget;
    LvglRenderer m_renderer;
    ComponentPanel* m_compPanel;
    PropertyPanel* m_propPanel;
    TreePanel* m_treePanel;
    QUndoStack* m_undoStack;
    std::string m_selectedId;
    std::vector<std::string> m_multiSelectedIds;

    QDockWidget* m_compDock;
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
    void onDelete();
    void onCanvasSize();
    void onCompile();
    void onAbout();
    void onBringToFront();
    void scheduleRebuild();
    void onSendToBack();
    void onContextMenu(const QPoint& pos);
    bool confirmDiscard();

    void onApplyLayoutVBox();
    void onApplyLayoutHBox();
    void onApplyLayoutGrid();
    void onApplyLayoutForm();
    void onApplyLayoutStacked();
    void onBreakLayout();
    void applyLayoutToSelection(const std::string& layoutType);
};

}
