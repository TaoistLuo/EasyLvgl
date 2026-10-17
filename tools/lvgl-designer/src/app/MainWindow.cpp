#include "app/MainWindow.hpp"
#include "commands/UndoCommands.hpp"
#include <QApplication>
#include <QStatusBar>
#include <QKeySequence>
#include <QInputDialog>
#include <QClipboard>
#include <QTimer>
#include <algorithm>
#include <set>

namespace designer {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setWindowTitle("LVGL++ Designer");
    resize(1400, 900);

    m_undoStack = new QUndoStack(this);

    m_lvglWidget = new SDL2Widget(this);
    setCentralWidget(m_lvglWidget);
    m_lvglWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    m_compPanel = new ComponentPanel(this);
    m_propPanel = new PropertyPanel(this);
    m_treePanel = new TreePanel(this);

    setupDocks();
    setupMenus();
    setupToolbar();
    setupConnections();

    m_propPanel->setDocument(&m_doc);
    m_propPanel->setUndoStack(m_undoStack);
    m_treePanel->setDocument(&m_doc);

    m_statusInfo = new QLabel("Ready");
    statusBar()->addWidget(m_statusInfo, 1);
}

bool MainWindow::openFile(const QString& path) {
    if (!m_doc.load(path)) {
        QMessageBox::critical(this, "Error", "Failed to open: " + path);
        return false;
    }

    m_undoStack->clear();
    m_selectedId.clear();
    m_multiSelectedIds.clear();
    m_clipboard.reset();
    m_propPanel->setDocument(&m_doc);
    m_treePanel->setDocument(&m_doc);
    updateTitle();
    rebuildFromDocument();
    updateStatusBar();
    return true;
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (m_doc.isModified() && !confirmDiscard()) {
        event->ignore();
        return;
    }
    event->accept();
}

void MainWindow::rebuildFromDocument() {
    if (!m_lvglWidget->display()) return;
    m_renderer.buildTree(m_doc);
    m_treePanel->rebuild();
}

void MainWindow::scheduleRebuild() {
    if (m_rebuildScheduled) return;
    m_rebuildScheduled = true;
    QTimer::singleShot(0, this, [this]() {
        m_rebuildScheduled = false;
        rebuildFromDocument();
        if (!m_selectedId.empty()) {
            m_renderer.selectObject(m_selectedId);
        }
    });
}

void MainWindow::setupMenus() {
    auto* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&New", this, &MainWindow::onNew, QKeySequence("Ctrl+N"));
    fileMenu->addAction("&Open...", this, &MainWindow::onOpen, QKeySequence::Open);
    fileMenu->addAction("&Save", this, &MainWindow::onSave, QKeySequence::Save);
    fileMenu->addAction("Save &As...", this, &MainWindow::onSaveAs, QKeySequence("Ctrl+Shift+S"));
    fileMenu->addSeparator();
    fileMenu->addAction("&Close", this, &MainWindow::onClose, QKeySequence("Ctrl+W"));
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, &MainWindow::onExit, QKeySequence::Quit);

    auto* editMenu = menuBar()->addMenu("&Edit");
    auto* undoAct = m_undoStack->createUndoAction(this, "&Undo");
    undoAct->setShortcut(QKeySequence::Undo);
    editMenu->addAction(undoAct);
    auto* redoAct = m_undoStack->createRedoAction(this, "&Redo");
    redoAct->setShortcut(QKeySequence::Redo);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction("Cu&t", this, &MainWindow::onCut, QKeySequence::Cut);
    editMenu->addAction("&Copy", this, &MainWindow::onCopy, QKeySequence::Copy);
    editMenu->addAction("&Paste", this, &MainWindow::onPaste, QKeySequence::Paste);
    editMenu->addSeparator();
    editMenu->addAction("&Delete", this, &MainWindow::onDelete, QKeySequence::Delete);
    editMenu->addSeparator();
    editMenu->addAction("Select &All", this, &MainWindow::onSelectAll, QKeySequence::SelectAll);

    auto* viewMenu = menuBar()->addMenu("&View");
    m_compDock->toggleViewAction()->setText("&Components");
    viewMenu->addAction(m_compDock->toggleViewAction());
    m_treeDock->toggleViewAction()->setText("Widget &Tree");
    viewMenu->addAction(m_treeDock->toggleViewAction());
    m_propDock->toggleViewAction()->setText("&Properties");
    viewMenu->addAction(m_propDock->toggleViewAction());
    viewMenu->addSeparator();
    auto* statusToggle = viewMenu->addAction("&Status Bar");
    statusToggle->setCheckable(true);
    statusToggle->setChecked(true);
    connect(statusToggle, &QAction::toggled, statusBar(), &QWidget::setVisible);
    viewMenu->addSeparator();
    viewMenu->addAction("Canvas &Size...", this, &MainWindow::onCanvasSize);

    auto* buildMenu = menuBar()->addMenu("&Build");
    buildMenu->addAction("&Compile .lui", this, &MainWindow::onCompile, QKeySequence("F5"));

    auto* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("&About", this, &MainWindow::onAbout);
    helpMenu->addAction("About &Qt", this, &QApplication::aboutQt);
}

