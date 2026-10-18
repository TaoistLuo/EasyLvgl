/**************************************************************************************************
 * @file        ItemFactory.hpp
 * @brief       EasyLVGL 设计器
 * @details     EasyLVGL —— 让 LVGL 开发像 Qt 一样简单：C++ 封装 + 可视化设计器 + 代码生成。
 *
 * @author      taoist.luo
 * @version     1.0.0
 * @date        2026-10-18
 *
 * Copyright (c) 2026 taoist.luo (https://github.com/TaoistLuo/EasyLVGL)
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *************************************************************************************************/

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
        else if (t == "LRadioButton") item = new RadioButtonItem(node.id, t, p, s, parentItem);
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
