/**************************************************************************************************
 * @file        dialog.hpp
 * @brief       dialog
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
#include "lvgl/lvgl.h"
#include <functional>
#include <utility>

#include "easylvgl/core/event.hpp"

namespace easylvgl {

class LDialog {
private:
    lv_obj_t* msgbox_;

    explicit LDialog(lv_obj_t* obj) : msgbox_(obj) {}

public:
    LDialog() : msgbox_(nullptr) {}

    ~LDialog() {
        if (msgbox_) {
            lv_msgbox_close(msgbox_);
            msgbox_ = nullptr;
        }
    }

    LDialog(LDialog&& other) noexcept
        : msgbox_(other.msgbox_) {
        other.msgbox_ = nullptr;
    }

    LDialog& operator=(LDialog&& other) noexcept {
        if (this != &other) {
            if (msgbox_) {
                lv_msgbox_close(msgbox_);
            }
            msgbox_ = other.msgbox_;
            other.msgbox_ = nullptr;
        }
        return *this;
    }

    LDialog(const LDialog&) = delete;
    LDialog& operator=(const LDialog&) = delete;

    static LDialog create(const char* title, const char* msg) {
        LDialog d(lv_msgbox_create(nullptr));
        lv_msgbox_add_title(d.msgbox_, title);
        lv_msgbox_add_text(d.msgbox_, msg);
        return d;
    }

    static LDialog info(const char* title, const char* msg) {
        return create(title, msg);
    }

    static LDialog confirm(const char* title, const char* msg) {
        return create(title, msg);
    }

    LDialog& add_button(const char* text, std::function<void()> callback = nullptr) {
        if (msgbox_) {
            lv_obj_t* btn = lv_msgbox_add_footer_button(msgbox_, text);
            if (callback && btn) {
                on_clicked(btn, std::move(callback));
            }
        }
        return *this;
    }

    void show() {
        if (msgbox_) {
            lv_obj_move_foreground(msgbox_);
        }
    }

    void close() {
        if (msgbox_) {
            lv_msgbox_close(msgbox_);
            msgbox_ = nullptr;
        }
    }

    lv_obj_t* native() { return msgbox_; }
};

} // namespace easylvgl
