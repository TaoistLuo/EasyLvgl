/**************************************************************************************************
 * @file        image.hpp
 * @brief       LImage 组件封装
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

class LImage : public LWidgetCRTP<LImage> {
public:
    explicit LImage(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_image_create(parent ? parent : lv_scr_act())) {}

    LImage& set_src(const void* src) {
        lv_image_set_src(m_obj, src);
        return *this;
    }

    LImage& set_src(const char* path) {
        lv_image_set_src(m_obj, path);
        return *this;
    }

    LImage& set_offset(int x, int y) {
        lv_image_set_offset_x(m_obj, x);
        lv_image_set_offset_y(m_obj, y);
        return *this;
    }

    LImage& set_rotation(int16_t angle) {
        lv_image_set_rotation(m_obj, angle);
        return *this;
    }

    LImage& set_pivot(int x, int y) {
        lv_image_set_pivot(m_obj, x, y);
        return *this;
    }

    LImage& set_scale(uint16_t zoom) {
        lv_image_set_scale(m_obj, zoom);
        return *this;
    }

    LImage& set_scale(uint16_t zoom_x, uint16_t zoom_y) {
        lv_image_set_scale_x(m_obj, zoom_x);
        lv_image_set_scale_y(m_obj, zoom_y);
        return *this;
    }

LImage& set_antialias(bool enable) {
    lv_image_set_antialias(m_obj, enable);
    return *this;
}

    LImage& set_opacity(uint8_t opa) {
        lv_obj_set_style_image_opa(m_obj, opa, 0);
        return *this;
    }

    LImage& set_recolor(uint32_t color, uint8_t intensity) {
        lv_obj_set_style_image_recolor(m_obj, lv_color_hex(color), 0);
        lv_obj_set_style_image_recolor_opa(m_obj, intensity, 0);
        return *this;
    }

    LImage& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LImage& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    LImage& set_align(lv_align_t align) {
        lv_obj_set_align(m_obj, align);
        return *this;
    }

    template<typename Func>
    LImage& on_click(Func&& callback) {
        lv_obj_add_flag(m_obj, LV_OBJ_FLAG_CLICKABLE);
        connect_event(m_obj, LV_EVENT_CLICKED, std::forward<Func>(callback));
        return *this;
    }
    LImage& set_scale_y(int zoom_y) { lv_image_set_scale_y(m_obj, zoom_y); return *this; }

    LImage& set_scale_x(int zoom_x) { lv_image_set_scale_x(m_obj, zoom_x); return *this; }


    /** 图片在控件内的对齐方式（不缩放时使用） */
    LImage& set_inner_align(int align) {
        lv_image_set_inner_align(m_obj, static_cast<lv_image_align_t>(align));
        return static_cast<LImage&>(*this);
    }
};

} // namespace easylvgl
