#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LKeyboard : public LWidget {
public:
    explicit LKeyboard(lv_obj_t* parent = nullptr)
        : LWidget(lv_keyboard_create(parent ? parent : lv_scr_act())) {}

    LKeyboard& textarea(lv_obj_t* ta) {
        lv_keyboard_set_textarea(m_obj, ta);
        return *this;
    }

    LKeyboard& mode(lv_keyboard_mode_t mode) {
        lv_keyboard_set_mode(m_obj, mode);
        return *this;
    }
};

} // namespace lvgl
