#include "app/MainWindow.hpp"
#include "commands/UndoCommands.hpp"
#include <QApplication>
#include <QStatusBar>
#include <QKeySequence>
#include <QInputDialog>
#include <QClipboard>
#include <QTimer>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>
#include <QPixmap>
#include <QPolygonF>
#include <QTabWidget>
#include <QSettings>
#include <QJsonArray>
#include <QJsonObject>
#include <QPainter>
#include <QPixmap>
#include <QPolygonF>
#include <QTabWidget>
#include <QSettings>
#include <QInputDialog>
#include <QRegularExpression>
#include <algorithm>
#include <set>

namespace designer {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setWindowTitle("LVGL++ Designer");
    /* 停靠面板的标签放在上方（更接近 Qt Creator） */
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::North);
    setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::North);
    setDockNestingEnabled(true);
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

    /* 恢复上次打开的工程（命令行带参数时会被覆盖） */
    {
        const QString last = QSettings().value("project/last").toString();
        if (!last.isEmpty() && QFileInfo::exists(last))
            QTimer::singleShot(0, this, [this, last] { loadProject(last); });
    }

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

    m_doc.setProjectResources(m_projectResources, QFileInfo(m_projectFile).absolutePath());

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
        /* 重建会清掉选中状态：多选时整体恢复，单选时恢复单个 */
        if (m_multiSelectedIds.size() > 1) {
            m_renderer.selectMultiple(m_multiSelectedIds);
        } else if (!m_selectedId.empty()) {
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
    m_leftDock->toggleViewAction()->setText("组件 / 工程(&C)");
    viewMenu->addAction(m_leftDock->toggleViewAction());
    viewMenu->addAction(m_buildDock->toggleViewAction());
    viewMenu->addAction(m_runDock->toggleViewAction());
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
    buildMenu->addAction("运行(&R)", this, &MainWindow::onRunProject, QKeySequence("Ctrl+R"));
    buildMenu->addAction("全构建并运行(&A)", this, &MainWindow::onBuildAndRun, QKeySequence("Ctrl+F5"));
    buildMenu->addAction("设置(&S)...", this, &MainWindow::onSettings);

    auto* helpMenu = menuBar()->addMenu("帮助(&H)");
    helpMenu->addAction("关于(&A)", this, &MainWindow::onAbout);
}


/* 自绘图标：主题图标在深色工具栏上多为黑色，且部分名称缺失。
 * 这里用 QPainter 画一组浅色线框图标，保证任何系统上都清晰可见。 */
namespace {

QIcon makeIcon(const QString& kind) {
    const QColor fg(0xd8, 0xd8, 0xd8);
    const QColor accent(0x4c, 0xc0, 0x6a);   // 运行类用绿色，和 Qt Creator 一致
    QPixmap pm(32, 32);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);

