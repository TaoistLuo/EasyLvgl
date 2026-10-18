/**************************************************************************************************
 * @file        switch.hpp
 * @brief       LSwitch 组件封装
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

/**
 */
class LSwitch : public LWidgetCRTP<LSwitch> {
public:
    explicit LSwitch(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_switch_create(parent ? parent : lv_scr_act())) {}

    LSwitch& set_checked(bool state) {
        if (state) {
            lv_obj_add_state(m_obj, LV_STATE_CHECKED);
        } else {
            lv_obj_remove_state(m_obj, LV_STATE_CHECKED);
        }
        return *this;
    }

    bool is_checked() const {
        return lv_obj_has_state(m_obj, LV_STATE_CHECKED);
    }

    LSwitch& toggle() {
        if (is_checked()) {
            lv_obj_remove_state(m_obj, LV_STATE_CHECKED);
        } else {
            lv_obj_add_state(m_obj, LV_STATE_CHECKED);
        }
        return *this;
    }

    LSwitch& on() {
        return set_checked(true);
    }

    LSwitch& off() {
        return set_checked(false);
    }

    LSwitch& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    LSwitch& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LSwitch& set_align(lv_align_t align) {
        lv_obj_set_align(m_obj, align);
        return *this;
    }

    template<typename Func>
    LSwitch& on_change(Func&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<Func>(callback));
        return *this;
    }

    LSwitch& set_enabled(bool enabled) {
        if (enabled) {
            lv_obj_remove_state(m_obj, LV_STATE_DISABLED);
        } else {
            lv_obj_add_state(m_obj, LV_STATE_DISABLED);
        }
        return *this;
    }

    LSwitch& set_colors(uint32_t bg_color, uint32_t checked_color) {
        lv_obj_set_style_bg_color(m_obj, lv_color_hex(bg_color), LV_PART_MAIN);
        lv_obj_set_style_bg_color(m_obj, lv_color_hex(checked_color), LV_PART_INDICATOR);
        return *this;
    }
};

} // namespace easylvgl
