#pragma once
#include "easylvgl/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace easylvgl {

class LSpinner : public LWidgetCRTP<LSpinner> {
public:
    using LWidgetCRTP<LSpinner>::set_size;

    explicit LSpinner(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_spinner_create(parent ? parent : lv_scr_act())) {}

    LSpinner& set_size(int size) {
        lv_obj_set_size(m_obj, size, size);
        return *this;
    }

    LSpinner& set_color(uint32_t color) {
        lv_obj_set_style_arc_color(m_obj, lv_color_hex(color), LV_PART_INDICATOR);
        return *this;
    }

    LSpinner& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
};

} // namespace easylvgl
