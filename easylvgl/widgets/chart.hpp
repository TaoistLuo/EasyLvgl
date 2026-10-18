/**************************************************************************************************
 * @file        chart.hpp
 * @brief       LChart 组件封装
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

class LChart : public LWidgetCRTP<LChart> {
public:
    explicit LChart(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_chart_create(parent ? parent : lv_scr_act())) {}

    LChart& set_type(lv_chart_type_t type) {
        lv_chart_set_type(m_obj, type);
        return *this;
    }

    LChart& set_type(int type) {
        return set_type(static_cast<lv_chart_type_t>(type));
    }

    LChart& set_point_count(uint16_t cnt) {
        lv_chart_set_point_count(m_obj, cnt);
        return *this;
    }

    LChart& set_range(lv_chart_axis_t axis, int32_t min, int32_t max) {
        lv_chart_set_range(m_obj, axis, min, max);
        return *this;
    }

    LChart& set_div_line_count(uint8_t hdiv, uint8_t vdiv) {
        lv_chart_set_div_line_count(m_obj, hdiv, vdiv);
        return *this;
    }

    LChart& set_update_mode(lv_chart_update_mode_t mode) {
        lv_chart_set_update_mode(m_obj, mode);
        return *this;
    }

    LChart& set_update_mode(int mode) {
        return set_update_mode(static_cast<lv_chart_update_mode_t>(mode));
    }

    lv_chart_series_t* add_series(uint32_t color, lv_chart_axis_t axis) {
        return lv_chart_add_series(m_obj, lv_color_hex(color), axis);
    }

    LChart& set_next_value(lv_chart_series_t* ser, int32_t value) {
        lv_chart_set_next_value(m_obj, ser, value);
        return *this;
    }

    LChart& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    LChart& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
};

} // namespace easylvgl
