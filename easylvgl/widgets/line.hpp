#pragma once
#include "easylvgl/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace easylvgl {

class LLine : public LWidgetCRTP<LLine> {
public:
    explicit LLine(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_line_create(parent ? parent : lv_scr_act())) {}

    LLine& set_points(const lv_point_precise_t* points, uint16_t point_num) {
        lv_line_set_points(m_obj, points, point_num);
        return *this;
    }

    LLine& set_width(int width) {
        lv_obj_set_style_line_width(m_obj, width, 0);
        return *this;
    }

    LLine& set_color(uint32_t color) {
        lv_obj_set_style_line_color(m_obj, lv_color_hex(color), 0);
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

    /** 虚线：实线段长度（0 = 实线） */
    LLine& set_dash_width(int w) {
        lv_obj_set_style_line_dash_width(m_obj, w, 0);
        return static_cast<LLine&>(*this);
    }

    /** 虚线：间隔长度 */
    LLine& set_dash_gap(int g) {
        lv_obj_set_style_line_dash_gap(m_obj, g, 0);
        return static_cast<LLine&>(*this);
    }

    /** 线段端点是否圆角 */
    LLine& set_rounded(bool on) {
        lv_obj_set_style_line_rounded(m_obj, on, 0);
        return static_cast<LLine&>(*this);
    }
};

} // namespace easylvgl
