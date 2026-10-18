/**************************************************************************************************
 * @file        roller.hpp
 * @brief       LRoller 组件封装
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

class LRoller : public LWidgetCRTP<LRoller> {
public:
    explicit LRoller(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_roller_create(parent ? parent : lv_scr_act())) {}

    LRoller& set_options(const char* opts, lv_roller_mode_t mode = LV_ROLLER_MODE_NORMAL) {
        lv_roller_set_options(m_obj, opts, mode);
        return *this;
    }

    LRoller& set_selected(uint16_t idx, lv_anim_enable_t anim = LV_ANIM_OFF) {
        lv_roller_set_selected(m_obj, idx, anim);
        return *this;
    }

    uint16_t selected() const {
        return lv_roller_get_selected(m_obj);
    }

    LRoller& set_visible_row_count(uint8_t row_cnt) {
        lv_roller_set_visible_row_count(m_obj, row_cnt);
        return *this;
    }

    LRoller& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    LRoller& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    template<typename Func>
    LRoller& on_change(Func&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<Func>(callback));
        return *this;
    }
};

} // namespace easylvgl
