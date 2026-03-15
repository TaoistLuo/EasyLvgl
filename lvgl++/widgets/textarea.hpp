#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/style.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LTextArea : public LWidgetCRTP<LTextArea> {
public:
    explicit LTextArea(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_textarea_create(parent ? parent : lv_scr_act())) {}

    LTextArea& set_text(const char* txt) {
        lv_textarea_set_text(m_obj, txt);
        return *this;
    }

    const char* text() const {
        return lv_textarea_get_text(m_obj);
    }

    LTextArea& set_placeholder(const char* txt) {
        lv_textarea_set_placeholder_text(m_obj, txt);
        return *this;
    }

    LTextArea& set_one_line(bool enable) {
        lv_textarea_set_one_line(m_obj, enable);
        return *this;
    }

    LTextArea& set_password_mode(bool enable) {
        lv_textarea_set_password_mode(m_obj, enable);
        return *this;
    }

    LTextArea& set_max_length(uint32_t len) {
        lv_textarea_set_max_length(m_obj, len);
        return *this;
    }

    LTextArea& set_accepted_chars(const char* chars) {
        lv_textarea_set_accepted_chars(m_obj, chars);
        return *this;
    }

    LTextArea& add_text(const char* txt) {
        lv_textarea_add_text(m_obj, txt);
        return *this;
    }

    LTextArea& clear() {
        lv_textarea_set_text(m_obj, "");
        return *this;
    }

    LTextArea& set_cursor_pos(uint32_t pos) {
        lv_textarea_set_cursor_pos(m_obj, pos);
        return *this;
    }

    LTextArea& set_cursor_click_pos(bool enable) {
        lv_textarea_set_cursor_click_pos(m_obj, enable);
        return *this;
    }

    LTextArea& set_text_align(lv_text_align_t align) {
        lv_obj_set_style_text_align(m_obj, align, 0);
        return *this;
    }

    LTextArea& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LTextArea& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    template<typename Func>
    LTextArea& on_change(Func&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<Func>(callback));
        return *this;
    }

    template<typename Func>
    LTextArea& on_value_changed(Func&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<Func>(callback));
        return *this;
    }

    template<typename Func>
    LTextArea& on_ready(Func&& callback) {
        connect_event(m_obj, LV_EVENT_READY, std::forward<Func>(callback));
        return *this;
    }

    LTextArea& set_enabled(bool enabled) {
        if (enabled) {
            lv_obj_remove_state(m_obj, LV_STATE_DISABLED);
        } else {
            lv_obj_add_state(m_obj, LV_STATE_DISABLED);
        }
        return *this;
    }
};

} // namespace lvgl
