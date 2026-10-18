/**************************************************************************************************
 * @file        combobox.hpp
 * @brief       LCombobox 组件封装
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

class LComboBox : public LWidgetCRTP<LComboBox> {
public:
    explicit LComboBox(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_dropdown_create(parent ? parent : lv_scr_act())) {}

    LComboBox& set_options(const char* opts) {
        lv_dropdown_set_options(m_obj, opts);
        return *this;
    }

    LComboBox& add_option(const char* opt, uint32_t pos = LV_DROPDOWN_POS_LAST) {
        lv_dropdown_add_option(m_obj, opt, pos);
        return *this;
    }

    LComboBox& clear_options() {
        lv_dropdown_clear_options(m_obj);
        return *this;
    }

    LComboBox& set_selected(uint16_t idx) {
        lv_dropdown_set_selected(m_obj, idx);
        return *this;
    }

    uint16_t selected() const {
        return lv_dropdown_get_selected(m_obj);
    }

    void selected_text(char* buf, uint32_t buf_size) const {
        lv_dropdown_get_selected_str(m_obj, buf, buf_size);
    }

    uint16_t option_count() const {
        return lv_dropdown_get_option_count(m_obj);
    }

    LComboBox& set_direction(lv_dir_t dir) {
        /* 0 = LV_DIR_NONE：会让下拉列表不再对齐（停留在原点），视为默认方向（下方） */
        lv_dropdown_set_dir(m_obj, static_cast<lv_dir_t>(dir) == LV_DIR_NONE ? LV_DIR_BOTTOM : static_cast<lv_dir_t>(dir));
        return *this;
    }

    LComboBox& set_direction(int dir) {
        return set_direction(static_cast<lv_dir_t>(dir));
    }

    LComboBox& set_symbol(const char* symbol) {
        lv_dropdown_set_symbol(m_obj, symbol);
        return *this;
    }

    LComboBox& set_text(const char* txt) {
        lv_dropdown_set_text(m_obj, txt);
        return *this;
    }

    LComboBox& open(bool open = true) {
        if (open) {
            lv_dropdown_open(m_obj);
        } else {
            lv_dropdown_close(m_obj);
        }
        return *this;
    }

    void close() {
        lv_dropdown_close(m_obj);
    }

    LComboBox& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LComboBox& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    template<typename Func>
    LComboBox& on_change(Func&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<Func>(callback));
        return *this;
    }

    LComboBox& set_enabled(bool enabled) {
        if (enabled) {
            lv_obj_remove_state(m_obj, LV_STATE_DISABLED);
        } else {
            lv_obj_add_state(m_obj, LV_STATE_DISABLED);
        }
        return *this;
    }
    LComboBox& set_selected_highlight(bool on) { lv_dropdown_set_selected_highlight(m_obj, on); return *this; }

};

} // namespace easylvgl
