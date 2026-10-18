/**************************************************************************************************
 * @file        span.hpp
 * @brief       LSpan 组件封装
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

class LSpan : public LWidgetCRTP<LSpan> {
public:
    using LWidgetCRTP<LSpan>::set_align;

    explicit LSpan(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_spangroup_create(parent ? parent : lv_scr_act())) {}

    lv_span_t* new_span() {
        return lv_spangroup_new_span(m_obj);
    }

    LSpan& set_align(lv_text_align_t align) {
        lv_spangroup_set_align(m_obj, align);
        return *this;
    }

    LSpan& set_overflow(lv_span_overflow_t overflow) {
        lv_spangroup_set_overflow(m_obj, overflow);
        return *this;
    }

    LSpan& set_indent(int32_t indent) {
        lv_spangroup_set_indent(m_obj, indent);
        return *this;
    }

    LSpan& set_mode(lv_span_mode_t mode) {
        lv_spangroup_set_mode(m_obj, mode);
        return *this;
    }

    LSpan& set_mode(int mode) {
        return set_mode(static_cast<lv_span_mode_t>(mode));
    }
    LSpan& set_overflow(int overflow) { lv_spangroup_set_overflow(m_obj, static_cast<lv_span_overflow_t>(overflow)); return *this; }

};

} // namespace easylvgl
