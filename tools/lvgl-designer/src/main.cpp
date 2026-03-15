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
    app.setStyleSheet("QToolTip { color: #cccccc; background-color: #2d2d2d; border: 1px solid #3c3c3c; }");

    designer::MainWindow window;
    window.show();

    if (argc > 1) {
        window.openFile(QString::fromLocal8Bit(argv[1]));
    }

    return app.exec();
}
