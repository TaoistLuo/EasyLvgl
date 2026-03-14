#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LScale : public LWidget {
public:
    explicit LScale(lv_obj_t* parent = nullptr)
        : LWidget(lv_scale_create(parent ? parent : lv_scr_act())) {}

    LScale& mode(lv_scale_mode_t mode) {
        lv_scale_set_mode(m_obj, mode);
        return *this;
    }

    LScale& range(int32_t min, int32_t max) {
        lv_scale_set_range(m_obj, min, max);
        return *this;
    }

    LScale& total_tick_count(uint16_t cnt) {
        lv_scale_set_total_tick_count(m_obj, cnt);
        return *this;
    }

    LScale& major_tick_every(uint16_t nth) {
        lv_scale_set_major_tick_every(m_obj, nth);
        return *this;
    }
};

} // namespace lvgl
