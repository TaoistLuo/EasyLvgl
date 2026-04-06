#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/style.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LArc : public LWidgetCRTP<LArc> {
public:
    using LWidgetCRTP<LArc>::set_size;

    explicit LArc(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_arc_create(parent ? parent : lv_scr_act())) {}

    LArc& set_value(int16_t value) {
        lv_arc_set_value(m_obj, value);
        return *this;
    }

    int16_t value() const {
        return lv_arc_get_value(m_obj);
    }

    LArc& set_range(int16_t min, int16_t max) {
        lv_arc_set_range(m_obj, min, max);
        return *this;
    }

    LArc& set_mode(lv_arc_mode_t mode) {
        lv_arc_set_mode(m_obj, mode);
        return *this;
    }

    LArc& set_rotation(uint16_t rotation) {
        lv_arc_set_rotation(m_obj, rotation);
        return *this;
    }

    LArc& set_bg_angles(uint16_t start, uint16_t end) {
        lv_arc_set_bg_angles(m_obj, start, end);
        return *this;
    }

    LArc& set_angles(uint16_t start, uint16_t end) {
        lv_arc_set_angles(m_obj, start, end);
        return *this;
    }

    LArc& set_change_rate(uint16_t rate) {
        lv_arc_set_change_rate(m_obj, rate);
        return *this;
    }

    LArc& set_size(int size) {
        lv_obj_set_size(m_obj, size, size);
        return *this;
    }

    LArc& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    template<typename Func>
    LArc& on_change(Func&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<Func>(callback));
        return *this;
    }

    LArc& set_knob_enabled(bool enable) {
        if (!enable) {
            lv_obj_remove_flag(m_obj, LV_OBJ_FLAG_CLICKABLE);
        } else {
            lv_obj_add_flag(m_obj, LV_OBJ_FLAG_CLICKABLE);
        }
        return *this;
    }

    LArc& set_colors(uint32_t bg_color, uint32_t fg_color) {
        lv_obj_set_style_arc_color(m_obj, lv_color_hex(bg_color), LV_PART_MAIN);
        lv_obj_set_style_arc_color(m_obj, lv_color_hex(fg_color), LV_PART_INDICATOR);
        
        return *this;
    }
};

} // namespace lvgl
