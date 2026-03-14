#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LWin : public LWidget {
public:
    explicit LWin(lv_obj_t* parent = nullptr)
        : LWidget(lv_win_create(parent ? parent : lv_scr_act())) {}

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
};

} // namespace lvgl
