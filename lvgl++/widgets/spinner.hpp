#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/style.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

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
        LStyle style;
        style.set_bg_color(color).apply_to(m_obj, LV_PART_INDICATOR);
        return *this;
    }

    LSpinner& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
};

} // namespace lvgl
