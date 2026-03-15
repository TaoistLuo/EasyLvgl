#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LChart : public LWidgetCRTP<LChart> {
public:
    explicit LChart(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_chart_create(parent ? parent : lv_scr_act())) {}

    LChart& set_type(lv_chart_type_t type) {
        lv_chart_set_type(m_obj, type);
        return *this;
    }

    LChart& set_point_count(uint16_t cnt) {
        lv_chart_set_point_count(m_obj, cnt);
        return *this;
    }

    LChart& set_range(lv_chart_axis_t axis, int32_t min, int32_t max) {
        lv_chart_set_range(m_obj, axis, min, max);
        return *this;
    }

    LChart& set_div_line_count(uint8_t hdiv, uint8_t vdiv) {
        lv_chart_set_div_line_count(m_obj, hdiv, vdiv);
        return *this;
    }

    LChart& set_update_mode(lv_chart_update_mode_t mode) {
        lv_chart_set_update_mode(m_obj, mode);
        return *this;
    }

    lv_chart_series_t* add_series(uint32_t color, lv_chart_axis_t axis) {
        return lv_chart_add_series(m_obj, lv_color_hex(color), axis);
    }

    LChart& set_next_value(lv_chart_series_t* ser, int32_t value) {
        lv_chart_set_next_value(m_obj, ser, value);
        return *this;
    }

    LChart& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    LChart& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
};

} // namespace lvgl
