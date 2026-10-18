#pragma once

/**
 * @file checkbox.hpp
 * @brief 复选框组件
 * @version 2.0.0
 * @date 2026-03-15
 */

#include "easylvgl/core/widget.hpp"
#include "easylvgl/core/event.hpp"
#include "lvgl/lvgl.h"

namespace easylvgl {

class LCheckbox : public LWidgetCRTP<LCheckbox> {
public:
    explicit LCheckbox(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_checkbox_create(parent ? parent : lv_scr_act())) {
        apply_checkbox_style();
    }

    LCheckbox& set_text(const char* txt) {
        lv_checkbox_set_text(m_obj, txt);
        return *this;
    }
    
    LCheckbox& set_checked(bool check) {
        if (check) {
            lv_obj_add_state(m_obj, LV_STATE_CHECKED);
        } else {
            lv_obj_remove_state(m_obj, LV_STATE_CHECKED);
        }
        return *this;
    }
    
    bool is_checked() const {
        return lv_obj_has_state(m_obj, LV_STATE_CHECKED);
    }

    LCheckbox& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    LCheckbox& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        apply_checkbox_style();
        return *this;
    }
    
    template<typename F>
    LCheckbox& on_change(F&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<F>(callback));
        return *this;
    }

private:
    void apply_checkbox_style() {
        lv_obj_update_layout(m_obj);
        const lv_font_t* font = lv_obj_get_style_text_font(m_obj, LV_PART_MAIN);
        const int font_h = font ? lv_font_get_line_height(font) : 14;
        const int obj_h = lv_obj_get_height(m_obj);
        const int extra = obj_h > font_h ? (obj_h - font_h) / 2 : 0;
        const int top_extra = extra > 0 ? extra + 1 : 0;
        const int bottom_extra = extra > 0 ? extra - 1 : 0;

        lv_obj_set_style_pad_top(m_obj, top_extra, LV_PART_MAIN);
        lv_obj_set_style_pad_bottom(m_obj, bottom_extra, LV_PART_MAIN);
        lv_obj_set_style_pad_column(m_obj, 6, LV_PART_MAIN);
        lv_obj_set_style_pad_all(m_obj, 1, LV_PART_INDICATOR);
    }
};

} // namespace easylvgl