    if (kind == "compile") {                       // 环形箭头（重新生成）
        p.setPen(QPen(fg, 2.4, Qt::SolidLine, Qt::RoundCap));
        p.drawArc(QRectF(7, 7, 18, 18), 40 * 16, 280 * 16);
        QPolygonF head; head << QPointF(22,4) << QPointF(27,10) << QPointF(19,11);
        p.setBrush(fg); p.setPen(Qt::NoPen); p.drawPolygon(head);
    } else if (kind == "build") {                  // 锤子
        p.setPen(QPen(fg, 3, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(11, 24), QPointF(21, 12));
        p.setBrush(fg); p.setPen(Qt::NoPen);
        p.save(); p.translate(21, 9); p.rotate(45);
        p.drawRoundedRect(QRectF(-7, -5, 14, 10), 2, 2); p.restore();
    } else if (kind == "run") {                    // 播放三角（绿）
        p.setBrush(accent); p.setPen(Qt::NoPen);
        QPolygonF tri; tri << QPointF(11,7) << QPointF(26,16) << QPointF(11,25); p.drawPolygon(tri);
    } else if (kind == "buildrun") {               // 三角 + 构建条
        p.setPen(QPen(fg, 2.4, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(6, 9), QPointF(6, 23));
        p.setBrush(accent); p.setPen(Qt::NoPen);
        QPolygonF tri2; tri2 << QPointF(11,9) << QPointF(25,16) << QPointF(11,23); p.drawPolygon(tri2);
    } else if (kind == "vbox") {                   // 三条横线
        p.setPen(QPen(fg, 3, Qt::SolidLine, Qt::RoundCap));
        for (int i = 0; i < 3; i++) p.drawLine(QPointF(8, 10 + i * 6), QPointF(24, 10 + i * 6));
    } else if (kind == "hbox") {                   // 三条竖线
        p.setPen(QPen(fg, 3, Qt::SolidLine, Qt::RoundCap));
        for (int i = 0; i < 3; i++) p.drawLine(QPointF(10 + i * 6, 8), QPointF(10 + i * 6, 24));
    } else if (kind == "grid") {                   // 2x2 方格
        p.setPen(QPen(fg, 2));
        p.setBrush(Qt::NoBrush);
        p.drawRect(QRectF(7, 7, 8, 8));   p.drawRect(QRectF(17, 7, 8, 8));
        p.drawRect(QRectF(7, 17, 8, 8));  p.drawRect(QRectF(17, 17, 8, 8));
    } else if (kind == "stacked") {                // 叠放
        p.setPen(QPen(fg, 2));
        p.setBrush(Qt::NoBrush);
        p.drawRect(QRectF(9, 5, 14, 14));
        p.drawRect(QRectF(5, 11, 14, 14));
    } else if (kind == "new") {                    // 空白页 + 加号
        p.setPen(QPen(fg, 2));
        p.setBrush(Qt::NoBrush);
        p.drawRoundedRect(QRectF(6, 5, 14, 20), 2, 2);
        p.drawLine(QPointF(20, 18), QPointF(27, 18));
        p.drawLine(QPointF(23.5, 14.5), QPointF(23.5, 21.5));
    } else if (kind == "open") {                   // 文件夹
        p.setPen(QPen(fg, 2));
        p.setBrush(Qt::NoBrush);
        QPolygonF folder;
        folder << QPointF(6,10) << QPointF(14,10) << QPointF(16,13)
               << QPointF(26,13) << QPointF(26,23) << QPointF(6,23);
        p.drawPolygon(folder);
    } else if (kind == "save") {                   // 软盘
        p.setPen(QPen(fg, 2));
        p.setBrush(Qt::NoBrush);
        p.drawRoundedRect(QRectF(6, 6, 20, 20), 2, 2);
        p.drawRect(QRectF(10, 6, 12, 7));
        p.drawRect(QRectF(10, 17, 12, 9));
    } else if (kind == "cut") {                    // 剪刀（加粗）
        p.setPen(QPen(fg, 3, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(11, 7), QPointF(21, 22));
        p.drawLine(QPointF(21, 7), QPointF(11, 22));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(QPointF(8, 24), 4, 4);
        p.drawEllipse(QPointF(24, 24), 4, 4);
    } else if (kind == "copy") {                   // 两张纸（实心+描边）
        p.setPen(QPen(fg, 2));
        p.setBrush(Qt::NoBrush);
        p.drawRoundedRect(QRectF(5, 10, 14, 17), 2, 2);
        p.setBrush(fg);
        p.drawRoundedRect(QRectF(13, 5, 14, 17), 2, 2);
    } else if (kind == "paste") {                  // 剪贴板（粗框 + 实心夹子）
        p.setPen(QPen(fg, 2.5));
        p.setBrush(Qt::NoBrush);
        p.drawRoundedRect(QRectF(6, 6, 20, 22), 2, 2);
        p.setBrush(fg);
        p.drawRoundedRect(QRectF(11, 3, 10, 6), 1, 1);
    } else if (kind == "delete") {                 // 垃圾桶（加粗）
        p.setPen(QPen(fg, 2.6, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(7, 10), QPointF(25, 10));
        p.drawLine(QPointF(13, 6), QPointF(19, 6));
        p.setBrush(Qt::NoBrush);
        p.drawRoundedRect(QRectF(9, 12, 14, 15), 2, 2);
        p.drawLine(QPointF(14, 16), QPointF(14, 24));
        p.drawLine(QPointF(18, 16), QPointF(18, 24));
    } else if (kind == "undo" || kind == "redo") { // 弧形箭头
        p.setPen(QPen(fg, 2.4, Qt::SolidLine, Qt::RoundCap));
        p.drawArc(QRectF(7, 9, 18, 16), 30 * 16, 240 * 16);
        QPolygonF head;
        if (kind == "undo") head << QPointF(8,12) << QPointF(14,10) << QPointF(13,17);
        else                head << QPointF(24,12) << QPointF(18,10) << QPointF(19,17);
        p.setBrush(fg); p.setPen(Qt::NoPen); p.drawPolygon(head);
    } else if (kind == "break") {                  // 断开布局
        p.setPen(QPen(fg, 2.6, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(8, 8), QPointF(24, 24));
        p.drawLine(QPointF(24, 8), QPointF(8, 24));
    }
    p.end();
    return QIcon(pm);
}

} // namespace

void MainWindow::setupToolbar() {
    /* 剪切/复制/粘贴/删除：使用 Lucide 图标（ISC 许可），随 qrc 打包；
     * 资源缺失时回退到自绘图标 */
    auto svgIcon = [this](const char* res, const char* fb) {
        QIcon ic(QString(":/icons/%1").arg(QString::fromLatin1(res)));
        /* Qt5.7 的 SVG 插件在部分环境下加载失败：图标对象非空但没有像素，
         * 所以用"能否取到 16x16 像素"来判断可用性 */
        if (ic.pixmap(16, 16).isNull()) ic = makeIcon(QString::fromLatin1(fb));
        return ic;
    };

    auto* toolbar = addToolBar("主工具栏");
    toolbar->setMovable(false);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);   // 图标 + 文字
    toolbar->setIconSize(QSize(16, 16));
    toolbar->addAction(makeIcon("new"), "新建", this, &MainWindow::onNew);
    toolbar->addAction(makeIcon("open"), "打开", this, &MainWindow::onOpen);
    toolbar->addAction(makeIcon("save"), "保存", this, &MainWindow::onSave);
    toolbar->addSeparator();
    /* 构建相关：图标 + 文字，悬停有提示 */
    auto* aCompile = toolbar->addAction(makeIcon("compile"),
                                        "编译", this, &MainWindow::onCompile);
    aCompile->setToolTip("生成代码（luic，F5）");
    auto* aBuild = toolbar->addAction(makeIcon("build"),
                                      "构建", this, &MainWindow::onBuildProject);
    aBuild->setToolTip("构建工程（Ctrl+B）");
    auto* aRun = toolbar->addAction(makeIcon("run"),
                                    "运行", this, &MainWindow::onRunProject);
    aRun->setToolTip("运行（Ctrl+R）");
    auto* aBuildRun = toolbar->addAction(makeIcon("buildrun"),
                                         "构建运行", this, &MainWindow::onBuildAndRun);
    aBuildRun->setToolTip("全构建并运行（Ctrl+F5）");
    toolbar->addSeparator();
    toolbar->addAction(makeIcon("cut"), "剪切", this, &MainWindow::onCut);
    toolbar->addAction(makeIcon("copy"), "复制", this, &MainWindow::onCopy);
    toolbar->addAction(makeIcon("paste"), "粘贴", this, &MainWindow::onPaste);
    toolbar->addAction(makeIcon("delete"), "删除", this, &MainWindow::onDelete);
    toolbar->addSeparator();
    auto* undoAct = m_undoStack->createUndoAction(this, "撤销");
    undoAct->setIcon(makeIcon("undo"));
    toolbar->addAction(undoAct);
    auto* redoAct = m_undoStack->createRedoAction(this, "重做");
    redoAct->setIcon(makeIcon("redo"));
    toolbar->addAction(redoAct);
    auto* layoutBar = addToolBar("布局工具栏");
    layoutBar->setMovable(false);
    layoutBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    layoutBar->setIconSize(QSize(16, 16));
    layoutBar->addAction(makeIcon("vbox"),
                       "垂直", this, &MainWindow::onApplyLayoutVBox);
    layoutBar->addAction(makeIcon("hbox"),
                       "水平", this, &MainWindow::onApplyLayoutHBox);
    layoutBar->addAction(makeIcon("grid"),
                       "网格", this, &MainWindow::onApplyLayoutGrid);
    layoutBar->addAction(makeIcon("stacked"),
                       "堆叠", this, &MainWindow::onApplyLayoutStacked);
    toolbar->addSeparator();
    layoutBar->addAction(makeIcon("break"),
                       "解除", this, &MainWindow::onBreakLayout);
}

void MainWindow::setupDocks() {
    /* 左侧：一个停靠面板里放「组件 / 工程 / 组件树」三个页签，
     * 避免多个 QDockWidget tabify 之后留下空白 tab 条 */
    m_leftDock = new QDockWidget("组件 / 工程", this);
    m_leftDock->setFeatures(QDockWidget::DockWidgetMovable);
    auto* leftTabs = new QTabWidget(m_leftDock);
    leftTabs->setDocumentMode(true);
    /* 页签顺序（左→右）：工程 / 组件 / 组件树 */
    m_projectPanel = new ProjectPanel(this);
    leftTabs->addTab(m_projectPanel, "工程");
    leftTabs->addTab(m_compPanel, "组件");
    leftTabs->addTab(m_treePanel, "组件树");
    leftTabs->setCurrentIndex(0);
    m_leftDock->setWidget(leftTabs);
    addDockWidget(Qt::LeftDockWidgetArea, m_leftDock);

    m_buildDock = new QDockWidget("编译输出", this);
    m_buildPanel = new OutputPanel(this);
    m_buildDock->setWidget(m_buildPanel);
    m_buildDock->setFeatures(QDockWidget::DockWidgetMovable);
    addDockWidget(Qt::BottomDockWidgetArea, m_buildDock);

    m_runDock = new QDockWidget("运行输出", this);
    m_runPanel = new OutputPanel(this);
    m_runDock->setWidget(m_runPanel);
    m_runDock->setFeatures(QDockWidget::DockWidgetMovable);
    addDockWidget(Qt::BottomDockWidgetArea, m_runDock);
    tabifyDockWidget(m_buildDock, m_runDock);
    m_runDock->hide();
    /* 输出区默认高度 200px；与画布之间的分隔条可拖动调整 */
    m_buildPanel->setMinimumHeight(60);
    m_runPanel->setMinimumHeight(60);
    resizeDocks({m_buildDock, m_runDock}, {200, 200}, Qt::Vertical);


    m_propDock = new QDockWidget("属性", this);
    m_propDock->setWidget(m_propPanel);
    m_propDock->setFeatures(QDockWidget::DockWidgetMovable);
    m_propDock->setMinimumWidth(300);
    addDockWidget(Qt::RightDockWidgetArea, m_propDock);
}

void MainWindow::setupConnections() {
    /* Ctrl+点击（Qt 侧，带修饰键）：加选到多选集合 */
    connect(m_lvglWidget, &SDL2Widget::widgetClicked, this,
            [this](int x, int y, Qt::KeyboardModifiers mod) {
        if (!(mod & Qt::ControlModifier)) return;
        std::string id = m_renderer.findIdAt(x, y);
        if (id.empty() || id == LvglRenderer::kRootId) return;
        /* 第一次 Ctrl+点击：先把"当前单选的对象"并入多选集合，
         * 否则第二个对象会顶替掉第一个，看起来没加进去 */
        if (m_multiSelectedIds.empty() && !m_selectedId.empty() && m_selectedId != id)
            m_multiSelectedIds.push_back(m_selectedId);
        if (std::find(m_multiSelectedIds.begin(), m_multiSelectedIds.end(), id) ==
            m_multiSelectedIds.end())
            m_multiSelectedIds.push_back(id);
        m_renderer.selectMultiple(m_multiSelectedIds);
        statusBar()->showMessage(
            QString("已选中 %1 个组件（Ctrl+点击继续加选，然后点布局）")
                .arg(m_multiSelectedIds.size()), 5000);
    });

    connect(m_lvglWidget, &SDL2Widget::lvglReady, this, [this]() {
        m_renderer.setOnClick([this](const std::string& id) {
            /* Ctrl 按住时的点击交给 widgetClicked 做加选，这里不改变选中 */
            if (QGuiApplication::keyboardModifiers() & Qt::ControlModifier) return;
            /* 点中的是本就在多选集合里的对象：保留多选（例如多选后拖动） */
            const bool inMulti = m_multiSelectedIds.size() > 1 &&
                std::find(m_multiSelectedIds.begin(), m_multiSelectedIds.end(), id) !=
                    m_multiSelectedIds.end();
            m_selectedId = id;
            if (inMulti) {
                m_renderer.selectMultiple(m_multiSelectedIds);
                updateStatusBar();
                return;
            }
            m_multiSelectedIds.clear();   // 普通点击 = 单选，清空多选
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
    /* 工程级：复制到 <工程目录>/assets，登记进清单（而不是当前 .lui） */
    const bool projectMode = !m_projectFile.isEmpty();
    const QString baseDir = projectMode ? QFileInfo(m_projectFile).absolutePath()
                                        : m_doc.baseDir();
    QString assetsDir = QDir(baseDir).absoluteFilePath("assets");
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

    if (projectMode) {
        updateManifestResources(alias, ("assets/" + fi.fileName()).toStdString());
        const QString manifest = m_projectFile;
        loadProject(manifest);
    } else {
        m_doc.setResource(alias, ("assets/" + fi.fileName()).toStdString());
        m_doc.markModified();
    }
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

    /* 工程级资源：清单 resources，应用到所有文档 */
    m_projectResources.clear();
    if (root.contains("resources") && root["resources"].isObject()) {
        auto res = root["resources"].toObject();
        for (auto it = res.begin(); it != res.end(); ++it)
            m_projectResources[it.key().toStdString()] = it.value().toString().toStdString();
    }
    QSettings().setValue("project/last", manifest);   // 下次启动自动打开

    /* 默认构建目录：工程同级 build_lvgl（不存在就建；用户自定义在工程内的保持不动） */
    {
        const QString projDir = QFileInfo(manifest).absolutePath();
        const QString defBuild = QDir(projDir).absoluteFilePath("build_lvgl");
        const QString cur = QSettings().value("build/buildDir").toString();
        if (cur.isEmpty() || QFileInfo(cur).absolutePath() != projDir)
            QSettings().setValue("build/buildDir", defBuild);
        QDir().mkpath(defBuild);
    }
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
        doc->setProjectResources(m_projectResources, base);
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
    /* ---- 完整工程脚手架：CMakeLists.txt + main.cpp + ui_generated 目录 ---- */
    /* lvgl++ 仓库根：设计器可执行文件位于 <repo>/tools/lvgl-designer/build/ */
    QString repoRoot = QDir(QCoreApplication::applicationDirPath())
                           .absoluteFilePath("../../..");
    repoRoot = QDir(repoRoot).absolutePath();

    QString cmakeLists = QString(
        "cmake_minimum_required(VERSION 3.16)\n"
        "project(%1 C CXX)\n\n"
        "set(CMAKE_CXX_STANDARD 17)\n"
        "set(CMAKE_EXPORT_COMPILE_COMMANDS ON)\n\n"
        "# lvgl++ 仓库位置（换机器时改这里，或用 -DLVGLPP_ROOT=... 覆盖）\n"
        "if(NOT DEFINED LVGLPP_ROOT)\n"
        "  set(LVGLPP_ROOT \"%2\" CACHE PATH \"lvgl++ 仓库根目录\")\n"
        "endif()\n\n"
        "add_subdirectory(${LVGLPP_ROOT} ${CMAKE_BINARY_DIR}/lvglpp EXCLUDE_FROM_ALL)\n\n"
        "# .lui -> *_ui.hpp（改界面后重新编译会自动重生成）\n"
        "file(GLOB LUI_FILES ${CMAKE_CURRENT_SOURCE_DIR}/ui/screens/*.lui\n"
        "                    ${CMAKE_CURRENT_SOURCE_DIR}/ui/components/*.lui)\n"
        "set(UI_HEADERS \"\")\n"
        "file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ui_generated)\n"
        "add_custom_target(ui ALL\n"
        "    COMMAND ${LVGLPP_ROOT}/bin/luic${CMAKE_EXECUTABLE_SUFFIX} --project\n"
        "            ${CMAKE_CURRENT_SOURCE_DIR}/lvglpp.project.json\n"
        "            --out-dir ${CMAKE_CURRENT_SOURCE_DIR}/ui_generated\n"
        "    DEPENDS ${LUI_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/lvglpp.project.json\n"
        "    COMMENT \"luic: 生成 UI 代码\")\n\n"
        "# 图片资源：把工程清单里的 resources 转成 lv_image_dsc_t（qrc 式）\n"
        "find_program(LVGLPP_PYTHON NAMES python3 python)\n"
        "add_custom_target(assets\n"
        "    COMMAND ${LVGLPP_PYTHON} ${LVGLPP_ROOT}/scripts/assets/convert_assets.py\n"
        "            ${CMAKE_CURRENT_SOURCE_DIR}/lvglpp.project.json\n"
        "    COMMENT \"转换图片资源\")\n"
        "add_dependencies(ui assets)\n"
        "file(GLOB ASSET_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/assets/*.c)\n"
        "add_executable(%1 main.cpp ${ASSET_SOURCES})\n"
        "add_dependencies(%1 ui)\n"
        "target_include_directories(%1 PRIVATE\n"
        "    ${CMAKE_CURRENT_SOURCE_DIR}\n"
        "    ${CMAKE_CURRENT_SOURCE_DIR}/ui_generated\n"
        "    ${CMAKE_CURRENT_SOURCE_DIR}/ui\n"
        "    ${LVGLPP_ROOT})\n"
        "# SDL2：优先用 lvgl++ 仓库自带的查找模块\n"
        "list(APPEND CMAKE_MODULE_PATH \"${LVGLPP_ROOT}/cmake\" \"${LVGLPP_ROOT}/lvgl\")\n"
        "find_package(SDL2)\n"
        "find_package(SDL2_image)\n"
        "if(NOT SDL2_FOUND)\n"
        "  message(FATAL_ERROR \"未找到 SDL2：请安装 libsdl2-dev / SDL2（Windows 用 vcpkg 或官方开发包）\")\n"
        "endif()\n"
        "target_link_libraries(%1 PRIVATE lvgl lvgl::thorvg\n"
        "    ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} m pthread)\n"
    ).arg(name).arg(repoRoot);
    {
        QFile f(dir + "/CMakeLists.txt");
        if (f.open(QIODevice::WriteOnly)) f.write(cmakeLists.toUtf8());
    }
    QString mainCpp = QString(
        "#include \"lvgl++/lvgl_widgets.hpp\"\n"
        "#include \"lvgl++/core/signal_handler.hpp\"\n"
        "#include \"%1_ui.hpp\"\n"
        "#include <unistd.h>\n\n"
        "using namespace lvgl;   // 生成的界面类在 namespace lvgl 里\n\n"
        "// 入口：由 lvglpp.project.json 的 entry 指定屏幕\n"
        "class App : public lvgl::%1_UI {\n"
        "public:\n"
        "    // 在这里 override on_<id>_<event>(lv_event_t*) 处理交互\n"
        "};\n\n"
        "int main() {\n"
        "    lv_init();\n"
        "    lv_sdl_window_create(%1_UI::kScreenWidth, %1_UI::kScreenHeight);\n"
        "    lv_sdl_mouse_create();\n"
        "    lvgl::SignalHandler signal_handler;\n"
        "    App app;\n"
        "    app.setup();\n"
        "    app.bind_events();\n"
        "    while (signal_handler.is_running()) { lv_timer_handler(); usleep(5000); }\n"
        "    return 0;\n"
        "}\n"
    ).arg(screenName);
    {
        QFile f(dir + "/main.cpp");
        if (f.open(QIODevice::WriteOnly)) f.write(mainCpp.toUtf8());
    }
    QDir().mkpath(dir + "/ui_generated");

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

/* 在工程清单里登记图片资源（已存在则覆盖同名项） */
bool MainWindow::updateManifestResources(const std::string& alias, const std::string& rel) {
    if (m_projectFile.isEmpty()) return false;
    QFile f(m_projectFile);
    if (!f.open(QIODevice::ReadOnly)) return false;
    auto root = QJsonDocument::fromJson(f.readAll()).object();
    f.close();
    QJsonObject res = root["resources"].toObject();
    res[QString::fromStdString(alias)] = QString::fromStdString(rel);
    root["resources"] = res;
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;
    f.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return true;
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

/* 运行：启动构建产物，stdout/stderr 进输出面板 */
/* 一步：构建工程，成功后直接运行 */
void MainWindow::onBuildAndRun() {
    onBuildProject();
    onRunProject();
}

void MainWindow::onRunProject() {
    const QString buildDir = SettingsDialog::buildDir();
    if (buildDir.isEmpty() || m_projectFile.isEmpty()) {
        QMessageBox::information(this, "运行", "请先打开工程并在「设置」里配置构建目录。");
        return;
    }
    QFile mf(m_projectFile);
    QString name = QFileInfo(m_projectFile).completeBaseName();
    if (mf.open(QIODevice::ReadOnly)) {
        auto j = QJsonDocument::fromJson(mf.readAll()).object();
        if (!j["name"].toString().isEmpty()) name = j["name"].toString();
    }
    QStringList cands{buildDir + "/" + name, buildDir + "/" + name + ".exe",
                      buildDir + "/Debug/" + name + ".exe", buildDir + "/Release/" + name + ".exe"};
    QString exe;
    for (auto& c : cands) if (QFileInfo::exists(c)) { exe = c; break; }
    if (exe.isEmpty()) {
        m_runPanel->append("找不到可执行文件：" + cands.join(" / ") + "\n");
        m_runDock->show();
        m_runDock->raise();
        return;
    }
    if (m_runProc) { m_runProc->kill(); m_runProc->deleteLater(); }
    m_runProc = new QProcess(this);
    m_runProc->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_runProc, &QProcess::readyReadStandardOutput, this, [this] {
        m_runPanel->append(QString::fromLocal8Bit(m_runProc->readAllStandardOutput()));
    });
    connect(m_runProc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this](int code, QProcess::ExitStatus) {
                m_runPanel->append(QString("\n=== 程序退出，code=%1 ===\n").arg(code));
            });
    m_runPanel->append("=== 运行 " + exe + " ===\n");
    m_runDock->show();
    m_runDock->raise();
    m_runProc->start(exe, QStringList());
}

void MainWindow::onSettings() {
    SettingsDialog dlg(this);
    dlg.exec();
}

/* 构建工程：先 cmake 配置（构建目录为空时），再 cmake --build */
void MainWindow::onBuildProject() {
    if (m_projectFile.isEmpty()) {
        QMessageBox::information(this, "构建工程", "请先新建或打开一个工程（构建需要工程的 CMakeLists.txt）。");
        return;
    }
    const QString srcDir = QFileInfo(m_projectFile).absolutePath();
    QString buildDir = SettingsDialog::buildDir();
    if (buildDir.isEmpty()) buildDir = srcDir + "/build";   // 默认放工程目录下
    const QString cmake = SettingsDialog::cmakePath();
    QDir().mkpath(buildDir);
    /* 构建目录里若已有指向别的源码树的缓存，必须清掉，否则会去构建那个工程 */
    const QString cache = buildDir + "/CMakeCache.txt";
    if (QFileInfo::exists(cache)) {
        QFile cf(cache);
        bool otherSource = false;
        if (cf.open(QIODevice::ReadOnly)) {
            for (auto& line : QString::fromLocal8Bit(cf.readAll()).split('\n')) {
                if (line.startsWith("CMAKE_HOME_DIRECTORY:")) {
                    const QString v = line.section('=', 1).trimmed();
                    if (v != srcDir) otherSource = true;
                }
            }
        }
        if (otherSource) {
            if (QMessageBox::question(this, "构建工程",
                    "构建目录已被其它工程使用：\n" + buildDir + "\n\n清空并重新配置？")
                != QMessageBox::Yes)
                return;
            QDir(buildDir).removeRecursively();
            QDir().mkpath(buildDir);
        }
    }
    if (!QFileInfo::exists(buildDir + "/CMakeCache.txt")) {
        m_buildPanel->append("=== 配置 " + srcDir + " -> " + buildDir + " ===\n");
        QStringList cfg{"-S", srcDir, "-B", buildDir,
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
    m_buildPanel->append("=== 构建 " + buildDir + " ===\n");
    m_buildDock->show();
    m_buildDock->raise();
    QProcess b;
    b.start(cmake, {"--build", buildDir, "-j"});
    statusBar()->showMessage("正在构建…");
    if (!b.waitForFinished(600000)) {
        QMessageBox::critical(this, "构建工程", "构建超时。");
        return;
    }
    const QString out = QString::fromLocal8Bit(b.readAllStandardError());
    m_buildPanel->append(QString::fromLocal8Bit(b.readAllStandardOutput()));
    m_buildPanel->append(out);
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
    m_buildPanel->append("=== 编译 " + m_doc.filePath() + " ===\n");
    m_buildDock->show();
    m_buildDock->raise();
    proc.start(luicPath, args);
    statusBar()->showMessage("正在编译...");

    if (!proc.waitForFinished(10000)) {
        m_buildPanel->append("luic 执行超时（10 秒）\n");
        m_buildDock->show();
        m_buildDock->raise();
        statusBar()->showMessage("编译失败：超时", 5000);
        return;
    }

    QString stdoutStr = proc.readAllStandardOutput();
    QString stderrStr = proc.readAllStandardError();
    if (!stdoutStr.isEmpty()) m_buildPanel->append(stdoutStr);
    if (!stderrStr.isEmpty()) m_buildPanel->append(stderrStr);
    m_buildDock->show();
    m_buildDock->raise();
    if (proc.exitCode() == 0) {
        statusBar()->showMessage("编译成功", 5000);
        m_buildPanel->append("=== 编译成功 ===\n");
    } else {
        statusBar()->showMessage("编译失败", 5000);
        m_buildPanel->append(QString("=== 编译失败（luic 退出码 %1）===\n").arg(proc.exitCode()));
    }
}

void MainWindow::onAbout() {
    QMessageBox::about(this, "关于 LVGL++ Designer",
        "<h3>LVGL++ Designer</h3>"
        "<p>LVGL++ 项目的可视化 UI 设计器。</p>"
        "<p>Version 1.0</p>"
        "<p>作者：吃荤小道长</p>");
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
