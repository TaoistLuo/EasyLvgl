/**************************************************************************************************
 * @file        button.hpp
 * @brief       LButton 组件封装
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

/**
 * @class LButton
 * 
 * 提供链式API和预设主题样式。
 */
class LButton : public LWidgetCRTP<LButton> {
private:
    lv_obj_t* m_label;
    
public:
    explicit LButton(lv_obj_t* parent = nullptr) 
        : LWidgetCRTP(lv_button_create(parent ? parent : lv_scr_act())) {
        m_label = lv_label_create(m_obj);
        lv_obj_center(m_label);
    }
    
    LButton& set_text(const char* txt) {
        lv_label_set_text(m_label, txt);
        return *this;
    }
    
    LButton& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
    
    LButton& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
    
    LButton& set_align(lv_align_t a) {
        lv_obj_set_align(m_obj, a);
        return *this;
    }
    
    template<typename F>
    LButton& on_click(F&& callback) {
        connect_event(m_obj, LV_EVENT_CLICKED, std::forward<F>(callback));
        return *this;
    }
    
    LButton& primary() {
        lv_obj_set_style_bg_color(m_obj, lv_color_hex(0x2196F3), 0);
        lv_obj_set_style_radius(m_obj, 8, 0);
        lv_obj_set_style_pad_all(m_obj, 10, 0);
        return *this;
    }
    
    LButton& danger() {
        lv_obj_set_style_bg_color(m_obj, lv_color_hex(0xF44336), 0);
        lv_obj_set_style_radius(m_obj, 8, 0);
        lv_obj_set_style_pad_all(m_obj, 10, 0);
        return *this;
    }
    
    LButton& success() {
        lv_obj_set_style_bg_color(m_obj, lv_color_hex(0x4CAF50), 0);
        lv_obj_set_style_radius(m_obj, 8, 0);
        lv_obj_set_style_pad_all(m_obj, 10, 0);
        return *this;
    }
    
    LButton& warning() {
        lv_obj_set_style_bg_color(m_obj, lv_color_hex(0xFF9800), 0);
        lv_obj_set_style_radius(m_obj, 8, 0);
        lv_obj_set_style_pad_all(m_obj, 10, 0);
        return *this;
    }
    
    LButton& set_disabled(bool dis = true) {
        if (dis) {
            lv_obj_add_state(m_obj, LV_STATE_DISABLED);
        } else {
            lv_obj_remove_state(m_obj, LV_STATE_DISABLED);
        }
        return *this;
    }
};

} // namespace easylvgl