void MainWindow::setupToolbar() {
    auto* toolbar = addToolBar("Main");
    toolbar->setMovable(false);
    toolbar->addAction("New", this, &MainWindow::onNew);
    toolbar->addAction("Open", this, &MainWindow::onOpen);
    toolbar->addAction("Save", this, &MainWindow::onSave);
    toolbar->addSeparator();
    toolbar->addAction("Compile", this, &MainWindow::onCompile);
    toolbar->addSeparator();
    toolbar->addAction("Cut", this, &MainWindow::onCut);
    toolbar->addAction("Copy", this, &MainWindow::onCopy);
    toolbar->addAction("Paste", this, &MainWindow::onPaste);
    toolbar->addAction("Delete", this, &MainWindow::onDelete);
    toolbar->addSeparator();
    toolbar->addAction(m_undoStack->createUndoAction(this, "Undo"));
    toolbar->addAction(m_undoStack->createRedoAction(this, "Redo"));
    toolbar->addSeparator();
    toolbar->addAction("VBox", this, &MainWindow::onApplyLayoutVBox);
    toolbar->addAction("HBox", this, &MainWindow::onApplyLayoutHBox);
    toolbar->addAction("Grid", this, &MainWindow::onApplyLayoutGrid);
    toolbar->addAction("Stacked", this, &MainWindow::onApplyLayoutStacked);
    toolbar->addSeparator();
    toolbar->addAction("Break Layout", this, &MainWindow::onBreakLayout);
}

void MainWindow::setupDocks() {
    m_compDock = new QDockWidget("Components", this);
    m_compDock->setWidget(m_compPanel);
    m_compDock->setFeatures(QDockWidget::DockWidgetMovable);
    addDockWidget(Qt::LeftDockWidgetArea, m_compDock);

    m_treeDock = new QDockWidget("Widget Tree", this);
    m_treeDock->setWidget(m_treePanel);
    m_treeDock->setFeatures(QDockWidget::DockWidgetMovable);
    addDockWidget(Qt::LeftDockWidgetArea, m_treeDock);

    tabifyDockWidget(m_compDock, m_treeDock);
    m_compDock->raise();

    m_propDock = new QDockWidget("Properties", this);
    m_propDock->setWidget(m_propPanel);
    m_propDock->setFeatures(QDockWidget::DockWidgetMovable);
    m_propDock->setMinimumWidth(300);
    addDockWidget(Qt::RightDockWidgetArea, m_propDock);
}

