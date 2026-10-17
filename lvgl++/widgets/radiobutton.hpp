#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LRadioButton : public LWidgetCRTP<LRadioButton> {
public:
    explicit LRadioButton(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_obj_create(parent ? parent : lv_scr_act())) {
        lv_obj_remove_style_all(m_obj);
        lv_obj_set_size(m_obj, 140, 30);
        lv_obj_add_flag(m_obj, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(m_obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_bg_opa(m_obj, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_border_width(m_obj, 0, LV_PART_MAIN);
        lv_obj_set_style_pad_all(m_obj, 0, LV_PART_MAIN);

        m_ring = lv_obj_create(m_obj);
        lv_obj_remove_style_all(m_ring);
        lv_obj_clear_flag(m_ring, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_opa(m_ring, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_border_width(m_ring, 2, LV_PART_MAIN);
        lv_obj_set_style_border_color(m_ring, lv_color_hex(0x888888), LV_PART_MAIN);
        lv_obj_set_style_radius(m_ring, LV_RADIUS_CIRCLE, LV_PART_MAIN);
        lv_obj_set_style_pad_all(m_ring, 0, LV_PART_MAIN);

        m_dot = lv_obj_create(m_ring);
        lv_obj_remove_style_all(m_dot);
        lv_obj_clear_flag(m_dot, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_opa(m_dot, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_bg_color(m_dot, lv_color_hex(0x0e639c), LV_PART_MAIN);
        lv_obj_set_style_border_width(m_dot, 0, LV_PART_MAIN);
        lv_obj_set_style_radius(m_dot, LV_RADIUS_CIRCLE, LV_PART_MAIN);
        lv_obj_add_flag(m_dot, LV_OBJ_FLAG_HIDDEN);

        m_label = lv_label_create(m_obj);
        lv_obj_clear_flag(m_label, LV_OBJ_FLAG_CLICKABLE);
        lv_label_set_text(m_label, "");

        lv_obj_add_event_cb(m_obj, [](lv_event_t* e) {
            lv_obj_t* obj = static_cast<lv_obj_t*>(lv_event_get_current_target(e));
            if (lv_obj_has_state(obj, LV_STATE_CHECKED)) return;

            lv_obj_add_state(obj, LV_STATE_CHECKED);
            lv_obj_t* ring = lv_obj_get_child(obj, 0);
            lv_obj_t* dot = ring ? lv_obj_get_child(ring, 0) : nullptr;
            if (dot) lv_obj_clear_flag(dot, LV_OBJ_FLAG_HIDDEN);
            lv_obj_send_event(obj, LV_EVENT_VALUE_CHANGED, nullptr);
        }, LV_EVENT_CLICKED, nullptr);

        refresh_layout();
    }

    LRadioButton& set_text(const char* txt) {
        lv_label_set_text(m_label, txt ? txt : "");
        refresh_layout();
        return *this;
    }

    LRadioButton& set_checked(bool check) {
        if (check) {
            lv_obj_add_state(m_obj, LV_STATE_CHECKED);
            lv_obj_clear_flag(m_dot, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_remove_state(m_obj, LV_STATE_CHECKED);
            lv_obj_add_flag(m_dot, LV_OBJ_FLAG_HIDDEN);
        }
        return *this;
    }

    bool is_checked() const {
        return lv_obj_has_state(m_obj, LV_STATE_CHECKED);
    }

    LRadioButton& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    LRadioButton& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        refresh_layout();
        return *this;
    }

    template<typename F>
    LRadioButton& on_change(F&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<F>(callback));
        return *this;
    }

private:
    lv_obj_t* m_ring = nullptr;
    lv_obj_t* m_dot = nullptr;
    lv_obj_t* m_label = nullptr;

    void refresh_layout() {
        lv_obj_update_layout(m_obj);
        const int height = lv_obj_get_height(m_obj);
        const int width = lv_obj_get_width(m_obj);
        const int ring_size = (height - 8) > 18 ? 18 : (height - 8);
        const int safe_ring_size = ring_size > 8 ? ring_size : 8;
        const int x = 3;
        const int y = (height - safe_ring_size) / 2;
        const int gap = 8;

        lv_obj_set_pos(m_ring, x, y);
        lv_obj_set_size(m_ring, safe_ring_size, safe_ring_size);

        int dot_size = safe_ring_size / 2;
        if (dot_size < 6) dot_size = 6;
        lv_obj_set_size(m_dot, dot_size, dot_size);
        lv_obj_center(m_dot);

        const lv_font_t* font = lv_obj_get_style_text_font(m_label, LV_PART_MAIN);
        const int font_h = font ? lv_font_get_line_height(font) : 16;
        int label_x = x + safe_ring_size + gap;
        int label_width = width - label_x;
        if (label_width < 0) label_width = 0;
        int label_y = (height - font_h) / 2;
        if (label_y < 0) label_y = 0;
        lv_obj_set_pos(m_label, label_x, label_y);
        lv_obj_set_size(m_label, label_width, font_h);
        lv_obj_set_style_text_align(m_label, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
        lv_obj_set_style_pad_all(m_label, 0, LV_PART_MAIN);
    }
};

}
