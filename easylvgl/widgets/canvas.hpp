/**************************************************************************************************
 * @file        canvas.hpp
 * @brief       LCanvas 组件封装
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

class LCanvas : public LWidgetCRTP<LCanvas> {
public:
    explicit LCanvas(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_canvas_create(parent ? parent : lv_scr_act())) {}

    LCanvas& set_buffer(void* buf, int32_t w, int32_t h, lv_color_format_t cf) {
        lv_canvas_set_buffer(m_obj, buf, w, h, cf);
        return *this;
    }

    LCanvas& set_px(int32_t x, int32_t y, uint32_t color, lv_opa_t opa = LV_OPA_COVER) {
        lv_canvas_set_px(m_obj, x, y, lv_color_hex(color), opa);
        return *this;
    }

    LCanvas& fill_bg(uint32_t color, lv_opa_t opa = LV_OPA_COVER) {
        lv_canvas_fill_bg(m_obj, lv_color_hex(color), opa);
        return *this;
    }
};

} // namespace easylvgl