void MainWindow::setupConnections() {
    connect(m_lvglWidget, &SDL2Widget::lvglReady, this, [this]() {
        m_renderer.setOnClick([this](const std::string& id) {
            m_selectedId = id;
            m_renderer.selectObject(id);
            m_propPanel->showWidget(m_doc.findById(id));
            updateStatusBar();
        });

        m_renderer.setOnMoved([this](const std::string& id, int x, int y, int w, int h) {
            auto* node = m_doc.findById(id);
            if (!node) return;

            QPointF newPos(x, y);
            QSizeF newSize(w, h);
            if (m_moveResizeId != id) {
                m_moveResizeId = id;
                m_moveResizeOldPos = node->pos();
                m_moveResizeOldSize = node->size();
            }

            auto rebuildCb = [this]() {
                scheduleRebuild();
            };

            m_undoStack->push(new MoveResizeCommand(&m_doc, rebuildCb, id,
                                                    m_moveResizeOldPos, newPos,
                                                    m_moveResizeOldSize, newSize));

            m_moveResizeId.clear();
            m_doc.markModified();
            updateTitle();
            updateStatusBar();
        });

        rebuildFromDocument();

        // Poll LVGL pending events (clicks, moves) after each LVGL tick
        auto* pollTimer = new QTimer(this);
        connect(pollTimer, &QTimer::timeout, this, [this]() {
            m_renderer.pollEvents();
        });
        pollTimer->start(5);
    });

    connect(m_treePanel, &TreePanel::widgetSelected, this, [this](const std::string& id) {
        m_selectedId = id;
        m_renderer.selectObject(id);
        m_propPanel->showWidget(m_doc.findById(id));
        updateStatusBar();
    });

    connect(m_propPanel, &PropertyPanel::documentModified, this, [this]() {
        updateTitle();
        scheduleRebuild();
    });

    m_propPanel->setRebuildCallback([this]() {
        scheduleRebuild();
    });

    connect(m_lvglWidget, &SDL2Widget::widgetDropped, this, [this](const QString& widgetType, int x, int y) {
        WidgetNode node;
        node.type = widgetType.toStdString();
        node.id = m_doc.generateUniqueId(node.type);
        node.initDefaultProps();

        std::string parentId = m_renderer.findContainerAt(x, y);
        node.parentId = parentId;

        int relX = x, relY = y;
        if (!parentId.empty()) m_renderer.screenToRelative(parentId, relX, relY);

        QVariantList pos;
        pos.append(QVariant(relX));
        pos.append(QVariant(relY));
        node.props["pos"] = QVariant(pos);

        std::string newId = node.id;
        m_undoStack->push(new AddWidgetCommand(&m_doc, [this]() { scheduleRebuild(); }, std::move(node)));
        m_selectedId = newId;
        m_propPanel->showWidget(m_doc.findById(newId));
        updateTitle();
        updateStatusBar();
    });

    connect(m_lvglWidget, &SDL2Widget::customContextMenuRequested, this, &MainWindow::onContextMenu);
}

void MainWindow::updateTitle() {
    QString title = "LVGL++ Designer";
    if (!m_doc.filePath().isEmpty()) title += " - " + m_doc.filePath();
    if (m_doc.isModified()) title += " *";
    setWindowTitle(title);
}

void MainWindow::updateStatusBar() {
    if (m_selectedId.empty()) {
        m_statusInfo->setText(QString("Ready | Canvas: %1x%2").arg(m_doc.width()).arg(m_doc.height()));
        return;
    }
    auto* node = m_doc.findById(m_selectedId);
    if (!node) {
        m_statusInfo->setText(QString("Ready | Canvas: %1x%2").arg(m_doc.width()).arg(m_doc.height()));
        return;
    }
    auto p = node->pos();
    auto s = node->size();
    m_statusInfo->setText(QString("Selected: %1 (%2) | Pos: %3,%4 | Size: %5x%6 | Canvas: %7x%8")
        .arg(QString::fromStdString(node->id))
        .arg(QString::fromStdString(node->type))
        .arg((int)p.x()).arg((int)p.y())
        .arg((int)s.width()).arg((int)s.height())
        .arg(m_doc.width()).arg(m_doc.height()));
}

void MainWindow::onNew() {
    if (m_doc.isModified() && !confirmDiscard()) return;
    m_doc.createNew();
    m_undoStack->clear();
    m_selectedId.clear();
    m_multiSelectedIds.clear();
    m_clipboard.reset();
    m_propPanel->setDocument(&m_doc);
    m_treePanel->setDocument(&m_doc);
    updateTitle();
    rebuildFromDocument();
    updateStatusBar();
}

void MainWindow::onOpen() {
    if (m_doc.isModified() && !confirmDiscard()) return;
    QString path = QFileDialog::getOpenFileName(this, "Open .lui file", QString(), "LUI Files (*.lui);;All Files (*)");
    if (!path.isEmpty()) openFile(path);
}

