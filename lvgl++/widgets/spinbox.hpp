#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LSpinbox : public LWidgetCRTP<LSpinbox> {
public:
    explicit LSpinbox(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_spinbox_create(parent ? parent : lv_scr_act())) {}

    LSpinbox& set_value(int32_t val) {
        lv_spinbox_set_value(m_obj, val);
        return *this;
    }

    int32_t value() const {
        return lv_spinbox_get_value(m_obj);
    }

    LSpinbox& set_range(int32_t min, int32_t max) {
        lv_spinbox_set_range(m_obj, min, max);
        return *this;
    }

    LSpinbox& set_step(uint32_t step) {
        lv_spinbox_set_step(m_obj, step);
        return *this;
    }

    LSpinbox& set_digit_format(uint8_t digit_count, uint8_t separator_position) {
        lv_spinbox_set_digit_format(m_obj, digit_count, separator_position);
        return *this;
    }

    LSpinbox& set_rollover(bool en) {
        lv_spinbox_set_rollover(m_obj, en);
        return *this;
    }

    LSpinbox& increment() {
        lv_spinbox_increment(m_obj);
        return *this;
    }

    LSpinbox& decrement() {
        lv_spinbox_decrement(m_obj);
        return *this;
    }

    LSpinbox& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    LSpinbox& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
};

} // namespace lvgl
