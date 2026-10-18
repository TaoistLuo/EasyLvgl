/**************************************************************************************************
 * @file        imagebutton.hpp
 * @brief       LImagebutton 组件封装
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

class LImageButton : public LWidgetCRTP<LImageButton> {
public:
    explicit LImageButton(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_imagebutton_create(parent ? parent : lv_scr_act())) {}

    LImageButton& set_src(lv_imagebutton_state_t state, const void* src) {
        lv_imagebutton_set_src(m_obj, state, nullptr, src, nullptr);
        return *this;
    }

    LImageButton& set_state(lv_imagebutton_state_t state) {
        lv_imagebutton_set_state(m_obj, state);
        return *this;
    }

    LImageButton& set_state(int state) {
        return set_state(static_cast<lv_imagebutton_state_t>(state));
    }

    LImageButton& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    template<typename Func>
    LImageButton& on_click(Func&& callback) {
        connect_event(m_obj, LV_EVENT_CLICKED, std::forward<Func>(callback));
        return *this;
    }
};

} // namespace easylvgl