void MainWindow::onSave() {
    if (m_doc.filePath().isEmpty()) {
        onSaveAs();
        return;
    }
    if (m_doc.save()) {
        updateTitle();
        statusBar()->showMessage("Saved", 3000);
    } else {
        QMessageBox::critical(this, "Error", "Failed to save file");
    }
}

void MainWindow::onSaveAs() {
    QString path = QFileDialog::getSaveFileName(this, "Save .lui file", QString(), "LUI Files (*.lui)");
    if (path.isEmpty()) return;
    if (m_doc.saveTo(path)) {
        updateTitle();
        statusBar()->showMessage("Saved: " + path, 3000);
    } else {
        QMessageBox::critical(this, "Error", "Failed to save file");
    }
}

void MainWindow::onClose() {
    if (m_doc.isModified() && !confirmDiscard()) return;
    m_doc.createNew();
    m_undoStack->clear();
    m_selectedId.clear();
    m_multiSelectedIds.clear();
    m_clipboard.reset();
    m_propPanel->setDocument(&m_doc);
    m_treePanel->setDocument(&m_doc);
    updateTitle();
    rebuildFromDocument();
    updateStatusBar();
}

void MainWindow::onExit() { close(); }
void MainWindow::onCut() { onCopy(); onDelete(); }

void MainWindow::onCopy() {
    std::string id = m_propPanel->currentId();
    if (id.empty()) id = m_selectedId;
    if (id.empty()) return;
    auto* node = m_doc.findById(id);
    if (!node) return;
    m_clipboard = *node;
}

void MainWindow::onPaste() {
    if (!m_clipboard.has_value()) return;
    WidgetNode copy = *m_clipboard;
    copy.id = m_doc.generateUniqueId(copy.type);
    copy.children.clear();
    copy.parentId.clear();
    auto p = copy.pos();
    copy.setPos(QPointF(p.x() + 20, p.y() + 20));

    std::string newId = copy.id;
    m_undoStack->push(new AddWidgetCommand(&m_doc, [this]() { scheduleRebuild(); }, std::move(copy)));
    m_selectedId = newId;
    m_propPanel->showWidget(m_doc.findById(newId));
    updateTitle();
    updateStatusBar();
}

void MainWindow::onSelectAll() {
    m_multiSelectedIds.clear();
    std::vector<std::string> ids;
    for (const auto& child : m_doc.children()) {
        ids.push_back(child.id);
    }
    if (ids.empty()) return;
    m_multiSelectedIds = ids;
    m_selectedId = ids.front();
    m_renderer.selectMultiple(ids);
    m_propPanel->showWidget(m_doc.findById(m_selectedId));
    updateStatusBar();
}

void MainWindow::onDelete() {
    std::string id = m_propPanel->currentId();
    if (id.empty()) id = m_selectedId;
    if (id.empty()) return;
    m_undoStack->push(new DeleteWidgetCommand(&m_doc, [this]() { scheduleRebuild(); }, id));
    m_selectedId.clear();
    m_multiSelectedIds.clear();
    m_propPanel->clearSelection();
    updateTitle();
    updateStatusBar();
}

void MainWindow::onCanvasSize() {
    bool ok = false;
    int w = QInputDialog::getInt(this, "Canvas Size", "Width:", m_doc.width(), 100, 4096, 10, &ok);
    if (!ok) return;
    int h = QInputDialog::getInt(this, "Canvas Size", "Height:", m_doc.height(), 100, 4096, 10, &ok);
    if (!ok) return;

    m_doc.setScreenSize(w, h);
    m_lvglWidget->setLvglSize(w, h);
    m_doc.markModified();
    updateTitle();
    rebuildFromDocument();
    updateStatusBar();
}

