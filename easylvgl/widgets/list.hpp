/**************************************************************************************************
 * @file        list.hpp
 * @brief       LList 组件封装
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
#include "easylvgl/core/event.hpp"
#include "lvgl/lvgl.h"

namespace easylvgl {

class LList : public LWidgetCRTP<LList> {
public:
    explicit LList(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_list_create(parent ? parent : lv_scr_act())) {}

    lv_obj_t* add_text(const char* txt) {
        return lv_list_add_text(m_obj, txt);
    }

    lv_obj_t* add_button(const char* icon, const char* txt) {
        return lv_list_add_button(m_obj, icon, txt);
    }

    const char* get_button_text(lv_obj_t* btn) {
        return lv_list_get_button_text(m_obj, btn);
    }

    LList& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LList& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
};

} // namespace easylvgl
