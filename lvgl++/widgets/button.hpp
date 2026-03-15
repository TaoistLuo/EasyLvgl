#pragma once

/**
 * @file button.hpp
 * @brief 按钮组件
 * @version 2.0.0
 * @date 2026-03-15
 */

#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/style.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

/**
 * @class LButton
 * @brief 按钮组件，封装lv_button
 * 
 * 提供链式API和预设主题样式。
 */
class LButton : public LWidgetCRTP<LButton> {
private:
    lv_obj_t* m_label;
    
public:
    explicit LButton(lv_obj_t* parent = nullptr) 
        : LWidgetCRTP(lv_button_create(parent ? parent : lv_scr_act())) {
        m_label = lv_label_create(m_obj);
        lv_obj_center(m_label);
    }
    
    LButton& set_text(const char* txt) {
        lv_label_set_text(m_label, txt);
        return *this;
    }
    
    LButton& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
    
    LButton& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
    
    LButton& set_align(lv_align_t a) {
        lv_obj_set_align(m_obj, a);
        return *this;
    }
    
    template<typename F>
    LButton& on_click(F&& callback) {
        connect_event(m_obj, LV_EVENT_CLICKED, std::forward<F>(callback));
        return *this;
    }
    
    LButton& primary() {
        LStyle style;
        style.set_bg_color(0x2196F3)
             .set_radius(8)
             .set_pad_all(10)
             .apply_to(m_obj);
        return *this;
    }
    
    LButton& danger() {
        LStyle style;
        style.set_bg_color(0xF44336)
             .set_radius(8)
             .set_pad_all(10)
             .apply_to(m_obj);
        return *this;
    }
    
    LButton& success() {
        LStyle style;
        style.set_bg_color(0x4CAF50)
             .set_radius(8)
             .set_pad_all(10)
             .apply_to(m_obj);
        return *this;
    }
    
    LButton& warning() {
        LStyle style;
        style.set_bg_color(0xFF9800)
             .set_radius(8)
             .set_pad_all(10)
             .apply_to(m_obj);
        return *this;
    }
    
    LButton& set_disabled(bool dis = true) {
        if (dis) {
            lv_obj_add_state(m_obj, LV_STATE_DISABLED);
        } else {
            lv_obj_remove_state(m_obj, LV_STATE_DISABLED);
        }
        return *this;
    }
};

} // namespace lvgl