void MainWindow::onCompile() {
    onSave();
    if (m_doc.filePath().isEmpty()) {
        QMessageBox::warning(this, "Compile", "Please save the file first.");
        return;
    }

    QProcess proc;
    QString luicPath = QApplication::applicationDirPath() + "/luic";
    QFileInfo luicFi(luicPath);
    if (!luicFi.exists()) {
        luicPath = QApplication::applicationDirPath() + "/../bin/luic";
        luicFi.setFile(luicPath);
        if (!luicFi.exists()) {
            QMessageBox::critical(this, "Compile Error", "luic not found at:\n" + luicPath + "\n\nPlease build luic first.");
            return;
        }
    }

    QFileInfo fi(m_doc.filePath());
    QString outputDir = fi.absolutePath() + "/../ui_generated";
    QDir().mkpath(outputDir);
    QString outputPath = outputDir + "/" + fi.baseName() + "_ui.hpp";

    proc.start(luicPath, {m_doc.filePath(), "-o", outputPath, "--verbose"});
    statusBar()->showMessage("Compiling...");

    if (!proc.waitForFinished(10000)) {
        QMessageBox::critical(this, "Compile Error", "luic timed out after 10 seconds.");
        statusBar()->showMessage("Compile failed: timeout", 5000);
        return;
    }

    QString stdoutStr = proc.readAllStandardOutput();
    QString stderrStr = proc.readAllStandardError();
    if (proc.exitCode() == 0) {
        statusBar()->showMessage("Compile OK: " + outputPath, 5000);
        QMessageBox::information(this, "Compile Success", "Generated:\n" + outputPath + (stdoutStr.isEmpty() ? "" : "\n\n" + stdoutStr));
    } else {
        statusBar()->showMessage("Compile FAILED", 5000);
        QString msg = "luic exited with code " + QString::number(proc.exitCode());
        if (!stderrStr.isEmpty()) msg += "\n\n" + stderrStr;
        if (!stdoutStr.isEmpty()) msg += "\n\n" + stdoutStr;
        QMessageBox::critical(this, "Compile Error", msg);
    }
}

void MainWindow::onAbout() {
    QMessageBox::about(this, "About LVGL++ Designer",
        "<h3>LVGL++ Designer</h3>"
        "<p>Visual UI designer for LVGL++ projects.</p>"
        "<p>Version 1.0</p>");
}

void MainWindow::onBringToFront() {
    if (m_selectedId.empty()) return;
    int idx = m_doc.childIndex(m_selectedId);
    if (idx < 0) return;
    auto* siblings = m_doc.siblingsOf(m_selectedId);
    if (!siblings || idx >= (int)siblings->size() - 1) return;
    WidgetNode node = std::move((*siblings)[idx]);
    siblings->erase(siblings->begin() + idx);
    siblings->push_back(std::move(node));
    m_doc.markModified();
    updateTitle();
    rebuildFromDocument();
    m_renderer.selectObject(m_selectedId);
    updateStatusBar();
}

void MainWindow::onSendToBack() {
    if (m_selectedId.empty()) return;
    int idx = m_doc.childIndex(m_selectedId);
    if (idx <= 0) return;
    auto* siblings = m_doc.siblingsOf(m_selectedId);
    if (!siblings) return;
    WidgetNode node = std::move((*siblings)[idx]);
    siblings->erase(siblings->begin() + idx);
    siblings->insert(siblings->begin(), std::move(node));
    m_doc.markModified();
    updateTitle();
    rebuildFromDocument();
    m_renderer.selectObject(m_selectedId);
    updateStatusBar();
}

void MainWindow::onContextMenu(const QPoint& pos) {
    QMenu menu(this);
    menu.addAction("Delete", this, &MainWindow::onDelete);
    menu.addAction("Copy", this, &MainWindow::onCopy);
    if (m_clipboard.has_value()) menu.addAction("Paste", this, &MainWindow::onPaste);
    else {
        auto* pasteAct = menu.addAction("Paste");
        pasteAct->setEnabled(false);
    }
    menu.addSeparator();
    menu.addAction("Bring to Front", this, &MainWindow::onBringToFront);
    menu.addAction("Send to Back", this, &MainWindow::onSendToBack);
    menu.exec(m_lvglWidget->mapToGlobal(pos));
}

