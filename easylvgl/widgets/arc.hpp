/**************************************************************************************************
 * @file        arc.hpp
 * @brief       LArc 组件封装
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
#include "easylvgl/core/style.hpp"
#include "easylvgl/core/event.hpp"
#include "lvgl/lvgl.h"

namespace easylvgl {

class LArc : public LWidgetCRTP<LArc> {
public:
    using LWidgetCRTP<LArc>::set_size;

    explicit LArc(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_arc_create(parent ? parent : lv_scr_act())) {}

    LArc& set_value(int16_t value) {
        lv_arc_set_value(m_obj, value);
        return *this;
    }

    int16_t value() const {
        return lv_arc_get_value(m_obj);
    }

    LArc& set_range(int16_t min, int16_t max) {
        lv_arc_set_range(m_obj, min, max);
        return *this;
    }

    LArc& set_mode(lv_arc_mode_t mode) {
        lv_arc_set_mode(m_obj, mode);
        return *this;
    }

    LArc& set_mode(int mode) {
        return set_mode(static_cast<lv_arc_mode_t>(mode));
    }

    LArc& set_rotation(uint16_t rotation) {
        lv_arc_set_rotation(m_obj, rotation);
        return *this;
    }

    LArc& set_bg_angles(uint16_t start, uint16_t end) {
        lv_arc_set_bg_angles(m_obj, start, end);
        return *this;
    }

    LArc& set_angles(uint16_t start, uint16_t end) {
        lv_arc_set_angles(m_obj, start, end);
        return *this;
    }

    LArc& set_change_rate(uint16_t rate) {
        lv_arc_set_change_rate(m_obj, rate);
        return *this;
    }

    LArc& set_size(int size) {
        lv_obj_set_size(m_obj, size, size);
        return *this;
    }

    LArc& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    template<typename Func>
    LArc& on_change(Func&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<Func>(callback));
        return *this;
    }

    LArc& set_knob_enabled(bool enable) {
        if (!enable) {
            lv_obj_remove_flag(m_obj, LV_OBJ_FLAG_CLICKABLE);
        } else {
            lv_obj_add_flag(m_obj, LV_OBJ_FLAG_CLICKABLE);
        }
        return *this;
    }

    LArc& set_colors(uint32_t bg_color, uint32_t fg_color) {
        lv_obj_set_style_arc_color(m_obj, lv_color_hex(bg_color), LV_PART_MAIN);
        lv_obj_set_style_arc_color(m_obj, lv_color_hex(fg_color), LV_PART_INDICATOR);
        
        return *this;
    }
    LArc& set_knob_offset(int offset) { lv_arc_set_knob_offset(m_obj, offset); return *this; }

};

} // namespace easylvgl
