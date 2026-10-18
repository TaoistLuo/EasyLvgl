/**************************************************************************************************
 * @file        bar.hpp
 * @brief       LBar 组件封装
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

class LBar : public LWidgetCRTP<LBar> {
public:
    explicit LBar(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_bar_create(parent ? parent : lv_scr_act())) {}

    LBar& set_value(int32_t value, lv_anim_enable_t anim = LV_ANIM_OFF) {
        lv_bar_set_value(m_obj, value, anim);
        return *this;
    }

    int32_t value() const {
        return lv_bar_get_value(m_obj);
    }

    LBar& set_start_value(int32_t start_value, lv_anim_enable_t anim = LV_ANIM_OFF) {
        lv_bar_set_start_value(m_obj, start_value, anim);
        return *this;
    }

    LBar& set_range(int32_t min, int32_t max) {
        lv_bar_set_range(m_obj, min, max);
        return *this;
    }

    LBar& set_mode(lv_bar_mode_t mode) {
        lv_bar_set_mode(m_obj, mode);
        return *this;
    }

    LBar& set_mode(int mode) {
        return set_mode(static_cast<lv_bar_mode_t>(mode));
    }

    LBar& set_orientation(bool horizontal) {
        if (horizontal) {
            lv_obj_set_size(m_obj, 200, 20);
        } else {
            lv_obj_set_size(m_obj, 20, 200);
        }
        return *this;
    }

    LBar& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LBar& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    LBar& set_align(lv_align_t align) {
        lv_obj_set_align(m_obj, align);
        return *this;
    }

    LBar& set_colors(uint32_t bg_color, uint32_t indicator_color) {
        lv_obj_set_style_bg_color(m_obj, lv_color_hex(bg_color), LV_PART_MAIN);
        lv_obj_set_style_bg_color(m_obj, lv_color_hex(indicator_color), LV_PART_INDICATOR);
        
        return *this;
    }

    LBar& set_indeterminate(bool enable) {
        if (enable) {
            lv_bar_set_mode(m_obj, LV_BAR_MODE_NORMAL);
            lv_bar_set_value(m_obj, 0, LV_ANIM_ON);
        }
        return *this;
    }
};

} // namespace easylvgl
