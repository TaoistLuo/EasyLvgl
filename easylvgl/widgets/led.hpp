#pragma once
#include "easylvgl/core/widget.hpp"
#include "easylvgl/core/style.hpp"
#include "easylvgl/core/event.hpp"
#include "lvgl/lvgl.h"

namespace easylvgl {

class LLed : public LWidgetCRTP<LLed> {
public:
    using LWidgetCRTP<LLed>::set_size;

    explicit LLed(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_led_create(parent ? parent : lv_scr_act())) {}

    LLed& on() {
        lv_led_on(m_obj);
        return *this;
    }

    LLed& off() {
        lv_led_off(m_obj);
        return *this;
    }

    LLed& toggle() {
        lv_led_toggle(m_obj);
        return *this;
    }

    LLed& set_brightness(uint8_t bright) {
        lv_led_set_brightness(m_obj, bright);
        return *this;
    }

    LLed& set_color(uint32_t color) {
        lv_led_set_color(m_obj, lv_color_hex(color));
        return *this;
    }

    LLed& set_size(int size) {
        lv_obj_set_size(m_obj, size, size);
        return *this;
    }

    LLed& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
};

} // namespace easylvgl
