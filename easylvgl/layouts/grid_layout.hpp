/**************************************************************************************************
 * @file        grid_layout.hpp
 * @brief       LGridLayout 布局器
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
#include <vector>
#include <cstdint>

namespace easylvgl {

// ============================================================================
// LGridLayout — Grid layout (Qt: QGridLayout)
// Arranges children in a configurable grid of rows and columns.
// ============================================================================
class LGridLayout : public LWidgetCRTP<LGridLayout> {
public:
    explicit LGridLayout(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_obj_create(parent ? parent : lv_scr_act())) {
    }

    LGridLayout& set_columns(const std::vector<int32_t>& cols) {
        m_col_dsc = cols;
        m_col_dsc.push_back(LV_GRID_TEMPLATE_LAST);
        apply_grid();
        return *this;
    }

    LGridLayout& set_columns(int count) {
        std::vector<int32_t> cols;
        for (int i = 0; i < count; i++) cols.push_back(LV_GRID_FR(1));
        return set_columns(cols);
    }

    LGridLayout& set_rows(const std::vector<int32_t>& rows) {
        m_row_dsc = rows;
        m_row_dsc.push_back(LV_GRID_TEMPLATE_LAST);
        apply_grid();
        return *this;
    }

    LGridLayout& set_rows(int count) {
        std::vector<int32_t> rows;
        for (int i = 0; i < count; i++) rows.push_back(LV_GRID_FR(1));
        return set_rows(rows);
    }

    LGridLayout& set_grid(const std::vector<int32_t>& cols, const std::vector<int32_t>& rows) {
        m_col_dsc = cols;
        m_col_dsc.push_back(LV_GRID_TEMPLATE_LAST);
        m_row_dsc = rows;
        m_row_dsc.push_back(LV_GRID_TEMPLATE_LAST);
        apply_grid();
        return *this;
    }

    LGridLayout& set_column_align(lv_grid_align_t align) {
        lv_obj_set_grid_align(m_obj, align, m_row_align);
        m_col_align = align;
        return *this;
    }

    LGridLayout& set_row_align(lv_grid_align_t align) {
        lv_obj_set_grid_align(m_obj, m_col_align, align);
        m_row_align = align;
        return *this;
    }

    LGridLayout& set_grid_align(lv_grid_align_t col_align, lv_grid_align_t row_align) {
        m_col_align = col_align;
        m_row_align = row_align;
        lv_obj_set_grid_align(m_obj, col_align, row_align);
        return *this;
    }

    static void place(lv_obj_t* child,
                      lv_grid_align_t col_align, int32_t col_pos, int32_t col_span,
                      lv_grid_align_t row_align, int32_t row_pos, int32_t row_span) {
        lv_obj_set_grid_cell(child,
            col_align, col_pos, col_span,
            row_align, row_pos, row_span);
    }

    static void place(lv_obj_t* child, int32_t col, int32_t row,
                      int32_t col_span = 1, int32_t row_span = 1) {
        lv_obj_set_grid_cell(child,
            LV_GRID_ALIGN_STRETCH, col, col_span,
            LV_GRID_ALIGN_STRETCH, row, row_span);
    }

    LGridLayout& set_padding(int pad) {
        lv_obj_set_style_pad_all(m_obj, pad, 0);
        return *this;
    }

    LGridLayout& set_padding(int top, int bottom, int left, int right) {
        lv_obj_set_style_pad_top(m_obj, top, 0);
        lv_obj_set_style_pad_bottom(m_obj, bottom, 0);
        lv_obj_set_style_pad_left(m_obj, left, 0);
        lv_obj_set_style_pad_right(m_obj, right, 0);
        return *this;
    }

    LGridLayout& set_row_spacing(int gap) {
        lv_obj_set_style_pad_row(m_obj, gap, 0);
        return *this;
    }

    LGridLayout& set_column_spacing(int gap) {
        lv_obj_set_style_pad_column(m_obj, gap, 0);
        return *this;
    }

    LGridLayout& set_spacing(int row_gap, int col_gap) {
        lv_obj_set_style_pad_row(m_obj, row_gap, 0);
        lv_obj_set_style_pad_column(m_obj, col_gap, 0);
        return *this;
    }

    LGridLayout& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LGridLayout& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    LGridLayout& set_width(int w) {
        lv_obj_set_width(m_obj, w);
        return *this;
    }

    LGridLayout& set_height(int h) {
        lv_obj_set_height(m_obj, h);
        return *this;
    }

private:
    std::vector<int32_t> m_col_dsc;
    std::vector<int32_t> m_row_dsc;
    lv_grid_align_t m_col_align = LV_GRID_ALIGN_START;
    lv_grid_align_t m_row_align = LV_GRID_ALIGN_START;

    void apply_grid() {
        if (!m_col_dsc.empty() && !m_row_dsc.empty()) {
            lv_obj_set_grid_dsc_array(m_obj, m_col_dsc.data(), m_row_dsc.data());
            lv_obj_set_grid_align(m_obj, m_col_align, m_row_align);
        }
    }
};

} // namespace easylvgl
