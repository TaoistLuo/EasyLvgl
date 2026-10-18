#include "app/MainWindow.hpp"
#include "commands/UndoCommands.hpp"
#include <QApplication>
#include <QStatusBar>
#include <QKeySequence>
#include <QInputDialog>
#include <QClipboard>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QInputDialog>
#include <QRegularExpression>
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
    m_lvglWidget->setLvglSize(m_doc.width(), m_doc.height());

    m_statusInfo = new QLabel("就绪");
    statusBar()->addWidget(m_statusInfo, 1);
}

bool MainWindow::openFile(const QString& path) {
    if (!m_doc.load(path)) {
        QMessageBox::critical(this, "错误", "打开失败：" + path);
        return false;
    }

    m_undoStack->clear();
    m_selectedId.clear();
    m_multiSelectedIds.clear();
    m_clipboard.reset();
    m_propPanel->setDocument(&m_doc);
    m_treePanel->setDocument(&m_doc);
    m_lvglWidget->setLvglSize(m_doc.width(), m_doc.height());
    updateTitle();
    rebuildFromDocument();
    updateStatusBar();
    if (m_projectPanel) m_projectPanel->setCurrentFile(path);
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
        /* 重建后把面板显示值同步成 LVGL 真实生效值（新加组件/撤销重做后会变） */
        m_propPanel->refreshValues();
    });
}

void MainWindow::setupMenus() {
    auto* fileMenu = menuBar()->addMenu("文件(&F)");
    fileMenu->addAction("新建(&N)", this, &MainWindow::onNew, QKeySequence("Ctrl+N"));
    fileMenu->addAction("打开(&O)...", this, &MainWindow::onOpen, QKeySequence::Open);
    fileMenu->addAction("新建工程(&J)...", this, &MainWindow::onNewProject);
    fileMenu->addAction("打开工程(&P)...", this, &MainWindow::onOpenProject);
    fileMenu->addAction("新建组件(&M)...", this, &MainWindow::onNewComponent);
    fileMenu->addAction("保存(&S)", this, &MainWindow::onSave, QKeySequence::Save);
    fileMenu->addAction("另存为(&A)...", this, &MainWindow::onSaveAs, QKeySequence("Ctrl+Shift+S"));
    fileMenu->addSeparator();
    fileMenu->addAction("关闭(&C)", this, &MainWindow::onClose, QKeySequence("Ctrl+W"));
    fileMenu->addSeparator();
    fileMenu->addAction("退出(&X)", this, &MainWindow::onExit, QKeySequence::Quit);

    auto* editMenu = menuBar()->addMenu("编辑(&E)");
    auto* undoAct = m_undoStack->createUndoAction(this, "撤销(&U)");
    undoAct->setShortcut(QKeySequence::Undo);
    editMenu->addAction(undoAct);
    auto* redoAct = m_undoStack->createRedoAction(this, "重做(&R)");
    redoAct->setShortcut(QKeySequence::Redo);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction("剪切(&T)", this, &MainWindow::onCut, QKeySequence::Cut);
    editMenu->addAction("复制(&C)", this, &MainWindow::onCopy, QKeySequence::Copy);
    editMenu->addAction("粘贴(&P)", this, &MainWindow::onPaste, QKeySequence::Paste);
    editMenu->addSeparator();
    editMenu->addAction("删除(&D)", this, &MainWindow::onDelete, QKeySequence::Delete);
    editMenu->addSeparator();
    editMenu->addAction("全选(&A)", this, &MainWindow::onSelectAll, QKeySequence::SelectAll);
    editMenu->addSeparator();
    editMenu->addAction("添加图片资源(&I)...", this, &MainWindow::onAddImageResource);

    auto* viewMenu = menuBar()->addMenu("视图(&V)");
    m_compDock->toggleViewAction()->setText("组件(&C)");
    viewMenu->addAction(m_compDock->toggleViewAction());
    viewMenu->addAction(m_projectDock->toggleViewAction());
    m_treeDock->toggleViewAction()->setText("组件树(&T)");
    viewMenu->addAction(m_treeDock->toggleViewAction());
    m_propDock->toggleViewAction()->setText("属性(&P)");
    viewMenu->addAction(m_propDock->toggleViewAction());
    viewMenu->addSeparator();
    auto* statusToggle = viewMenu->addAction("状态栏(&S)");
    statusToggle->setCheckable(true);
    statusToggle->setChecked(true);
    connect(statusToggle, &QAction::toggled, statusBar(), &QWidget::setVisible);
    viewMenu->addSeparator();
    viewMenu->addAction("画布尺寸(&S)...", this, &MainWindow::onCanvasSize);

    auto* buildMenu = menuBar()->addMenu("构建(&B)");
    buildMenu->addAction("编译 .lui(&C)", this, &MainWindow::onCompile, QKeySequence("F5"));
    buildMenu->addAction("构建工程(&B)", this, &MainWindow::onBuildProject, QKeySequence("Ctrl+B"));
    buildMenu->addAction("设置(&S)...", this, &MainWindow::onSettings);

    auto* helpMenu = menuBar()->addMenu("帮助(&H)");
    helpMenu->addAction("关于(&A)", this, &MainWindow::onAbout);
    helpMenu->addAction("关于 Qt(&Q)", this, &QApplication::aboutQt);
}

