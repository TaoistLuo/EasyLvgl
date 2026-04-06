#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/style.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LComboBox : public LWidgetCRTP<LComboBox> {
public:
    explicit LComboBox(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_dropdown_create(parent ? parent : lv_scr_act())) {}

    LComboBox& set_options(const char* opts) {
        lv_dropdown_set_options(m_obj, opts);
        return *this;
    }

    LComboBox& add_option(const char* opt, uint32_t pos = LV_DROPDOWN_POS_LAST) {
        lv_dropdown_add_option(m_obj, opt, pos);
        return *this;
    }

    LComboBox& clear_options() {
        lv_dropdown_clear_options(m_obj);
        return *this;
    }

    LComboBox& set_selected(uint16_t idx) {
        lv_dropdown_set_selected(m_obj, idx);
        return *this;
    }

    uint16_t selected() const {
        return lv_dropdown_get_selected(m_obj);
    }

    void selected_text(char* buf, uint32_t buf_size) const {
        lv_dropdown_get_selected_str(m_obj, buf, buf_size);
    }

    uint16_t option_count() const {
        return lv_dropdown_get_option_count(m_obj);
    }

    LComboBox& set_direction(lv_dir_t dir) {
        lv_dropdown_set_dir(m_obj, dir);
        return *this;
    }

    LComboBox& set_symbol(const char* symbol) {
        lv_dropdown_set_symbol(m_obj, symbol);
        return *this;
    }

    LComboBox& set_text(const char* txt) {
        lv_dropdown_set_text(m_obj, txt);
        return *this;
    }

    LComboBox& open(bool open = true) {
        if (open) {
            lv_dropdown_open(m_obj);
        } else {
            lv_dropdown_close(m_obj);
        }
        return *this;
    }

    void close() {
        lv_dropdown_close(m_obj);
    }

    LComboBox& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LComboBox& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    template<typename Func>
    LComboBox& on_change(Func&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<Func>(callback));
        return *this;
    }

    LComboBox& set_enabled(bool enabled) {
        if (enabled) {
            lv_obj_remove_state(m_obj, LV_STATE_DISABLED);
        } else {
            lv_obj_add_state(m_obj, LV_STATE_DISABLED);
        }
        return *this;
    }
};

} // namespace lvgl
