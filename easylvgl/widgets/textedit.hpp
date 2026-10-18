/**************************************************************************************************
 * @file        textedit.hpp
 * @brief       LTextedit 组件封装
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

class LTextEdit : public LWidgetCRTP<LTextEdit> {
public:
    explicit LTextEdit(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_textarea_create(parent ? parent : lv_scr_act())) {
    }

    LTextEdit& set_text(const char* txt) {
        lv_textarea_set_text(m_obj, txt);
        return *this;
    }

    const char* text() const {
        return lv_textarea_get_text(m_obj);
    }

    LTextEdit& set_placeholder(const char* txt) {
        lv_textarea_set_placeholder_text(m_obj, txt);
        return *this;
    }

    LTextEdit& set_max_length(uint32_t len) {
        lv_textarea_set_max_length(m_obj, len);
        return *this;
    }

    LTextEdit& set_accepted_chars(const char* chars) {
        lv_textarea_set_accepted_chars(m_obj, chars);
        return *this;
    }

    LTextEdit& add_text(const char* txt) {
        lv_textarea_add_text(m_obj, txt);
        return *this;
    }

    LTextEdit& clear() {
        lv_textarea_set_text(m_obj, "");
        return *this;
    }

    LTextEdit& set_cursor_pos(uint32_t pos) {
        lv_textarea_set_cursor_pos(m_obj, pos);
        return *this;
    }

    LTextEdit& set_cursor_click_pos(bool enable) {
        lv_textarea_set_cursor_click_pos(m_obj, enable);
        return *this;
    }

    LTextEdit& set_text_align(lv_text_align_t align) {
        lv_obj_set_style_text_align(m_obj, align, 0);
        return *this;
    }

    LTextEdit& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LTextEdit& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    template<typename Func>
    LTextEdit& on_change(Func&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<Func>(callback));
        return *this;
    }

    template<typename Func>
    LTextEdit& on_ready(Func&& callback) {
        connect_event(m_obj, LV_EVENT_READY, std::forward<Func>(callback));
        return *this;
    }

    LTextEdit& set_enabled(bool enabled) {
        if (enabled) {
            lv_obj_remove_state(m_obj, LV_STATE_DISABLED);
        } else {
            lv_obj_add_state(m_obj, LV_STATE_DISABLED);
        }
        return *this;
    }
    LTextEdit& set_password_mode(bool on) { lv_textarea_set_password_mode(m_obj, on); return *this; }

    LTextEdit& set_password_show_time(int ms) { lv_textarea_set_password_show_time(m_obj, ms); return *this; }

    LTextEdit& set_text_selection(bool on) { lv_textarea_set_text_selection(m_obj, on); return *this; }

};

} // namespace easylvgl
