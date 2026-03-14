#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/style.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

/**
 * @brief LED component (indicator light)
 * 
 * Wraps lv_led to provide a simple LED indicator.
 * 
 * @example
 * LLed led;
 * led.color(0x00FF00)
 *    .brightness(255)
 *    .on();
 */
class LLed : public LWidget {
public:
    /**
     * @brief Constructor
     * @param parent Parent object (nullptr = current screen)
     */
    explicit LLed(lv_obj_t* parent = nullptr)
        : LWidget(lv_led_create(parent ? parent : lv_scr_act())) {}

    /**
     * @brief Turn LED on
     * @return LLed& Reference for chaining
     */
    LLed& on() {
        lv_led_on(m_obj);
        return *this;
    }

    /**
     * @brief Turn LED off
     * @return LLed& Reference for chaining
     */
    LLed& off() {
        lv_led_off(m_obj);
        return *this;
    }

    /**
     * @brief Toggle LED state
     * @return LLed& Reference for chaining
     */
    LLed& toggle() {
        lv_led_toggle(m_obj);
        return *this;
    }

    /**
     * @brief Set LED brightness
     * @param bright Brightness (0-255)
     * @return LLed& Reference for chaining
     */
    LLed& brightness(uint8_t bright) {
        lv_led_set_brightness(m_obj, bright);
        return *this;
    }

    /**
     * @brief Set LED color
     * @param color Color (hex value)
     * @return LLed& Reference for chaining
     */
    LLed& color(uint32_t color) {
        lv_led_set_color(m_obj, lv_color_hex(color));
        return *this;
    }

    /**
     * @brief Set size
     * @param size Size (width and height, LED is square)
     * @return LLed& Reference for chaining
     */
    LLed& size(int size) {
        lv_obj_set_size(m_obj, size, size);
        return *this;
    }

    /**
     * @brief Set position
     * @param x X coordinate
     * @param y Y coordinate
     * @return LLed& Reference for chaining
     */
    LLed& pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    /**
     * @brief Set alignment
     * @param align Alignment mode
     * @return LLed& Reference for chaining
     */
    LLed& align(lv_align_t align) {
        lv_obj_set_align(m_obj, align);
        return *this;
    }
};

} // namespace lvgl