void MainWindow::setupToolbar() {
    auto* toolbar = addToolBar("主工具栏");
    toolbar->setMovable(false);
    toolbar->addAction("新建", this, &MainWindow::onNew);
    toolbar->addAction("打开", this, &MainWindow::onOpen);
    toolbar->addAction("保存", this, &MainWindow::onSave);
    toolbar->addSeparator();
    toolbar->addAction("编译", this, &MainWindow::onCompile);
    toolbar->addSeparator();
    toolbar->addAction("剪切", this, &MainWindow::onCut);
    toolbar->addAction("复制", this, &MainWindow::onCopy);
    toolbar->addAction("粘贴", this, &MainWindow::onPaste);
    toolbar->addAction("删除", this, &MainWindow::onDelete);
    toolbar->addSeparator();
    toolbar->addAction(m_undoStack->createUndoAction(this, "撤销"));
    toolbar->addAction(m_undoStack->createRedoAction(this, "重做"));
    toolbar->addSeparator();
    toolbar->addAction("垂直布局", this, &MainWindow::onApplyLayoutVBox);
    toolbar->addAction("水平布局", this, &MainWindow::onApplyLayoutHBox);
    toolbar->addAction("网格布局", this, &MainWindow::onApplyLayoutGrid);
    toolbar->addAction("堆叠布局", this, &MainWindow::onApplyLayoutStacked);
    toolbar->addSeparator();
    toolbar->addAction("解除布局", this, &MainWindow::onBreakLayout);
}

