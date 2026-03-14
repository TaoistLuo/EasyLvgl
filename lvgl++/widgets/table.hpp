#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LTable : public LWidget {
public:
    explicit LTable(lv_obj_t* parent = nullptr)
        : LWidget(lv_table_create(parent ? parent : lv_scr_act())) {}

    LTable& cell_value(uint16_t row, uint16_t col, const char* txt) {
        lv_table_set_cell_value(m_obj, row, col, txt);
        return *this;
    }

    LTable& row_count(uint16_t cnt) {
        lv_table_set_row_count(m_obj, cnt);
        return *this;
    }

    LTable& column_count(uint16_t cnt) {
        lv_table_set_column_count(m_obj, cnt);
        return *this;
    }

    LTable& column_width(uint16_t col, int32_t w) {
        lv_table_set_column_width(m_obj, col, w);
        return *this;
    }
};

} // namespace lvgl
