#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LChart : public LWidget {
public:
    explicit LChart(lv_obj_t* parent = nullptr)
        : LWidget(lv_chart_create(parent ? parent : lv_scr_act())) {}

    LChart& type(lv_chart_type_t type) {
        lv_chart_set_type(m_obj, type);
        return *this;
    }

    LChart& point_count(uint16_t cnt) {
        lv_chart_set_point_count(m_obj, cnt);
        return *this;
    }

    LChart& range(lv_chart_axis_t axis, int32_t min, int32_t max) {
        lv_chart_set_range(m_obj, axis, min, max);
        return *this;
    }

    lv_chart_series_t* add_series(uint32_t color, lv_chart_axis_t axis) {
        return lv_chart_add_series(m_obj, lv_color_hex(color), axis);
    }

    LChart& set_next_value(lv_chart_series_t* ser, int32_t value) {
        lv_chart_set_next_value(m_obj, ser, value);
        return *this;
    }
};

} // namespace lvgl
