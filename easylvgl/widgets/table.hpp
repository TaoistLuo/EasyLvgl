#pragma once
#include "easylvgl/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace easylvgl {

class LTable : public LWidgetCRTP<LTable> {
public:
    explicit LTable(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_table_create(parent ? parent : lv_scr_act())) {}

    LTable& set_cell_value(uint16_t row, uint16_t col, const char* txt) {
        lv_table_set_cell_value(m_obj, row, col, txt);
        return *this;
    }

    LTable& set_row_count(uint16_t cnt) {
        lv_table_set_row_count(m_obj, cnt);
        return *this;
    }

    LTable& set_column_count(uint16_t cnt) {
        lv_table_set_column_count(m_obj, cnt);
        return *this;
    }

    LTable& set_column_width(uint16_t col, int32_t w) {
        lv_table_set_column_width(m_obj, col, w);
        return *this;
    }

    LTable& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    LTable& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
};

} // namespace easylvgl
