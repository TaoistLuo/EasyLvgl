/**************************************************************************************************
 * @file        tabview.hpp
 * @brief       LTabview 组件封装
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
#include "easylvgl/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace easylvgl {

class LTabView : public LWidgetCRTP<LTabView> {
public:
    explicit LTabView(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_tabview_create(parent ? parent : lv_scr_act())) {}

    lv_obj_t* add_tab(const char* name) {
        return lv_tabview_add_tab(m_obj, name);
    }

    LTabView& set_active(uint32_t id, lv_anim_enable_t anim = LV_ANIM_OFF) {
        lv_tabview_set_active(m_obj, id, anim);
        return *this;
    }

    uint32_t active_tab() const {
        return lv_tabview_get_tab_active(m_obj);
    }

    LTabView& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LTabView& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
    LTabView& set_tab_bar_position(int dir) { lv_tabview_set_tab_bar_position(m_obj, static_cast<lv_dir_t>(dir)); return *this; }

    LTabView& set_tab_bar_size(int size) { lv_tabview_set_tab_bar_size(m_obj, size); return *this; }

};

} // namespace easylvgl
