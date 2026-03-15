#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LKeyboard : public LWidgetCRTP<LKeyboard> {
public:
    explicit LKeyboard(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_keyboard_create(parent ? parent : lv_scr_act())) {}

    LKeyboard& set_textarea(lv_obj_t* ta) {
        lv_keyboard_set_textarea(m_obj, ta);
        return *this;
    }

    LKeyboard& set_mode(lv_keyboard_mode_t mode) {
        lv_keyboard_set_mode(m_obj, mode);
        return *this;
    }

    LKeyboard& set_popovers(bool en) {
        lv_keyboard_set_popovers(m_obj, en);
        return *this;
    }

    LKeyboard& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LKeyboard& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
};

} // namespace lvgl
