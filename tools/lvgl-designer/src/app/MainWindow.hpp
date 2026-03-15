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
#include "model/LuiDocument.hpp"
#include "canvas/DesignerScene.hpp"
#include "canvas/DesignerView.hpp"
#include "panels/ComponentPanel.hpp"
#include "panels/PropertyPanel.hpp"
#include "panels/TreePanel.hpp"

namespace designer {

class MainWindow : public QMainWindow {
    Q_OBJECT

    LuiDocument m_doc;
    DesignerScene* m_scene;
    DesignerView* m_view;
    ComponentPanel* m_compPanel;
    PropertyPanel* m_propPanel;
    TreePanel* m_treePanel;
    QUndoStack* m_undoStack;

public:
    explicit MainWindow(QWidget* parent = nullptr);
    bool openFile(const QString& path);

private:
    void setupMenus();
    void setupToolbar();
    void setupDocks();
    void setupConnections();
    void updateTitle();

    void onSave();
    void onSaveAs();
    void onCompile();
    void onDelete();
};

}
