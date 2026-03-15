#include "app/MainWindow.hpp"
#include <QApplication>
#include <QStatusBar>
#include <QKeySequence>

namespace designer {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setWindowTitle("LVGL++ Designer");
    resize(1400, 900);

    m_undoStack = new QUndoStack(this);

    m_scene = new DesignerScene(this);
    m_scene->setUndoStack(m_undoStack);

    m_view = new DesignerView(this);
    m_view->setScene(m_scene);
    setCentralWidget(m_view);

    m_compPanel = new ComponentPanel(this);
    m_propPanel = new PropertyPanel(this);
    m_propPanel->setScene(m_scene);
    m_treePanel = new TreePanel(this);

    setupDocks();
    setupMenus();
    setupToolbar();
    setupConnections();

    statusBar()->showMessage("Ready");
}

bool MainWindow::openFile(const QString& path) {
    if (!m_doc.load(path)) {
        QMessageBox::critical(this, "Error",
            "Failed to open: " + path);
        return false;
    }

    m_scene->setDocument(&m_doc);
    m_treePanel->setDocument(&m_doc);
    m_undoStack->clear();
    updateTitle();
    statusBar()->showMessage("Opened: " + path, 3000);
    return true;
}

void MainWindow::setupMenus() {
    auto* fileMenu = menuBar()->addMenu("&File");

    auto* openAct = fileMenu->addAction("&Open...", this, [this]() {
        QString path = QFileDialog::getOpenFileName(this,
            "Open .lui file", QString(), "LUI Files (*.lui);;All Files (*)");
        if (!path.isEmpty()) openFile(path);
    }, QKeySequence::Open);

    auto* saveAct = fileMenu->addAction("&Save", this,
        &MainWindow::onSave, QKeySequence::Save);

    auto* saveAsAct = fileMenu->addAction("Save &As...", this,
        &MainWindow::onSaveAs, QKeySequence("Ctrl+Shift+S"));

    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, &QWidget::close, QKeySequence::Quit);

    auto* editMenu = menuBar()->addMenu("&Edit");
    auto* undoAct = m_undoStack->createUndoAction(this, "&Undo");
    undoAct->setShortcut(QKeySequence::Undo);
    editMenu->addAction(undoAct);

    auto* redoAct = m_undoStack->createRedoAction(this, "&Redo");
    redoAct->setShortcut(QKeySequence::Redo);
    editMenu->addAction(redoAct);

    editMenu->addSeparator();
    editMenu->addAction("&Delete", this, &MainWindow::onDelete, QKeySequence::Delete);

    auto* buildMenu = menuBar()->addMenu("&Build");
    buildMenu->addAction("&Compile .lui", this,
        &MainWindow::onCompile, QKeySequence("F5"));
}

void MainWindow::setupToolbar() {
    auto* toolbar = addToolBar("Main");
    toolbar->setMovable(false);
    toolbar->addAction("Save", this, &MainWindow::onSave);
    toolbar->addAction("Compile", this, &MainWindow::onCompile);
    toolbar->addSeparator();
    toolbar->addAction("Delete", this, &MainWindow::onDelete);
    toolbar->addSeparator();
    toolbar->addAction(m_undoStack->createUndoAction(this, "Undo"));
    toolbar->addAction(m_undoStack->createRedoAction(this, "Redo"));
}

void MainWindow::setupDocks() {
    auto* compDock = new QDockWidget("Components", this);
    compDock->setWidget(m_compPanel);
    compDock->setFeatures(QDockWidget::DockWidgetMovable);
    addDockWidget(Qt::LeftDockWidgetArea, compDock);

    auto* treeDock = new QDockWidget("Widget Tree", this);
    treeDock->setWidget(m_treePanel);
    treeDock->setFeatures(QDockWidget::DockWidgetMovable);
    addDockWidget(Qt::LeftDockWidgetArea, treeDock);

    tabifyDockWidget(compDock, treeDock);
    compDock->raise();

    auto* propDock = new QDockWidget("Properties", this);
    propDock->setWidget(m_propPanel);
    propDock->setFeatures(QDockWidget::DockWidgetMovable);
    propDock->setMinimumWidth(300);
    addDockWidget(Qt::RightDockWidgetArea, propDock);
}

void MainWindow::setupConnections() {
    connect(m_scene, &DesignerScene::selectionChanged, this,
        [this](WidgetItem* item) {
            m_propPanel->showWidget(item);
        });

    connect(m_scene, &DesignerScene::documentModified, this,
        [this]() {
            m_doc.markModified();
            m_treePanel->rebuild();
            updateTitle();
        });

    connect(m_treePanel, &TreePanel::widgetSelected, this,
        [this](const std::string& id) {
            auto items = m_scene->items();
            for (auto* gi : items) {
                auto* wi = dynamic_cast<WidgetItem*>(gi);
                if (wi && wi->nodeId() == id) {
                    m_scene->selectItem(wi);
                    m_view->centerOn(wi);
                    break;
                }
            }
        });
}

void MainWindow::updateTitle() {
    QString title = "LVGL++ Designer";
    if (!m_doc.filePath().isEmpty()) {
        title += " - " + m_doc.filePath();
    }
    if (m_doc.isModified()) {
        title += " *";
    }
    setWindowTitle(title);
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
    QString path = QFileDialog::getSaveFileName(this,
        "Save .lui file", QString(), "LUI Files (*.lui)");
    if (path.isEmpty()) return;
    if (m_doc.saveTo(path)) {
        updateTitle();
        statusBar()->showMessage("Saved: " + path, 3000);
    } else {
        QMessageBox::critical(this, "Error", "Failed to save file");
    }
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
            QMessageBox::critical(this, "Compile Error",
                "luic not found at:\n" + luicPath +
                "\n\nPlease build luic first.");
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
        QMessageBox::critical(this, "Compile Error",
            "luic timed out after 10 seconds.");
        statusBar()->showMessage("Compile failed: timeout", 5000);
        return;
    }

    QString stdoutStr = proc.readAllStandardOutput();
    QString stderrStr = proc.readAllStandardError();

    if (proc.exitCode() == 0) {
        statusBar()->showMessage("Compile OK: " + outputPath, 5000);
        QMessageBox::information(this, "Compile Success",
            "Generated:\n" + outputPath +
            (stdoutStr.isEmpty() ? "" : "\n\n" + stdoutStr));
    } else {
        statusBar()->showMessage("Compile FAILED", 5000);
        QString msg = "luic exited with code " + QString::number(proc.exitCode());
        if (!stderrStr.isEmpty()) msg += "\n\n" + stderrStr;
        if (!stdoutStr.isEmpty()) msg += "\n\n" + stdoutStr;
        QMessageBox::critical(this, "Compile Error", msg);
    }
}

void MainWindow::onDelete() {
    m_scene->deleteSelected();
}

}
