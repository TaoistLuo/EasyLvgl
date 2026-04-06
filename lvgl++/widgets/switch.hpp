#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/style.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

/**
 * @brief LSwitch component (toggle switch)
 */
class LSwitch : public LWidgetCRTP<LSwitch> {
public:
    explicit LSwitch(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_switch_create(parent ? parent : lv_scr_act())) {}

    LSwitch& set_checked(bool state) {
        if (state) {
            lv_obj_add_state(m_obj, LV_STATE_CHECKED);
        } else {
            lv_obj_remove_state(m_obj, LV_STATE_CHECKED);
        }
        return *this;
    }

    bool is_checked() const {
        return lv_obj_has_state(m_obj, LV_STATE_CHECKED);
    }

    LSwitch& toggle() {
        if (is_checked()) {
            lv_obj_remove_state(m_obj, LV_STATE_CHECKED);
        } else {
            lv_obj_add_state(m_obj, LV_STATE_CHECKED);
        }
        return *this;
    }

    LSwitch& on() {
        return set_checked(true);
    }

    LSwitch& off() {
        return set_checked(false);
    }

    LSwitch& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    LSwitch& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LSwitch& set_align(lv_align_t align) {
        lv_obj_set_align(m_obj, align);
        return *this;
    }

    template<typename Func>
    LSwitch& on_change(Func&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<Func>(callback));
        return *this;
    }

    LSwitch& set_enabled(bool enabled) {
        if (enabled) {
            lv_obj_remove_state(m_obj, LV_STATE_DISABLED);
        } else {
            lv_obj_add_state(m_obj, LV_STATE_DISABLED);
        }
        return *this;
    }

    LSwitch& set_colors(uint32_t bg_color, uint32_t checked_color) {
        lv_obj_set_style_bg_color(m_obj, lv_color_hex(bg_color), LV_PART_MAIN);
        lv_obj_set_style_bg_color(m_obj, lv_color_hex(checked_color), LV_PART_INDICATOR);
        return *this;
    }
};

} // namespace lvgl
