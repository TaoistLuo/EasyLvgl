/**************************************************************************************************
 * @file        line.hpp
 * @brief       LLine 组件封装
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

class LLine : public LWidgetCRTP<LLine> {
public:
    explicit LLine(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_line_create(parent ? parent : lv_scr_act())) {}

    LLine& set_points(const lv_point_precise_t* points, uint16_t point_num) {
        lv_line_set_points(m_obj, points, point_num);
        return *this;
    }

    LLine& set_width(int width) {
        lv_obj_set_style_line_width(m_obj, width, 0);
        return *this;
    }

    LLine& set_color(uint32_t color) {
        lv_obj_set_style_line_color(m_obj, lv_color_hex(color), 0);
        return *this;
    }

    LLine& set_y_invert(bool enable) {
        lv_line_set_y_invert(m_obj, enable);
        return *this;
    }

    LLine& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    /** 虚线：实线段长度（0 = 实线） */
    LLine& set_dash_width(int w) {
        lv_obj_set_style_line_dash_width(m_obj, w, 0);
        return static_cast<LLine&>(*this);
    }

    /** 虚线：间隔长度 */
    LLine& set_dash_gap(int g) {
        lv_obj_set_style_line_dash_gap(m_obj, g, 0);
        return static_cast<LLine&>(*this);
    }

    /** 线段端点是否圆角 */
    LLine& set_rounded(bool on) {
        lv_obj_set_style_line_rounded(m_obj, on, 0);
        return static_cast<LLine&>(*this);
    }
};

} // namespace easylvgl
