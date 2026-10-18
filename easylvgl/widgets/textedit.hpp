#pragma once
#include "easylvgl/core/widget.hpp"
#include "easylvgl/core/style.hpp"
#include "easylvgl/core/event.hpp"
#include "lvgl/lvgl.h"

namespace easylvgl {

class LTextEdit : public LWidgetCRTP<LTextEdit> {
public:
    explicit LTextEdit(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_textarea_create(parent ? parent : lv_scr_act())) {
    }

    LTextEdit& set_text(const char* txt) {
        lv_textarea_set_text(m_obj, txt);
        return *this;
    }

    const char* text() const {
        return lv_textarea_get_text(m_obj);
    }

    LTextEdit& set_placeholder(const char* txt) {
        lv_textarea_set_placeholder_text(m_obj, txt);
        return *this;
    }

    LTextEdit& set_max_length(uint32_t len) {
        lv_textarea_set_max_length(m_obj, len);
        return *this;
    }

    LTextEdit& set_accepted_chars(const char* chars) {
        lv_textarea_set_accepted_chars(m_obj, chars);
        return *this;
    }

    LTextEdit& add_text(const char* txt) {
        lv_textarea_add_text(m_obj, txt);
        return *this;
    }

    LTextEdit& clear() {
        lv_textarea_set_text(m_obj, "");
        return *this;
    }

    LTextEdit& set_cursor_pos(uint32_t pos) {
        lv_textarea_set_cursor_pos(m_obj, pos);
        return *this;
    }

    LTextEdit& set_cursor_click_pos(bool enable) {
        lv_textarea_set_cursor_click_pos(m_obj, enable);
        return *this;
    }

    LTextEdit& set_text_align(lv_text_align_t align) {
        lv_obj_set_style_text_align(m_obj, align, 0);
        return *this;
    }

    LTextEdit& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LTextEdit& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    template<typename Func>
    LTextEdit& on_change(Func&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<Func>(callback));
        return *this;
    }

    template<typename Func>
    LTextEdit& on_ready(Func&& callback) {
        connect_event(m_obj, LV_EVENT_READY, std::forward<Func>(callback));
        return *this;
    }

    LTextEdit& set_enabled(bool enabled) {
        if (enabled) {
            lv_obj_remove_state(m_obj, LV_STATE_DISABLED);
        } else {
            lv_obj_add_state(m_obj, LV_STATE_DISABLED);
        }
        return *this;
    }
    LTextEdit& set_password_mode(bool on) { lv_textarea_set_password_mode(m_obj, on); return *this; }

    LTextEdit& set_password_show_time(int ms) { lv_textarea_set_password_show_time(m_obj, ms); return *this; }

    LTextEdit& set_text_selection(bool on) { lv_textarea_set_text_selection(m_obj, on); return *this; }

};

} // namespace easylvgl
