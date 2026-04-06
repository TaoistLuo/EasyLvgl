#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl/lvgl.h"
#include <vector>
#include <cstdint>

namespace lvgl {

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

    LGridLayout& set_rows(const std::vector<int32_t>& rows) {
        m_row_dsc = rows;
        m_row_dsc.push_back(LV_GRID_TEMPLATE_LAST);
        apply_grid();
        return *this;
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

} // namespace lvgl
