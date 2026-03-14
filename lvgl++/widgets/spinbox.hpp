#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LSpinbox : public LWidget {
public:
    explicit LSpinbox(lv_obj_t* parent = nullptr)
        : LWidget(lv_spinbox_create(parent ? parent : lv_scr_act())) {}

    LSpinbox& value(int32_t val) {
        lv_spinbox_set_value(m_obj, val);
        return *this;
    }

    int32_t value() const {
        return lv_spinbox_get_value(m_obj);
    }

    LSpinbox& range(int32_t min, int32_t max) {
        lv_spinbox_set_range(m_obj, min, max);
        return *this;
    }

    LSpinbox& step(uint32_t step) {
        lv_spinbox_set_step(m_obj, step);
        return *this;
    }

    LSpinbox& digit_format(uint8_t digit_count, uint8_t separator_position) {
        lv_spinbox_set_digit_format(m_obj, digit_count, separator_position);
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
};

} // namespace lvgl
