#pragma once
#include "easylvgl/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace easylvgl {

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

    LMenu& set_mode_header(lv_menu_mode_header_t mode) {
        lv_menu_set_mode_header(m_obj, mode);
        return *this;
    }

    LMenu& set_mode_header(int mode) {
        return set_mode_header(static_cast<lv_menu_mode_header_t>(mode));
    }

    LMenu& set_mode_root_back_button(lv_menu_mode_root_back_button_t mode) {
        lv_menu_set_mode_root_back_button(m_obj, mode);
        return *this;
    }

    LMenu& set_mode_root_back_button(int mode) {
        return set_mode_root_back_button(static_cast<lv_menu_mode_root_back_button_t>(mode));
    }

    LMenu& set_size(int w, int h) { lv_obj_set_size(m_obj, w, h); return *this; }
    LMenu& set_pos(int x, int y) { lv_obj_set_pos(m_obj, x, y); return *this; }
};

// A menu page (root or sub page). Created on a menu object.
class LMenuPage : public LWidgetCRTP<LMenuPage> {
public:
    explicit LMenuPage(lv_obj_t* menu, const char* title = "")
        : LWidgetCRTP(lv_menu_page_create(menu ? menu : lv_scr_act(), title)) {}

    LMenuPage& set_size(int w, int h) { lv_obj_set_size(m_obj, w, h); return *this; }
    LMenuPage& set_pos(int x, int y) { lv_obj_set_pos(m_obj, x, y); return *this; }
};

// A menu content container (holds widgets/buttons), created on a page/section.
class LMenuCont : public LWidgetCRTP<LMenuCont> {
public:
    explicit LMenuCont(lv_obj_t* parent)
        : LWidgetCRTP(lv_menu_cont_create(parent ? parent : lv_scr_act())) {}

    LMenuCont& set_size(int w, int h) { lv_obj_set_size(m_obj, w, h); return *this; }
    LMenuCont& set_pos(int x, int y) { lv_obj_set_pos(m_obj, x, y); return *this; }
};

// A menu section (group of containers), created on a page/cont.
class LMenuSection : public LWidgetCRTP<LMenuSection> {
public:
    explicit LMenuSection(lv_obj_t* parent)
        : LWidgetCRTP(lv_menu_section_create(parent ? parent : lv_scr_act())) {}

    LMenuSection& set_size(int w, int h) { lv_obj_set_size(m_obj, w, h); return *this; }
    LMenuSection& set_pos(int x, int y) { lv_obj_set_pos(m_obj, x, y); return *this; }
};

} // namespace easylvgl
