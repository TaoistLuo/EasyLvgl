#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LScale : public LWidgetCRTP<LScale> {
public:
    explicit LScale(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_scale_create(parent ? parent : lv_scr_act())) {}

    LScale& set_mode(lv_scale_mode_t mode) {
        lv_scale_set_mode(m_obj, mode);
        return *this;
    }

    LScale& set_mode(int mode) {
        return set_mode(static_cast<lv_scale_mode_t>(mode));
    }

    LScale& set_range(int32_t min, int32_t max) {
        lv_scale_set_range(m_obj, min, max);
        return *this;
    }

    LScale& set_total_tick_count(uint16_t cnt) {
        lv_scale_set_total_tick_count(m_obj, cnt);
        return *this;
    }

    LScale& set_major_tick_every(uint16_t nth) {
        lv_scale_set_major_tick_every(m_obj, nth);
        return *this;
    }

    LScale& set_rotation(int32_t rotation) {
        lv_scale_set_rotation(m_obj, rotation);
        return *this;
    }

    LScale& set_angle_range(uint32_t angle_range) {
        lv_scale_set_angle_range(m_obj, angle_range);
        return *this;
    }

    LScale& set_label_show(bool show) {
        lv_scale_set_label_show(m_obj, show);
        return *this;
    }

    LScale& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LScale& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
    LScale& set_draw_ticks_on_top(bool on) { lv_scale_set_draw_ticks_on_top(m_obj, on); return *this; }

};

} // namespace lvgl
