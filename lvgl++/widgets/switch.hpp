#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/style.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

/**
 * @brief LSwitch component (toggle switch)
 * 
 * Wraps lv_switch to provide a modern toggle switch with fluent API.
 * 
 * @example
 * LSwitch sw;
 * sw.checked(true)
 *   .on_change([](auto* e) {
 *       bool state = lv_obj_has_state(lv_event_get_target(e), LV_STATE_CHECKED);
 *       printf("LSwitch: %s\n", state ? "ON" : "OFF");
 *   });
 */
class LSwitch : public LWidget {
public:
    /**
     * @brief Constructor
     * @param parent Parent object (nullptr = current screen)
     */
    explicit LSwitch(lv_obj_t* parent = nullptr)
        : LWidget(lv_switch_create(parent ? parent : lv_scr_act())) {}

    /**
     * @brief Set switch state
     * @param state true = ON, false = OFF
     * @return LSwitch& Reference for chaining
     */
    LSwitch& checked(bool state) {
        if (state) {
            lv_obj_add_state(m_obj, LV_STATE_CHECKED);
        } else {
            lv_obj_remove_state(m_obj, LV_STATE_CHECKED);
        }
        return *this;
    }

    /**
     * @brief Get switch state
     * @return bool true = ON, false = OFF
     */
    bool is_checked() const {
        return lv_obj_has_state(m_obj, LV_STATE_CHECKED);
    }

    /**
     * @brief Toggle switch state
     * @return LSwitch& Reference for chaining
     */
    LSwitch& toggle() {
        if (is_checked()) {
            lv_obj_remove_state(m_obj, LV_STATE_CHECKED);
        } else {
            lv_obj_add_state(m_obj, LV_STATE_CHECKED);
        }
        return *this;
    }

    /**
     * @brief Set position
     * @param x X coordinate
     * @param y Y coordinate
     * @return LSwitch& Reference for chaining
     */
    LSwitch& pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    /**
     * @brief Set alignment
     * @param align Alignment mode
     * @return LSwitch& Reference for chaining
     */
    LSwitch& align(lv_align_t align) {
        lv_obj_set_align(m_obj, align);
        return *this;
    }

    /**
     * @brief Register change event callback
     * @param callback Event callback function
     * @return LSwitch& Reference for chaining
     */
    template<typename Func>
    LSwitch& on_change(Func&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<Func>(callback));
        return *this;
    }

    /**
     * @brief Enable/disable the switch
     * @param enabled true = enabled, false = disabled
     * @return LSwitch& Reference for chaining
     */
    LSwitch& enabled(bool enabled) {
        if (enabled) {
            lv_obj_remove_state(m_obj, LV_STATE_DISABLED);
        } else {
            lv_obj_add_state(m_obj, LV_STATE_DISABLED);
        }
        return *this;
    }

    /**
     * @brief Set custom colors
     * @param bg_color Background color (unchecked state)
     * @param checked_color Color when checked
     * @return LSwitch& Reference for chaining
     */
    LSwitch& colors(uint32_t bg_color, uint32_t checked_color) {
        LStyle style_default;
        style_default.bg_color(bg_color).apply_to(m_obj, LV_PART_MAIN | LV_STATE_DEFAULT);
        
        LStyle style_checked;
        style_checked.bg_color(checked_color).apply_to(m_obj, LV_PART_INDICATOR | LV_STATE_CHECKED);
        
        return *this;
    }
};

} // namespace lvgl
