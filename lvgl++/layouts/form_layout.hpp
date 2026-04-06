#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl/lvgl.h"
#include <vector>
#include <cstdint>

namespace lvgl {

// ============================================================================
// LFormLayout — Form layout (Qt: QFormLayout)
// Two-column grid: labels on left, inputs on right.
// ============================================================================
class LFormLayout : public LWidgetCRTP<LFormLayout> {
public:
    explicit LFormLayout(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_obj_create(parent ? parent : lv_scr_act())) {
        m_col_dsc = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        rebuild_row_dsc();
    }

    LFormLayout& set_label_width(int32_t width) {
        m_col_dsc = {width, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        lv_obj_set_grid_dsc_array(m_obj, m_col_dsc.data(), m_row_dsc.data());
        return *this;
    }

    LFormLayout& add_row(const char* label_text, lv_obj_t* input_widget) {
        int row = m_row_count;

        // Create label in column 0
        lv_obj_t* label = lv_label_create(m_obj);
        lv_label_set_text(label, label_text);
        lv_obj_set_grid_cell(label,
            LV_GRID_ALIGN_START, 0, 1,
            LV_GRID_ALIGN_CENTER, row, 1);

        // Place input widget in column 1
        if (input_widget) {
            lv_obj_set_parent(input_widget, m_obj);
            lv_obj_set_grid_cell(input_widget,
                LV_GRID_ALIGN_STRETCH, 1, 1,
                LV_GRID_ALIGN_CENTER, row, 1);
        }

        m_row_count++;
        rebuild_row_dsc();
        return *this;
    }

    LFormLayout& set_row_spacing(int gap) {
        lv_obj_set_style_pad_row(m_obj, gap, 0);
        return *this;
    }

    LFormLayout& set_column_spacing(int gap) {
        lv_obj_set_style_pad_column(m_obj, gap, 0);
        return *this;
    }

    LFormLayout& set_spacing(int row_gap, int col_gap) {
        lv_obj_set_style_pad_row(m_obj, row_gap, 0);
        lv_obj_set_style_pad_column(m_obj, col_gap, 0);
        return *this;
    }

    LFormLayout& set_padding(int pad) {
        lv_obj_set_style_pad_all(m_obj, pad, 0);
        return *this;
    }

    LFormLayout& set_padding(int top, int bottom, int left, int right) {
        lv_obj_set_style_pad_top(m_obj, top, 0);
        lv_obj_set_style_pad_bottom(m_obj, bottom, 0);
        lv_obj_set_style_pad_left(m_obj, left, 0);
        lv_obj_set_style_pad_right(m_obj, right, 0);
        return *this;
    }

    LFormLayout& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LFormLayout& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    int row_count() const { return m_row_count; }

private:
    std::vector<int32_t> m_col_dsc;
    std::vector<int32_t> m_row_dsc;
    int m_row_count = 0;

    void rebuild_row_dsc() {
        m_row_dsc.clear();
        for (int i = 0; i < m_row_count; i++) {
            m_row_dsc.push_back(LV_GRID_CONTENT);
        }
        m_row_dsc.push_back(LV_GRID_TEMPLATE_LAST);
        if (!m_col_dsc.empty()) {
            lv_obj_set_grid_dsc_array(m_obj, m_col_dsc.data(), m_row_dsc.data());
            lv_obj_set_grid_align(m_obj, LV_GRID_ALIGN_START, LV_GRID_ALIGN_CENTER);
        }
    }
};

} // namespace lvgl
