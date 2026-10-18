/**************************************************************************************************
 * @file        MainWindow.hpp
 * @brief       EasyLVGL 设计器
 * @details     EasyLVGL —— 让 LVGL 开发像 Qt 一样简单：C++ 封装 + 可视化设计器 + 代码生成。
 *
 * @author      taoist.luo
 * @version     1.0.0
 * @date        2026-10-18
 *
 * Copyright (c) 2026 taoist.luo (https://github.com/TaoistLuo/EasyLVGL)
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *************************************************************************************************/

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

    QDockWidget* m_compDock = nullptr;
    QDockWidget* m_leftDock = nullptr;
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
    bool updateManifestResources(const std::string& alias, const std::string& rel);
    std::map<std::string, std::string> m_projectResources;

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
    void onBuildAndRun();
    void onApplyLayoutVBox();
    void onApplyLayoutHBox();
    void onApplyLayoutGrid();
    void onApplyLayoutStacked();
    void onBreakLayout();
    void applyLayoutToSelection(const std::string& layoutType);
};

}
