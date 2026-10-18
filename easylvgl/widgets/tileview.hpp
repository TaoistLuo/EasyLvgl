/**************************************************************************************************
 * @file        tileview.hpp
 * @brief       LTileview 组件封装
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

class LTileView : public LWidgetCRTP<LTileView> {
public:
    explicit LTileView(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_tileview_create(parent ? parent : lv_scr_act())) {}

    lv_obj_t* add_tile(uint8_t col_id, uint8_t row_id, lv_dir_t dir) {
        return lv_tileview_add_tile(m_obj, col_id, row_id, dir);
    }

    LTileView& set_tile(lv_obj_t* tile_obj, lv_anim_enable_t anim = LV_ANIM_OFF) {
        lv_tileview_set_tile(m_obj, tile_obj, anim);
        return *this;
    }

    LTileView& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LTileView& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
};

class LTile : public LWidgetCRTP<LTile> {
public:
    explicit LTile(lv_obj_t* tileview, int col, int row, int dir)
        : LWidgetCRTP(lv_tileview_add_tile(tileview ? tileview : lv_scr_act(),
                                          (uint8_t)col, (uint8_t)row, (lv_dir_t)dir)) {}

    LTile& set_size(int w, int h) { lv_obj_set_size(m_obj, w, h); return *this; }
    LTile& set_pos(int x, int y) { lv_obj_set_pos(m_obj, x, y); return *this; }
};

} // namespace easylvgl
