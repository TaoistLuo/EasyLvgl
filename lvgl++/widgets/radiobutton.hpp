#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/style.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LRadioButton : public LWidgetCRTP<LRadioButton> {
public:
    explicit LRadioButton(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_checkbox_create(parent ? parent : lv_scr_act())) {
        lv_checkbox_set_text(m_obj, "");
    }

    LRadioButton& set_text(const char* txt) {
        lv_checkbox_set_text(m_obj, txt);
        return *this;
    }

    LRadioButton& set_checked(bool check) {
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

    LRadioButton& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    LRadioButton& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    template<typename F>
    LRadioButton& on_change(F&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<F>(callback));
        return *this;
    }
};

} // namespace lvgl
