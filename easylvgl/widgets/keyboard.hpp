/**************************************************************************************************
 * @file        keyboard.hpp
 * @brief       LKeyboard 组件封装
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

class LKeyboard : public LWidgetCRTP<LKeyboard> {
public:
    explicit LKeyboard(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_keyboard_create(parent ? parent : lv_scr_act())) {}

    LKeyboard& set_textarea(lv_obj_t* ta) {
        lv_keyboard_set_textarea(m_obj, ta);
        return *this;
    }

    LKeyboard& set_mode(lv_keyboard_mode_t mode) {
        lv_keyboard_set_mode(m_obj, mode);
        return *this;
    }

    LKeyboard& set_mode(int mode) {
        return set_mode(static_cast<lv_keyboard_mode_t>(mode));
    }

    LKeyboard& set_popovers(bool en) {
        lv_keyboard_set_popovers(m_obj, en);
        return *this;
    }

    LKeyboard& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LKeyboard& set_pos(int x, int y) {
        /* lv_keyboard_create 默认 LV_ALIGN_BOTTOM_MID，而对齐优先于坐标：
         * 不清掉的话 set_pos 会被忽略，键盘会被顶到父对象底部/画布外。 */
        lv_obj_set_align(m_obj, LV_ALIGN_DEFAULT);
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
};

} // namespace easylvgl
