#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/style.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LLine : public LWidgetCRTP<LLine> {
public:
    explicit LLine(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_line_create(parent ? parent : lv_scr_act())) {}

    LLine& set_points(const lv_point_precise_t* points, uint16_t point_num) {
        lv_line_set_points(m_obj, points, point_num);
        return *this;
    }

    LLine& set_width(int width) {
        LStyle style;
        style.set_border_width(width).apply_to(m_obj);
        return *this;
    }

    LLine& set_color(uint32_t color) {
        LStyle style;
        style.set_border_color(color).apply_to(m_obj);
        return *this;
    }

    LLine& set_y_invert(bool enable) {
        lv_line_set_y_invert(m_obj, enable);
        return *this;
    }

    LLine& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
};

} // namespace lvgl
