/**************************************************************************************************
 * @file        obj.hpp
 * @brief       LObj 组件封装
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

/**
 *
 * 主要用途是**组件根容器**：默认是"裸"对象——去掉主题样式（透明背景、
 * 无边框/圆角/内边距），关闭滚动，尺寸随内容自适应，因此不会给组件内部
 * 带来任何额外视觉；放到父级 flex/grid 里就是普通的一个布局项。
 */
class LObj : public LWidgetCRTP<LObj> {
public:
    explicit LObj(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_obj_create(parent ? parent : lv_scr_act())) {
        lv_obj_remove_style_all(m_obj);
        lv_obj_remove_flag(m_obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_size(m_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    }
};

} // namespace easylvgl
