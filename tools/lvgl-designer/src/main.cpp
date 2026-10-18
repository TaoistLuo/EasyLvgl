#include <QApplication>
#include "app/MainWindow.hpp"
#include <QDebug>
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("LVGL++ Designer");
    app.setOrganizationName("lvglpp");
    qDebug()<<"test start";
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(0x25, 0x25, 0x26));
    darkPalette.setColor(QPalette::WindowText, QColor(0xcc, 0xcc, 0xcc));
    darkPalette.setColor(QPalette::Base, QColor(0x1e, 0x1e, 0x1e));
    darkPalette.setColor(QPalette::AlternateBase, QColor(0x2d, 0x2d, 0x2d));
    darkPalette.setColor(QPalette::ToolTipBase, QColor(0x25, 0x25, 0x26));
    darkPalette.setColor(QPalette::ToolTipText, QColor(0xcc, 0xcc, 0xcc));
    darkPalette.setColor(QPalette::Text, QColor(0xcc, 0xcc, 0xcc));
    darkPalette.setColor(QPalette::Button, QColor(0x3c, 0x3c, 0x3c));
    darkPalette.setColor(QPalette::ButtonText, QColor(0xcc, 0xcc, 0xcc));
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(0x56, 0x9c, 0xd6));
    darkPalette.setColor(QPalette::Highlight, QColor(0x26, 0x4f, 0x78));
    darkPalette.setColor(QPalette::HighlightedText, Qt::white);
    app.setPalette(darkPalette);
    /* 统一的深色风格（参照 Qt Creator）：扁平、低对比边框、紧凑的控件尺寸 */
    QFont appFont("Segoe UI");
    appFont.setStyleHint(QFont::SansSerif);
    appFont.setPointSize(10);   // Qt Creator 界面字号约 9-10pt；过小，取 10pt
    app.setFont(appFont);

    app.setStyleSheet(R"QSS(
QMainWindow, QDialog { background: #2b2b2b; }
QWidget { color: #d0d0d0; }

QMenuBar { background: #2b2b2b; border-bottom: 1px solid #3c3c3c; padding: 2px 4px; }
QMenuBar::item { background: transparent; padding: 4px 9px; border-radius: 3px; }
QMenuBar::item:selected { background: #3d5a73; }
QMenu { background: #2f2f2f; border: 1px solid #4a4a4a; padding: 4px; }
QMenu::item { padding: 5px 24px 5px 20px; border-radius: 3px; }
QMenu::item:selected { background: #3d5a73; }
QMenu::separator { height: 1px; background: #4a4a4a; margin: 4px 8px; }

QToolBar { background: #2b2b2b; border-bottom: 1px solid #3c3c3c; padding: 2px; spacing: 1px; }
QToolBar::separator { width: 1px; background: #454545; margin: 4px 3px; }
QToolButton { background: transparent; border: 1px solid transparent; border-radius: 4px;
              padding: 3px 6px; color: #d0d0d0; }
QToolButton:hover { background: #3a3a3a; border-color: #4a4a4a; }
QToolButton:pressed, QToolButton:checked { background: #2f5d8a; border-color: #3c78b4; }

QPushButton { background: #3a3a3a; border: 1px solid #4a4a4a; border-radius: 4px; padding: 5px 14px; }
QPushButton:hover { background: #454545; }
QPushButton:pressed { background: #2f5d8a; }
QPushButton:disabled { color: #777; background: #333; border-color: #3c3c3c; }

QDockWidget { color: #cfcfcf; titlebar-close-icon: none; }
QDockWidget::title { background: #323232; padding: 5px 8px; border: 1px solid #3c3c3c; }
QTabBar::tab { background: #2f2f2f; color: #b8b8b8; padding: 6px 14px;
               border: 1px solid #3c3c3c; border-bottom: none; }
QTabBar::tab:selected { background: #2b2b2b; color: #e8e8e8; border-top: 2px solid #3c78b4; }
QTabBar::tab:hover { background: #383838; }

QTreeWidget, QTreeView, QListView {
    background: #252525; border: 1px solid #3c3c3c; outline: none;
    alternate-background-color: #282828; }
QTreeWidget::item, QTreeView::item { padding: 4px 2px; border: none; }
QTreeWidget::item:hover, QTreeView::item:hover { background: #333333; }
QTreeWidget::item:selected, QTreeView::item:selected { background: #2f5d8a; color: #ffffff; }
QHeaderView::section { background: #323232; color: #cfcfcf; padding: 4px 6px;
                       border: none; border-right: 1px solid #3c3c3c; }

QGroupBox { border: 1px solid #3c3c3c; border-radius: 4px; margin-top: 14px; padding-top: 6px; }
QGroupBox::title { subcontrol-origin: margin; left: 8px; padding: 1px 5px;
                   color: #9fc3e0; font-weight: 600; }

QLineEdit, QPlainTextEdit, QTextEdit, QComboBox {
    background: #1f1f1f; border: 1px solid #454545; border-radius: 3px;
    padding: 3px 6px; selection-background-color: #2f5d8a; }
QLineEdit:focus, QPlainTextEdit:focus, QComboBox:focus { border-color: #3c78b4; }
QComboBox::drop-down { width: 18px; border: none; background: transparent; }
/* 自绘下拉箭头：QSS 里一旦设置 drop-down，Qt 就不再画默认箭头 */
QComboBox::down-arrow {
    width: 0; height: 0;
    border-left: 4px solid transparent;
    border-right: 4px solid transparent;
    border-top: 5px solid #c8c8c8;
    margin-right: 5px; }
QComboBox::down-arrow:disabled { border-top-color: #6a6a6a; }
QComboBox QAbstractItemView { background: #2f2f2f; border: 1px solid #4a4a4a;
                              selection-background-color: #2f5d8a; }

QCheckBox::indicator { width: 14px; height: 14px; border: 1px solid #5a5a5a;
                       border-radius: 3px; background: #1f1f1f; }
QCheckBox::indicator:checked { background: #3c78b4; border-color: #5a9ad6; }

QStatusBar { background: #2b2b2b; color: #a8a8a8; border-top: 1px solid #3c3c3c; }
QScrollBar:vertical { background: #262626; width: 12px; margin: 0; }
QScrollBar::handle:vertical { background: #4a4a4a; border-radius: 6px; min-height: 24px; }
QScrollBar::handle:vertical:hover { background: #5a5a5a; }
QScrollBar::add-line, QScrollBar::sub-line { height: 0; }
QScrollBar:horizontal { background: #262626; height: 12px; }
QScrollBar::handle:horizontal { background: #4a4a4a; border-radius: 6px; min-width: 24px; }
QToolTip { color: #dcdcdc; background: #2f2f2f; border: 1px solid #4a4a4a; padding: 3px; }
)QSS");

    designer::MainWindow window;
    window.show();

    if (argc > 1) {
        QString arg = QString::fromLocal8Bit(argv[1]);
        /* 传工程清单（.json）则按工程打开，传 .lui 则单文件打开 */
        if (arg.endsWith(".json", Qt::CaseInsensitive))
            window.loadProject(arg);
        else
            window.openFile(arg);
    }

    return app.exec();
}
