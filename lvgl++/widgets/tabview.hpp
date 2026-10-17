#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LTabView : public LWidgetCRTP<LTabView> {
public:
    explicit LTabView(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_tabview_create(parent ? parent : lv_scr_act())) {}

    lv_obj_t* add_tab(const char* name) {
        return lv_tabview_add_tab(m_obj, name);
    }

    LTabView& set_active(uint32_t id, lv_anim_enable_t anim = LV_ANIM_OFF) {
        lv_tabview_set_active(m_obj, id, anim);
        return *this;
    }

    uint32_t active_tab() const {
        return lv_tabview_get_tab_active(m_obj);
    }

    LTabView& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LTabView& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
    LTabView& set_tab_bar_position(int dir) { lv_tabview_set_tab_bar_position(m_obj, static_cast<lv_dir_t>(dir)); return *this; }

    LTabView& set_tab_bar_size(int size) { lv_tabview_set_tab_bar_size(m_obj, size); return *this; }

};

} // namespace lvgl
