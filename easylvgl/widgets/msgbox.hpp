/**************************************************************************************************
 * @file        msgbox.hpp
 * @brief       LMsgbox 组件封装
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

class LMsgBox : public LWidgetCRTP<LMsgBox> {
public:
    explicit LMsgBox(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_msgbox_create(parent)) {}

    LMsgBox& set_title(const char* txt) {
        lv_msgbox_add_title(m_obj, txt);
        return *this;
    }

    LMsgBox& set_text(const char* txt) {
        lv_msgbox_add_text(m_obj, txt);
        return *this;
    }

    LMsgBox& add_close_button() {
        lv_msgbox_add_close_button(m_obj);
        return *this;
    }

    lv_obj_t* add_footer_button(const char* txt) {
        return lv_msgbox_add_footer_button(m_obj, txt);
    }

    template<typename Func>
    LMsgBox& on_value_changed(Func&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<Func>(callback));
        return *this;
    }

    void close() {
        if (m_obj) {
            lv_msgbox_close(m_obj);
            m_obj = nullptr;
        }
    }

    ~LMsgBox() override {
        if (m_obj) {
            lv_msgbox_close(m_obj);
            m_obj = nullptr;
        }
    }

};

} // namespace easylvgl
