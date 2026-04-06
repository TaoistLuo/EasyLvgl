#pragma once
#include "canvas/WidgetItem.hpp"
#include "canvas/items/ButtonItem.hpp"
#include "model/LuiDocument.hpp"

namespace designer {

class ItemFactory {
public:
    static WidgetItem* create(WidgetNode& node, QGraphicsItem* parentItem = nullptr) {
        const auto& t = node.type;
        QPointF p = node.pos();
        QSizeF s = node.size();

        WidgetItem* item = nullptr;

        if (t == "LButton")        item = new ButtonItem(node.id, t, p, s, parentItem);
        else if (t == "LLabel")    item = new LabelItem(node.id, t, p, s, parentItem);
        else if (t == "LSlider")   item = new SliderItem(node.id, t, p, s, parentItem);
        else if (t == "LBar")      item = new BarItem(node.id, t, p, s, parentItem);
        else if (t == "LCheckbox") item = new CheckboxItem(node.id, t, p, s, parentItem);
        else if (t == "LSwitch")   item = new SwitchItem(node.id, t, p, s, parentItem);
        else if (t == "LDropdown") item = new DropdownItem(node.id, t, p, s, parentItem);
        else if (t == "LTextArea") item = new TextAreaItem(node.id, t, p, s, parentItem);
        else if (t == "LArc")      item = new ArcItem(node.id, t, p, s, parentItem);
        else if (t == "LSpinner")  item = new SpinnerItem(node.id, t, p, s, parentItem);
        else if (t == "LLed")      item = new LedItem(node.id, t, p, s, parentItem);
        else if (t == "LVStack" || t == "LHStack" || t == "LList" ||
                 t == "LTabView" || t == "LMenu" || t == "LTileView" || t == "LWin")
                                   item = new ContainerItem(node.id, t, p, s, parentItem);
        else                       item = new GenericItem(node.id, t, p, s, parentItem);

        if (!item) return nullptr;

        bool isContainer = (t == "LVStack" || t == "LHStack" || t == "LList" ||
                           t == "LTabView" || t == "LMenu" || t == "LTileView" || t == "LWin");
        item->setIsContainer(isContainer);
        item->setAllProps(node.props);
        item->applyObjectProps();

        for (auto& child : node.children) {
            create(child, item);
        }

        return item;
    }
};

}
