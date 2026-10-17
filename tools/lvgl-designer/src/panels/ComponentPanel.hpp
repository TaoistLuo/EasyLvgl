#pragma once
#include <QTreeWidget>
#include <QDrag>
#include <QMimeData>
#include <QHeaderView>

namespace designer {

class ComponentPanel : public QTreeWidget {
    Q_OBJECT
public:
    explicit ComponentPanel(QWidget* parent = nullptr) : QTreeWidget(parent) {
        setDragEnabled(true);
        setHeaderHidden(true);
        setIndentation(16);
        setIconSize(QSize(16, 16));

        struct CompDef { const char* type; const char* label; };

        auto addGroup = [this](const char* name, std::initializer_list<CompDef> items) {
            auto* group = new QTreeWidgetItem(this);
            group->setText(0, name);
            group->setFlags(Qt::ItemIsEnabled);
            QFont f = group->font(0);
            f.setBold(true);
            group->setFont(0, f);
            for (auto& c : items) {
                auto* item = new QTreeWidgetItem(group);
                item->setText(0, c.label);
                item->setData(0, Qt::UserRole, c.type);
                item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
            }
            group->setExpanded(true);
        };

        addGroup("Layouts", {
            {"LVBoxLayout",    "VBox Layout"},
            {"LHBoxLayout",    "HBox Layout"},
            {"LGridLayout",    "Grid Layout"},
            {"LStackedLayout", "Stacked Layout"},
        });

        addGroup("Buttons", {
            {"LButton",        "Push Button"},
            {"LImageButton",   "Image Button"},
            {"LCheckbox",      "Check Box"},
            {"LRadioButton",   "Radio Button"},
            {"LSwitch",        "Switch"},
            {"LButtonMatrix",  "Button Matrix"},
        });

        addGroup("Containers", {
            {"LGroupBox",      "Group Box"},
            {"LList",          "List"},
            {"LTabView",       "Tab View"},
            {"LMenu",          "Menu"},
            {"LMenuPage",      "Menu Page"},
            {"LMenuCont",      "Menu Container"},
            {"LMenuSection",   "Menu Section"},
            {"LTileView",      "Tile View"},
            {"LWin",           "Window"},
            {"LCanvas",        "Canvas"},
        });

        addGroup("Input", {
            {"LLineEdit",      "Line Edit"},
            {"LTextEdit",      "Text Edit"},
            {"LComboBox",      "Combo Box"},
            {"LSlider",        "Slider"},
            {"LSpinbox",       "Spin Box"},
            {"LRoller",        "Roller"},
            {"LKeyboard",      "Keyboard"},
        });

        addGroup("Display", {
            {"LLabel",         "Label"},
            {"LImage",         "Image"},
            {"LBar",           "Bar"},
            {"LArc",           "Arc"},
            {"LSpinner",       "Spinner"},
            {"LLed",           "LED"},
            {"LLine",          "Line"},
            {"LScale",         "Scale"},
            {"LSpan",          "Span"},
        });

        addGroup("Views / Data", {
            {"LTable",         "Table"},
            {"LChart",         "Chart"},
            {"LCalendar",      "Calendar"},
        });

        addGroup("Dialogs", {
            {"LMsgBox",        "Message Box"},
        });

        addGroup("Animation", {
            {"LAnimImage",     "Animated Image"},
            {"LLottie",        "Lottie"},
        });
    }

protected:
    void startDrag(Qt::DropActions) override {
        auto* item = currentItem();
        if (!item || item->data(0, Qt::UserRole).isNull()) return;

        auto* drag = new QDrag(this);
        auto* mime = new QMimeData();
        mime->setData("application/x-lvgl-widget",
                      item->data(0, Qt::UserRole).toString().toUtf8());
        drag->setMimeData(mime);
        drag->exec(Qt::CopyAction);
    }
};

}
