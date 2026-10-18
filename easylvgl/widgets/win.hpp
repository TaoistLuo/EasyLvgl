#pragma once
#include "easylvgl/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace easylvgl {

class LWin : public LWidgetCRTP<LWin> {
public:
    explicit LWin(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_win_create(parent ? parent : lv_scr_act())) {}

    lv_obj_t* add_title(const char* txt) {
        return lv_win_add_title(m_obj, txt);
    }

    lv_obj_t* add_button(const void* icon, int32_t btn_w) {
        return lv_win_add_button(m_obj, icon, btn_w);
    }

    lv_obj_t* get_header() {
        return lv_win_get_header(m_obj);
    }

    lv_obj_t* get_content() {
        return lv_win_get_content(m_obj);
    }

    LWin& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LWin& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
};

} // namespace easylvgl
