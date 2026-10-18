/**************************************************************************************************
 * @file        spinbox.hpp
 * @brief       LSpinbox 组件封装
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

class LSpinbox : public LWidgetCRTP<LSpinbox> {
public:
    explicit LSpinbox(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_spinbox_create(parent ? parent : lv_scr_act())) {}

    LSpinbox& set_value(int32_t val) {
        lv_spinbox_set_value(m_obj, val);
        return *this;
    }

    int32_t value() const {
        return lv_spinbox_get_value(m_obj);
    }

    LSpinbox& set_range(int32_t min, int32_t max) {
        lv_spinbox_set_range(m_obj, min, max);
        return *this;
    }

    LSpinbox& set_step(uint32_t step) {
        lv_spinbox_set_step(m_obj, step);
        return *this;
    }

    LSpinbox& set_digit_format(uint8_t digit_count, uint8_t separator_position) {
        lv_spinbox_set_digit_format(m_obj, digit_count, separator_position);
        return *this;
    }

    LSpinbox& set_rollover(bool en) {
        lv_spinbox_set_rollover(m_obj, en);
        return *this;
    }

    LSpinbox& increment() {
        lv_spinbox_increment(m_obj);
        return *this;
    }

    LSpinbox& decrement() {
        lv_spinbox_decrement(m_obj);
        return *this;
    }

    LSpinbox& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    LSpinbox& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
    LSpinbox& set_digit_step_direction(int dir) { lv_spinbox_set_digit_step_direction(m_obj, static_cast<lv_dir_t>(dir)); return *this; }

    LSpinbox& set_cursor_pos(int pos) { lv_spinbox_set_cursor_pos(m_obj, pos); return *this; }

};

} // namespace easylvgl
