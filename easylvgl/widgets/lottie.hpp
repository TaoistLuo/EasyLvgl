/**************************************************************************************************
 * @file        lottie.hpp
 * @brief       LLottie 组件封装
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
#include <cstdint>
#include <vector>

namespace easylvgl {

class LLottie : public LWidgetCRTP<LLottie> {
    std::vector<uint8_t> m_buf;
    int m_bufW = 0;
    int m_bufH = 0;

    /* lv_lottie 是 canvas 型控件，渲染前必须由调用方提供像素缓冲 */
    void ensure_buffer() {
        const int w = lv_obj_get_width(m_obj);
        const int h = lv_obj_get_height(m_obj);
        if (w <= 0 || h <= 0) return;
        if (w == m_bufW && h == m_bufH) return;
        m_buf.assign(static_cast<size_t>(w) * static_cast<size_t>(h) * 4u, 0);
        lv_lottie_set_buffer(m_obj, w, h, m_buf.data());
        m_bufW = w;
        m_bufH = h;
    }

public:
    using LWidgetCRTP<LLottie>::set_align;

    explicit LLottie(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_lottie_create(parent ? parent : lv_scr_act())) {}

    LLottie& set_size(int w, int h) {
        LWidgetCRTP<LLottie>::set_size(w, h);
        m_bufW = m_bufH = 0;
        ensure_buffer();
        return *this;
    }

    LLottie& set_src(const char* src) {
        ensure_buffer();
        lv_lottie_set_src_file(m_obj, src);
        return *this;
    }

    LLottie& set_src_data(const void* data, size_t size) {
        ensure_buffer();
        lv_lottie_set_src_data(m_obj, data, size);
        return *this;
    }

    template<typename Func>
    LLottie& on_complete(Func&& callback) {
        connect_event(m_obj, LV_EVENT_READY, std::forward<Func>(callback));
        return *this;
    }
};

} // namespace easylvgl
