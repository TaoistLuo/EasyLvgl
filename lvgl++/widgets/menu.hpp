#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LMenu : public LWidgetCRTP<LMenu> {
public:
    explicit LMenu(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_menu_create(parent ? parent : lv_scr_act())) {}

    lv_obj_t* page_create(const char* title) {
        return lv_menu_page_create(m_obj, title);
    }

    lv_obj_t* cont_create(lv_obj_t* parent) {
        return lv_menu_cont_create(parent);
    }

    lv_obj_t* section_create(lv_obj_t* parent) {
        return lv_menu_section_create(parent);
    }

    LMenu& set_page(lv_obj_t* page) {
        lv_menu_set_page(m_obj, page);
        return *this;
    }
};

} // namespace lvgl
