/**************************************************************************************************
 * @file        groupbox.hpp
 * @brief       LGroupbox 组件封装
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
#include "lvgl/lvgl.h"

namespace easylvgl {

class LGroupBox : public LWidgetCRTP<LGroupBox> {
public:
    explicit LGroupBox(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_obj_create(parent ? parent : lv_scr_act())) {
        lv_obj_set_style_border_width(m_obj, 1, 0);
        lv_obj_set_style_border_opa(m_obj, LV_OPA_50, 0);
        lv_obj_set_style_border_color(m_obj, lv_palette_main(LV_PALETTE_GREY), 0);
        lv_obj_set_style_pad_all(m_obj, 10, 0);
        lv_obj_set_style_radius(m_obj, 6, 0);
        lv_obj_clear_flag(m_obj, LV_OBJ_FLAG_SCROLLABLE);
    }

    LGroupBox& set_title(const char* title) {
        m_title = title;
        lv_obj_set_style_bg_opa(m_obj, LV_OPA_COVER, 0);
        // Use a label at top as group title (Qt style)
        lv_obj_t* label = lv_obj_get_child(m_obj, 0);
        if (!label || lv_obj_get_child_count(m_obj) == 0 ||
            !is_title_label(label)) {
            label = lv_label_create(m_obj);
            mark_title_label(label);
        }
        lv_label_set_text(label, title);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, -14);
        lv_obj_set_pos(label, 8, -14);
        return *this;
    }

    const char* title() const { return m_title.c_str(); }

    LGroupBox& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LGroupBox& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    LGroupBox& set_padding(int pad) {
        lv_obj_set_style_pad_all(m_obj, pad, 0);
        return *this;
    }

    bool is_container() const { return true; }

private:
    std::string m_title;

    static void mark_title_label(lv_obj_t* label) {
        lv_obj_set_user_data(label, (void*)0x4742544C);
    }

    static bool is_title_label(lv_obj_t* label) {
        return lv_obj_get_user_data(label) == (void*)0x4742544C;
    }
};

} // namespace easylvgl
