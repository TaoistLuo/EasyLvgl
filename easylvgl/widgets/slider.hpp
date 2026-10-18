/**************************************************************************************************
 * @file        slider.hpp
 * @brief       LSlider 组件封装
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

class LSlider : public LWidgetCRTP<LSlider> {
public:
    explicit LSlider(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_slider_create(parent ? parent : lv_scr_act())) {}

    LSlider& set_range(int min, int max) {
        lv_slider_set_range(m_obj, min, max);
        return *this;
    }
    
    LSlider& set_value(int val, lv_anim_enable_t anim = LV_ANIM_OFF) {
        lv_slider_set_value(m_obj, val, anim);
        return *this;
    }

    LSlider& set_left_value(int val, lv_anim_enable_t anim = LV_ANIM_OFF) {
        lv_slider_set_left_value(m_obj, val, anim);
        return *this;
    }

    LSlider& set_mode(lv_slider_mode_t mode) {
        lv_slider_set_mode(m_obj, mode);
        return *this;
    }

    LSlider& set_mode(int mode) {
        return set_mode(static_cast<lv_slider_mode_t>(mode));
    }
    
    int value() const {
        return lv_slider_get_value(m_obj);
    }
    
    LSlider& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LSlider& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
    
    template<typename F>
    LSlider& on_change(F&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<F>(callback));
        return *this;
    }
};

} // namespace easylvgl
