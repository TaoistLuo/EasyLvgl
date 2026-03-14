#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/style.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LLine : public LWidget {
public:
    explicit LLine(lv_obj_t* parent = nullptr)
        : LWidget(lv_line_create(parent ? parent : lv_scr_act())) {}

    LLine& points(const lv_point_precise_t* points, uint16_t point_num) {
        lv_line_set_points(m_obj, points, point_num);
        return *this;
    }

    LLine& width(int width) {
        LStyle style;
        style.border_width(width).apply_to(m_obj);
        return *this;
    }

    LLine& color(uint32_t color) {
        LStyle style;
        style.border_color(color).apply_to(m_obj);
        return *this;
    }

    LLine& y_invert(bool enable) {
        lv_line_set_y_invert(m_obj, enable);
        return *this;
    }

    LLine& pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    LLine& align(lv_align_t align) {
        lv_obj_set_align(m_obj, align);
        return *this;
    }
};

} // namespace lvgl
