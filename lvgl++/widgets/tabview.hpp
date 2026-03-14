#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LTabView : public LWidget {
public:
    explicit LTabView(lv_obj_t* parent = nullptr)
        : LWidget(lv_tabview_create(parent ? parent : lv_scr_act())) {}

    lv_obj_t* add_tab(const char* name) {
        return lv_tabview_add_tab(m_obj, name);
    }

    LTabView& set_active(uint32_t id, lv_anim_enable_t anim = LV_ANIM_OFF) {
        lv_tabview_set_active(m_obj, id, anim);
        return *this;
    }

    uint32_t get_tab_active() const {
        return lv_tabview_get_tab_active(m_obj);
    }
};

} // namespace lvgl