void MainWindow::setupDocks() {
    m_compDock = new QDockWidget("组件", this);
    m_compDock->setWidget(m_compPanel);
    m_compDock->setFeatures(QDockWidget::DockWidgetMovable);
    addDockWidget(Qt::LeftDockWidgetArea, m_compDock);

    m_projectDock = new QDockWidget("工程", this);
    m_projectPanel = new ProjectPanel(this);
    m_projectDock->setWidget(m_projectPanel);
    m_projectDock->setFeatures(QDockWidget::DockWidgetMovable);
    addDockWidget(Qt::LeftDockWidgetArea, m_projectDock);
    tabifyDockWidget(m_compDock, m_projectDock);

    m_treeDock = new QDockWidget("组件树", this);
    m_treeDock->setWidget(m_treePanel);
    m_treeDock->setFeatures(QDockWidget::DockWidgetMovable);
    addDockWidget(Qt::LeftDockWidgetArea, m_treeDock);

    tabifyDockWidget(m_compDock, m_treeDock);
    m_compDock->raise();

    m_propDock = new QDockWidget("属性", this);
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
            m_lvglWidget->setRootSelected(id == LvglRenderer::kRootId);
            if (id == LvglRenderer::kRootId) {
                m_propPanel->showRoot(id, m_doc.width(), m_doc.height());
            } else {
                m_propPanel->showWidget(m_doc.findById(id));
            }
            updateStatusBar();
        });

        m_renderer.setOnMoved([this](const std::string& id, int x, int y, int w, int h) {
            if (id == LvglRenderer::kRootId) {
                if (w < 1 || h < 1) return;
                m_doc.setScreenSize(w, h);
                m_lvglWidget->setLvglSize(w, h);
                m_doc.markModified();
                m_propPanel->showRoot(id, w, h);
                updateTitle();
                updateStatusBar();
                return;
            }

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

    m_propPanel->setStyleValueProvider([this](const std::string& id, const std::string& prop) {
        return m_renderer.styleValue(id, prop);
    });
    connect(m_projectPanel, &ProjectPanel::openRequested, this, [this](const QString& p) {
        if (m_doc.isModified() && !confirmDiscard()) return;
        openFile(p);
    });
    connect(m_projectPanel, &ProjectPanel::newComponentRequested, this, &MainWindow::onNewComponent);
    connect(m_projectPanel, &ProjectPanel::newScreenRequested, this, &MainWindow::onNewScreen);
    connect(m_projectPanel, &ProjectPanel::addFilesRequested, this, &MainWindow::onAddProjectFiles);
    connect(m_projectPanel, &ProjectPanel::removeRequested, this, &MainWindow::removeFromProject);

    m_propPanel->setComponentResolver([this](const std::string& name) -> const LuiDocument* {
        auto it = m_componentDocs.find(name);
        return it == m_componentDocs.end() ? nullptr : it->second;
    });

    m_propPanel->setRootResizeCallback([this](int w, int h) {
        if (w < 1 || h < 1) return;
        m_doc.setScreenSize(w, h);
        m_lvglWidget->setLvglSize(w, h);
        m_doc.markModified();
        updateTitle();
        rebuildFromDocument();
        updateStatusBar();
    });

    connect(m_lvglWidget, &SDL2Widget::rootResized, this, [this](int w, int h) {
        if (w < 1 || h < 1) return;
        m_doc.setScreenSize(w, h);
        m_lvglWidget->setLvglSize(w, h);
        m_doc.markModified();
        m_propPanel->showRoot(LvglRenderer::kRootId, w, h);
        updateTitle();
        rebuildFromDocument();
        updateStatusBar();
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

/* 把图片复制进工程 assets/ 并登记到 .lui 的 resources，控件里即可用 "#别名" 引用。
 * 编译期由 scripts/assets/convert_assets.py 转成 lv_image_dsc_t，等价 Qt 的 qrc。 */
void MainWindow::onAddImageResource() {
    QString src = QFileDialog::getOpenFileName(this, "添加图片资源", m_doc.baseDir(),
                                               "图片 (*.png *.jpg *.jpeg *.bmp);;所有文件 (*)");
    if (src.isEmpty()) return;

    QFileInfo fi(src);
    QString assetsDir = QDir(m_doc.baseDir()).absoluteFilePath("assets");
    if (!QDir().mkpath(assetsDir)) {
        QMessageBox::warning(this, "添加图片资源", "无法创建目录：" + assetsDir);
        return;
    }
    QString dst = QDir(assetsDir).absoluteFilePath(fi.fileName());
    if (QFileInfo(dst).absoluteFilePath() != fi.absoluteFilePath()) {
        if (QFileInfo::exists(dst) &&
            QMessageBox::question(this, "添加图片资源",
                                  "assets/" + fi.fileName() + " 已存在，覆盖？") != QMessageBox::Yes)
            return;
        QFile::remove(dst);
        if (!QFile::copy(src, dst)) {
            QMessageBox::warning(this, "添加图片资源", "复制失败：" + dst);
            return;
        }
    }

    /* 资源名必须是合法的 C 标识符（它同时是生成代码里的变量名） */
    std::string alias = fi.baseName().toStdString();
    for (auto& c : alias)
        if (!isalnum((unsigned char)c) && c != '_') c = '_';
    if (alias.empty() || isdigit((unsigned char)alias[0])) alias = "img_" + alias;

    m_doc.setResource(alias, ("assets/" + fi.fileName()).toStdString());
    m_doc.markModified();
    updateTitle();
    statusBar()->showMessage(QString("已添加图片资源 #%1（生成代码前请运行 "
                                     "scripts/assets/convert_assets.py）")
                             .arg(QString::fromStdString(alias)), 8000);
}

void MainWindow::updateTitle() {
    QString title = "LVGL++ Designer";
    if (!m_doc.filePath().isEmpty()) title += " - " + m_doc.filePath();
    if (m_doc.isModified()) title += " *";
    setWindowTitle(title);
}

void MainWindow::updateStatusBar() {
    if (m_selectedId.empty()) {
        m_statusInfo->setText(QString("就绪 | 画布：%1x%2").arg(m_doc.width()).arg(m_doc.height()));
        return;
    }
    if (m_selectedId == LvglRenderer::kRootId) {
        m_statusInfo->setText(QString("已选中：根 (LScreen) | 画布：%1x%2")
            .arg(m_doc.width()).arg(m_doc.height()));
        return;
    }
    auto* node = m_doc.findById(m_selectedId);
    if (!node) {
        m_statusInfo->setText(QString("就绪 | 画布：%1x%2").arg(m_doc.width()).arg(m_doc.height()));
        return;
    }
    auto p = node->pos();
    auto s = node->size();
    m_statusInfo->setText(QString("已选中：%1 (%2) | 位置：%3,%4 | 尺寸：%5x%6 | 画布：%7x%8")
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
    m_lvglWidget->setLvglSize(m_doc.width(), m_doc.height());
    updateTitle();
    rebuildFromDocument();
    updateStatusBar();
}

void MainWindow::onOpen() {
    if (m_doc.isModified() && !confirmDiscard()) return;
    QString path = QFileDialog::getOpenFileName(this, "Open .lui file", QString(), "LUI Files (*.lui);;All Files (*)");
    if (!path.isEmpty()) openFile(path);
}

/* 打开工程：读取清单 -> 加载全部 .lui -> 填组件面板 -> 打开入口文件 */
void MainWindow::loadProject(const QString& manifest) {
    QFile f(manifest);
    if (!f.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "打开工程", "无法读取：" + manifest);
        return;
    }
    QJsonParseError err;
    auto jdoc = QJsonDocument::fromJson(f.readAll(), &err);
    if (err.error != QJsonParseError::NoError) {
        QMessageBox::critical(this, "打开工程", "清单 JSON 错误：" + err.errorString());
        return;
    }
    auto root = jdoc.object();
    const QString base = QFileInfo(manifest).absolutePath();
    const QString entryName = root["entry"].toString();

    m_projectFile = manifest;
    m_projectDocs.clear();
    m_componentDocs.clear();
    m_projectComponents.clear();

    QString entryPath;
    for (auto v : root["files"].toArray()) {
        QString rel = v.toString();
        QString path = QDir(base).absoluteFilePath(rel);
        auto doc = std::make_unique<LuiDocument>();
        if (!doc->load(path)) {
            QMessageBox::warning(this, "打开工程", "加载失败：" + path);
            continue;
        }
        /* 只有 kind: component 的文件能作为实例插入（屏幕不是部件） */
        if (doc->isComponent()) m_projectComponents.push_back({doc->name(), path});
        m_componentDocs[doc->name()] = doc.get();
        if (!entryName.isEmpty() && doc->name() == entryName.toStdString())
            entryPath = path;
        m_projectDocs.push_back(std::move(doc));
    }

    {
        std::vector<std::string> names;
        for (auto& c : m_projectComponents) names.push_back(c.first);
        m_compPanel->setProjectComponents(names);
    }
    m_renderer.setComponentResolver([this](const std::string& name) -> const LuiDocument* {
        auto it = m_componentDocs.find(name);
        return it == m_componentDocs.end() ? nullptr : it->second;
    });

    statusBar()->showMessage(QString("工程已打开：%1（%2 个文件）")
                                 .arg(QFileInfo(manifest).fileName())
                                 .arg(m_projectComponents.size()), 6000);

    if (entryPath.isEmpty() && !m_projectComponents.empty())
        entryPath = m_projectComponents.front().second;
    if (!entryPath.isEmpty()) openFile(entryPath);
    refreshProjectPanel();
}

/* ---------------- 新建工程 / 新建组件 ---------------- */
namespace {

/* 写一个 .lui（带一个可编译的最小内容） */
bool write_lui(const QString& path, const QString& name, const QString& kind,
               const QString& defaultText) {
    QJsonObject props{
        {"pos", QJsonArray{20, 20}},
        {"size", QJsonArray{300, 30}},
        {"text", defaultText},
        {"color", "0x000000"},
    };
    QJsonObject label{
        {"type", "LLabel"}, {"id", "title"}, {"props", props},
    };
    QJsonObject root{
        {"version", "1.0"},
        {"name", name},
        {"kind", kind},
        {"children", QJsonArray{label}},
    };
    if (kind == "screen") root["size"] = QJsonArray{800, 480};
    QDir().mkpath(QFileInfo(path).absolutePath());
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly)) return false;
    f.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return true;
}

} // namespace

void MainWindow::onNewProject() {
    if (m_doc.isModified() && !confirmDiscard()) return;

    bool ok = false;
    QString name = QInputDialog::getText(this, "新建工程", "工程名称：",
                                         QLineEdit::Normal, "MyProject", &ok).trimmed();
    if (!ok || name.isEmpty()) return;

    QString parent = QFileDialog::getExistingDirectory(this, "选择工程存放位置");
    if (parent.isEmpty()) return;

    QString dir = QDir(parent).absoluteFilePath(name);
    if (QDir(dir).exists() && !QDir(dir).entryList(QDir::NoDotAndDotDot | QDir::AllEntries).isEmpty()) {
        QMessageBox::warning(this, "新建工程", "目录已存在且不为空：\n" + dir);
        return;
    }
    QDir().mkpath(dir + "/ui/screens");
    QDir().mkpath(dir + "/ui/components");

    const QString screenName = "MainScreen";
    const QString screenPath = dir + "/ui/screens/" + screenName + ".lui";
    if (!write_lui(screenPath, screenName, "screen", "Hello LVGL++")) {
        QMessageBox::critical(this, "新建工程", "无法写入：" + screenPath);
        return;
    }

    QJsonObject manifest{
        {"version", "1.0"},
        {"name", name},
        {"entry", screenName},
        {"files", QJsonArray{"ui/screens/" + screenName + ".lui"}},
    };
    const QString manifestPath = dir + "/lvglpp.project.json";
    QFile f(manifestPath);
    if (!f.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "新建工程", "无法写入：" + manifestPath);
        return;
    }
    f.write(QJsonDocument(manifest).toJson(QJsonDocument::Indented));
    f.close();

    loadProject(manifestPath);
    statusBar()->showMessage("已新建工程：" + dir, 6000);
}

/* 在已打开的工程里新建一个组件（kind: component），并加进清单 */
void MainWindow::onNewComponent() {
    if (m_projectFile.isEmpty()) {
        QMessageBox::information(this, "新建组件", "请先新建或打开一个工程。");
        return;
    }
    bool ok = false;
    QString name = QInputDialog::getText(this, "新建组件", "组件名称（C++ 标识符）：",
                                         QLineEdit::Normal, "MyWidget", &ok).trimmed();
    if (!ok || name.isEmpty()) return;
    static const QRegularExpression ident("^[A-Za-z_][A-Za-z0-9_]*$");
    if (!ident.match(name).hasMatch()) {
        QMessageBox::warning(this, "新建组件", "名称必须是合法的 C++ 标识符。");
        return;
    }
    if (m_componentDocs.count(name.toStdString())) {
        QMessageBox::warning(this, "新建组件", "工程里已有同名组件或屏幕。");
        return;
    }

    const QString base = QFileInfo(m_projectFile).absolutePath();
    const QString rel = "ui/components/" + name + ".lui";
    const QString path = QDir(base).absoluteFilePath(rel);
    if (QFileInfo::exists(path)) {
        QMessageBox::warning(this, "新建组件", "文件已存在：" + path);
        return;
    }
    if (!write_lui(path, name, "component", name)) {
        QMessageBox::critical(this, "新建组件", "无法写入：" + path);
        return;
    }

    /* 加进清单 files */
    QFile mf(m_projectFile);
    QJsonObject root;
    if (mf.open(QIODevice::ReadOnly)) {
        root = QJsonDocument::fromJson(mf.readAll()).object();
        mf.close();
    }
    QJsonArray files = root["files"].toArray();
    files.append(rel);
    root["files"] = files;
    if (mf.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        mf.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
        mf.close();
    }

    const QString manifest = m_projectFile;
    loadProject(manifest);
    openFile(path);
}

void MainWindow::refreshProjectPanel() {
    if (m_projectFile.isEmpty()) {
        m_projectPanel->showEmpty();
        return;
    }
    QString name = QFileInfo(m_projectFile).completeBaseName();
    QFile f(m_projectFile);
    if (f.open(QIODevice::ReadOnly)) {
        auto j = QJsonDocument::fromJson(f.readAll()).object();
        if (!j["name"].toString().isEmpty()) name = j["name"].toString();
    }
    std::vector<ProjectFile> files;
    for (auto& kv : m_componentDocs) {
        if (!kv.second) continue;
        ProjectFile pf;
        pf.name = kv.first;
        pf.path = kv.second->filePath();
        pf.isComponent = kv.second->isComponent();
        files.push_back(pf);
    }
    m_projectPanel->setProject(name, m_projectFile, files);
    if (!m_doc.filePath().isEmpty()) m_projectPanel->setCurrentFile(m_doc.filePath());
}

bool MainWindow::updateManifestFiles(const std::function<void(QJsonArray&)>& fn) {
    QFile f(m_projectFile);
    if (!f.open(QIODevice::ReadOnly)) return false;
    auto root = QJsonDocument::fromJson(f.readAll()).object();
    f.close();
    QJsonArray files = root["files"].toArray();
    fn(files);
    root["files"] = files;
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;
    f.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return true;
}

/* 新建屏幕：与新建组件同流程，放到 ui/screens */
void MainWindow::onNewScreen() {
    if (m_projectFile.isEmpty()) {
        QMessageBox::information(this, "新建屏幕", "请先新建或打开一个工程。");
        return;
    }
    bool ok = false;
    QString name = QInputDialog::getText(this, "新建屏幕", "屏幕名称（C++ 标识符）：",
                                         QLineEdit::Normal, "MyScreen", &ok).trimmed();
    if (!ok || name.isEmpty()) return;
    static const QRegularExpression ident("^[A-Za-z_][A-Za-z0-9_]*$");
    if (!ident.match(name).hasMatch()) {
        QMessageBox::warning(this, "新建屏幕", "名称必须是合法的 C++ 标识符。");
        return;
    }
    const QString base = QFileInfo(m_projectFile).absolutePath();
    const QString rel = "ui/screens/" + name + ".lui";
    const QString path = QDir(base).absoluteFilePath(rel);
    if (QFileInfo::exists(path) || m_componentDocs.count(name.toStdString())) {
        QMessageBox::warning(this, "新建屏幕", "同名文件已存在。");
        return;
    }
    if (!write_lui(path, name, "screen", name)) {
        QMessageBox::critical(this, "新建屏幕", "无法写入：" + path);
        return;
    }
    updateManifestFiles([&](QJsonArray& files) { files.append(rel); });
    loadProject(m_projectFile);
    openFile(path);
}

/* 把外部复制进来的 .lui 收进工程（必要时先复制到 ui/components 或 ui/screens） */
void MainWindow::onAddProjectFiles() {
    if (m_projectFile.isEmpty()) {
        QMessageBox::information(this, "添加已有文件", "请先新建或打开一个工程。");
        return;
    }
    QStringList paths = QFileDialog::getOpenFileNames(this, "添加已有 .lui 到工程",
        QFileInfo(m_projectFile).absolutePath(), "LUI (*.lui)");
    if (paths.isEmpty()) return;

    const QString base = QFileInfo(m_projectFile).absolutePath();
    QJsonArray rels;
    for (auto& p : paths) {
        LuiDocument probe;
        if (!probe.load(p)) {
            QMessageBox::warning(this, "添加已有文件", "无法解析：" + p);
            continue;
        }
        QString target = p;
        if (!p.startsWith(base + "/")) {
            QString sub = probe.isComponent() ? "/ui/components/" : "/ui/screens/";
            QDir().mkpath(base + sub);
            target = base + sub + QFileInfo(p).fileName();
            QFile::remove(target);
            if (!QFile::copy(p, target)) {
                QMessageBox::warning(this, "添加已有文件", "复制失败：" + p);
                continue;
            }
        }
        rels.append(QDir(base).relativeFilePath(target));
    }
    if (rels.isEmpty()) return;
    updateManifestFiles([&](QJsonArray& files) {
        for (auto v : rels) files.append(v);
    });
    loadProject(m_projectFile);
}

void MainWindow::removeFromProject(const QString& path) {
    if (m_projectFile.isEmpty()) return;
    if (QMessageBox::question(this, "从工程中删除",
            "从工程移除并删除文件？\n\n" + path) != QMessageBox::Yes)
        return;
    const QString base = QFileInfo(m_projectFile).absolutePath();
    const QString rel = QDir(base).relativeFilePath(path);
    updateManifestFiles([&](QJsonArray& files) {
        QJsonArray out;
        for (auto v : files) if (v.toString() != rel) out.append(v);
        files = out;
    });
    QFile::remove(path);
    loadProject(m_projectFile);
}

void MainWindow::onOpenProject() {
    if (m_doc.isModified() && !confirmDiscard()) return;
    QString path = QFileDialog::getOpenFileName(this, "打开工程", QString(),
                                                "LVGL++ 工程 (lvglpp.project.json);;JSON (*.json)");
    if (!path.isEmpty()) loadProject(path);
}

void MainWindow::onSave() {
    if (m_doc.filePath().isEmpty()) {
        onSaveAs();
        return;
    }
    if (m_doc.save()) {
        updateTitle();
        statusBar()->showMessage("已保存", 3000);
    } else {
        QMessageBox::critical(this, "错误", "保存文件失败");
    }
}

void MainWindow::onSaveAs() {
    QString path = QFileDialog::getSaveFileName(this, "Save .lui file", QString(), "LUI Files (*.lui)");
    if (path.isEmpty()) return;
    if (m_doc.saveTo(path)) {
        updateTitle();
        statusBar()->showMessage("已保存：" + path, 3000);
    } else {
        QMessageBox::critical(this, "错误", "保存文件失败");
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
    m_lvglWidget->setLvglSize(m_doc.width(), m_doc.height());
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
    int w = QInputDialog::getInt(this, "画布尺寸", "宽度：", m_doc.width(), 100, 4096, 10, &ok);
    if (!ok) return;
    int h = QInputDialog::getInt(this, "画布尺寸", "高度：", m_doc.height(), 100, 4096, 10, &ok);
    if (!ok) return;

    m_doc.setScreenSize(w, h);
    m_lvglWidget->setLvglSize(w, h);
    m_doc.markModified();
    updateTitle();
    rebuildFromDocument();
    updateStatusBar();
}

void MainWindow::onSettings() {
    SettingsDialog dlg(this);
    dlg.exec();
}

/* 构建工程：先 cmake 配置（构建目录为空时），再 cmake --build */
void MainWindow::onBuildProject() {
    const QString buildDir = SettingsDialog::buildDir();
    if (buildDir.isEmpty()) {
        QMessageBox::information(this, "构建工程", "请先在「设置」里配置 CMake 与构建目录。");
        onSettings();
        return;
    }
    const QString cmake = SettingsDialog::cmakePath();
    QDir().mkpath(buildDir);
    if (!QFileInfo::exists(buildDir + "/CMakeCache.txt")) {
        QStringList cfg{"-S", QDir::currentPath(), "-B", buildDir,
                        "-G", SettingsDialog::generator()};
        for (auto& a : SettingsDialog::extraArgs().split(' ', QString::SkipEmptyParts)) cfg << a;
        QProcess c;
        c.start(cmake, cfg);
        if (!c.waitForFinished(60000) || c.exitCode() != 0) {
            QMessageBox::critical(this, "构建工程",
                "CMake 配置失败：\n" + QString::fromLocal8Bit(c.readAllStandardError()));
            return;
        }
    }
    QProcess b;
    b.start(cmake, {"--build", buildDir, "-j"});
    statusBar()->showMessage("正在构建…");
    if (!b.waitForFinished(600000)) {
        QMessageBox::critical(this, "构建工程", "构建超时。");
        return;
    }
    const QString out = QString::fromLocal8Bit(b.readAllStandardError());
    if (b.exitCode() == 0)
        statusBar()->showMessage("构建成功", 5000);
    else
        QMessageBox::critical(this, "构建错误", out.right(4000));
}

void MainWindow::onCompile() {
    onSave();
    if (m_doc.filePath().isEmpty()) {
        QMessageBox::warning(this, "编译", "请先保存文件。");
        return;
    }

    QProcess proc;
    QString luicPath = SettingsDialog::luicPath();
    QFileInfo luicFi(luicPath);
    if (!luicFi.exists()) {
        luicPath = QApplication::applicationDirPath() + "/../bin/luic";
        luicFi.setFile(luicPath);
        if (!luicFi.exists()) {
            QMessageBox::critical(this, "编译错误", "未找到 luic：\n" + luicPath + "\n\n请先构建 luic。");
            return;
        }
    }

    QFileInfo fi(m_doc.filePath());
    QString outputDir = fi.absolutePath() + "/../ui_generated";
    QDir().mkpath(outputDir);
    QString outputPath = outputDir + "/" + fi.baseName() + "_ui.hpp";

    QStringList args;
    if (!m_projectFile.isEmpty()) {
        /* 工程模式：一次生成全部文件（含 "@组件" 实例展开） */
        args << "--project" << m_projectFile << "--verbose";
    } else {
        args << m_doc.filePath() << "-o" << outputPath << "--verbose";
    }
    proc.start(luicPath, args);
    statusBar()->showMessage("正在编译...");

    if (!proc.waitForFinished(10000)) {
        QMessageBox::critical(this, "编译错误", "luic 执行超时（10 秒）。");
        statusBar()->showMessage("编译失败：超时", 5000);
        return;
    }

    QString stdoutStr = proc.readAllStandardOutput();
    QString stderrStr = proc.readAllStandardError();
    if (proc.exitCode() == 0) {
        const QString target = m_projectFile.isEmpty() ? outputPath : m_projectFile;
        statusBar()->showMessage("编译成功", 5000);
        QMessageBox::information(this, "编译成功",
                                 (m_projectFile.isEmpty() ? "已生成：\n" + outputPath
                                                          : "工程已生成：" + target) +
                                 (stdoutStr.isEmpty() ? "" : "\n\n" + stdoutStr));
    } else {
        statusBar()->showMessage("编译失败", 5000);
        QString msg = "luic exited with code " + QString::number(proc.exitCode());
        if (!stderrStr.isEmpty()) msg += "\n\n" + stderrStr;
        if (!stdoutStr.isEmpty()) msg += "\n\n" + stdoutStr;
        QMessageBox::critical(this, "编译错误", msg);
    }
}

void MainWindow::onAbout() {
    QMessageBox::about(this, "关于 LVGL++ Designer",
        "<h3>LVGL++ Designer</h3>"
        "<p>LVGL++ 项目的可视化 UI 设计器。</p>"
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
    menu.addAction("删除", this, &MainWindow::onDelete);
    menu.addAction("复制", this, &MainWindow::onCopy);
    if (m_clipboard.has_value()) menu.addAction("粘贴", this, &MainWindow::onPaste);
    else {
        auto* pasteAct = menu.addAction("粘贴");
        pasteAct->setEnabled(false);
    }
    menu.addSeparator();
    menu.addAction("置于顶层", this, &MainWindow::onBringToFront);
    menu.addAction("置于底层", this, &MainWindow::onSendToBack);
    menu.exec(m_lvglWidget->mapToGlobal(pos));
}

bool MainWindow::confirmDiscard() {
    if (!m_doc.isModified()) return true;
    auto result = QMessageBox::question(this, "未保存的更改",
        "文档已修改。\n是否保存更改？",
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