bool MainWindow::confirmDiscard() {
    if (!m_doc.isModified()) return true;
    auto result = QMessageBox::question(this, "Unsaved Changes",
        "The document has been modified.\nDo you want to save your changes?",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (result == QMessageBox::Save) {
        onSave();
        return !m_doc.isModified();
    }
    return result == QMessageBox::Discard;
}

void MainWindow::onApplyLayoutVBox()    { applyLayoutToSelection("LVBoxLayout"); }
void MainWindow::onApplyLayoutHBox()   { applyLayoutToSelection("LHBoxLayout"); }
void MainWindow::onApplyLayoutGrid()   { applyLayoutToSelection("LGridLayout"); }
void MainWindow::onApplyLayoutStacked() { applyLayoutToSelection("LStackedLayout"); }

void MainWindow::onBreakLayout() {
    std::string id = m_propPanel->currentId();
    if (id.empty()) id = m_selectedId;
    if (id.empty()) return;

    auto* node = m_doc.findById(id);
    if (!node) return;

    static const std::set<std::string> layoutTypes = {
        "LVBoxLayout", "LHBoxLayout", "LGridLayout", "LStackedLayout"
    };
    if (layoutTypes.find(node->type) == layoutTypes.end()) return;
    if (node->children.empty()) return;

    auto* parent = m_doc.findParentOf(id);
    std::string parentId = parent ? parent->id : "";

    auto* siblings = m_doc.siblingsOf(id);
    if (!siblings) return;
    int idx = m_doc.childIndex(id);
    if (idx < 0) return;

    std::vector<WidgetNode> extracted;
    for (auto& child : node->children) {
        child.parentId = parentId;
        extracted.push_back(std::move(child));
    }

    siblings->erase(siblings->begin() + idx);

    int insertIdx = idx;
    for (auto& n : extracted) {
        siblings->insert(siblings->begin() + insertIdx, std::move(n));
        insertIdx++;
    }

    m_multiSelectedIds.clear();
    m_selectedId.clear();
    m_doc.markModified();
    updateTitle();
    rebuildFromDocument();
    updateStatusBar();
}

void MainWindow::applyLayoutToSelection(const std::string& layoutType) {
    std::vector<std::string> targetIds;

    if (!m_multiSelectedIds.empty()) {
        targetIds = m_multiSelectedIds;
    } else if (!m_selectedId.empty()) {
        targetIds.push_back(m_selectedId);
    } else {
        return;
    }

    if (targetIds.empty()) return;

    std::string commonParentId;
    for (const auto& tid : targetIds) {
        auto* parent = m_doc.findParentOf(tid);
        std::string pid = parent ? parent->id : "";
        if (commonParentId.empty()) {
            commonParentId = pid;
        } else if (commonParentId != pid) {
            return;
        }
    }

    WidgetNode wrapper;
    wrapper.type = layoutType;
    wrapper.id = m_doc.generateUniqueId(layoutType);
    wrapper.parentId = commonParentId;
    wrapper.initDefaultProps();

    std::vector<WidgetNode> extracted;
    std::vector<int> indices;
    for (const auto& tid : targetIds) {
        auto* node = m_doc.findById(tid);
        if (!node) return;
        extracted.push_back(*node);
        int idx = m_doc.childIndex(tid);
        if (idx >= 0) indices.push_back(idx);
    }

    std::sort(indices.rbegin(), indices.rend());
    auto* siblings = m_doc.siblingsOf(targetIds.front());
    if (!siblings) return;
    for (int idx : indices) {
        if (idx < (int)siblings->size()) {
            siblings->erase(siblings->begin() + idx);
        }
    }

    for (auto& n : extracted) {
        n.parentId = wrapper.id;
        wrapper.children.push_back(std::move(n));
    }

    if (commonParentId.empty()) {
        m_doc.addChild(std::move(wrapper));
    } else {
        auto* parent = m_doc.findById(commonParentId);
        if (parent) {
            int insertIdx = indices.empty() ? (int)parent->children.size() : indices.back();
            m_doc.insertChildAt(commonParentId, insertIdx, std::move(wrapper));
        } else {
            m_doc.addChild(std::move(wrapper));
        }
    }

    m_multiSelectedIds.clear();
    m_selectedId = wrapper.id;
    m_doc.markModified();
    updateTitle();
    rebuildFromDocument();
    m_renderer.selectObject(m_selectedId);
    m_propPanel->showWidget(m_doc.findById(m_selectedId));
    updateStatusBar();
}

}
