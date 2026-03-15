#pragma once

/**
 * @file slider.hpp
 * @brief 滑块组件
 * @version 2.0.0
 * @date 2026-03-15
 */

#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/style.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LSlider : public LWidgetCRTP<LSlider> {
public:
    explicit LSlider(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_slider_create(parent ? parent : lv_scr_act())) {}

    LSlider& set_range(int min, int max) {
        lv_slider_set_range(m_obj, min, max);
        return *this;
    }
    
    LSlider& set_value(int val, lv_anim_enable_t anim = LV_ANIM_OFF) {
        lv_slider_set_value(m_obj, val, anim);
        return *this;
    }

    LSlider& set_left_value(int val, lv_anim_enable_t anim = LV_ANIM_OFF) {
        lv_slider_set_left_value(m_obj, val, anim);
        return *this;
    }

    LSlider& set_mode(lv_slider_mode_t mode) {
        lv_slider_set_mode(m_obj, mode);
        return *this;
    }
    
    int value() const {
        return lv_slider_get_value(m_obj);
    }
    
    LSlider& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LSlider& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
    
    template<typename F>
    LSlider& on_change(F&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<F>(callback));
        return *this;
    }
};

} // namespace lvgl
