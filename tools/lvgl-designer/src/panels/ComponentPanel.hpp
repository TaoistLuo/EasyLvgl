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

        addGroup("Basic", {
            {"LButton",   "Button"},
            {"LLabel",    "Label"},
            {"LImage",    "Image"},
        });

        addGroup("Input", {
            {"LSlider",       "Slider"},
            {"LCheckbox",     "Checkbox"},
            {"LSwitch",       "Switch"},
            {"LDropdown",     "Dropdown"},
            {"LTextArea",     "TextArea"},
            {"LRoller",       "Roller"},
            {"LSpinbox",      "Spinbox"},
            {"LKeyboard",     "Keyboard"},
            {"LButtonMatrix", "ButtonMatrix"},
        });

        addGroup("Display", {
            {"LBar",      "Bar"},
            {"LArc",      "Arc"},
            {"LSpinner",  "Spinner"},
            {"LLed",      "Led"},
            {"LLine",     "Line"},
            {"LChart",    "Chart"},
            {"LTable",    "Table"},
            {"LScale",    "Scale"},
            {"LCalendar", "Calendar"},
            {"LCanvas",   "Canvas"},
            {"LSpan",     "Span"},
        });

        addGroup("Container", {
            {"LVStack",   "VStack"},
            {"LHStack",   "HStack"},
            {"LList",     "List"},
            {"LTabView",  "TabView"},
            {"LMenu",     "Menu"},
            {"LTileView", "TileView"},
            {"LWin",      "Win"},
        });

        addGroup("Dialog", {
            {"LMsgBox",   "MsgBox"},
        });

        addGroup("Animation", {
            {"LAnimImage",   "AnimImage"},
            {"LLottie",      "Lottie"},
        });

        addGroup("Advanced", {
            {"LImageButton", "ImageButton"},
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
