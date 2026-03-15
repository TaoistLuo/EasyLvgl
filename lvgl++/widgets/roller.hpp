#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LRoller : public LWidgetCRTP<LRoller> {
public:
    explicit LRoller(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_roller_create(parent ? parent : lv_scr_act())) {}

    LRoller& set_options(const char* opts, lv_roller_mode_t mode = LV_ROLLER_MODE_NORMAL) {
        lv_roller_set_options(m_obj, opts, mode);
        return *this;
    }

    LRoller& set_selected(uint16_t idx, lv_anim_enable_t anim = LV_ANIM_OFF) {
        lv_roller_set_selected(m_obj, idx, anim);
        return *this;
    }

    uint16_t selected() const {
        return lv_roller_get_selected(m_obj);
    }

    LRoller& set_visible_row_count(uint8_t row_cnt) {
        lv_roller_set_visible_row_count(m_obj, row_cnt);
        return *this;
    }

    LRoller& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    LRoller& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    template<typename Func>
    LRoller& on_change(Func&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<Func>(callback));
        return *this;
    }
};

} // namespace lvgl
