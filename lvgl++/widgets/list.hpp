#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LList : public LWidgetCRTP<LList> {
public:
    explicit LList(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_list_create(parent ? parent : lv_scr_act())) {}

    lv_obj_t* add_text(const char* txt) {
        return lv_list_add_text(m_obj, txt);
    }

    lv_obj_t* add_button(const char* icon, const char* txt) {
        return lv_list_add_button(m_obj, icon, txt);
    }

    const char* get_button_text(lv_obj_t* btn) {
        return lv_list_get_button_text(m_obj, btn);
    }

    LList& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LList& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
};

} // namespace lvgl
