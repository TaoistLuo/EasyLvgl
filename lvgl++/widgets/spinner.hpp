#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/style.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LSpinner : public LWidget {
public:
    explicit LSpinner(lv_obj_t* parent = nullptr)
        : LWidget(lv_spinner_create(parent ? parent : lv_scr_act())) {}

    LSpinner& size(int size) {
        lv_obj_set_size(m_obj, size, size);
        return *this;
    }

    LSpinner& pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    LSpinner& align(lv_align_t align) {
        lv_obj_set_align(m_obj, align);
        return *this;
    }

    LSpinner& color(uint32_t color) {
        LStyle style;
        style.bg_color(color).apply_to(m_obj, LV_PART_INDICATOR);
        return *this;
    }
};

} // namespace lvgl
