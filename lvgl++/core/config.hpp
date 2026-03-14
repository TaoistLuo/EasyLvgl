#pragma once

/**
 * @file config.hpp
 * @brief 对象配置器，提供链式API
 * @version 1.0.0
 * @date 2026-03-15
 */

#include "lvgl/lvgl.h"
#include "event.hpp"

namespace lvgl {

class LStyle;

/**
 * @class ObjectConfig
 * @brief 对象配置器，提供链式API配置对象属性
 */
class ObjectConfig {
private:
    lv_obj_t* m_obj;
    
public:
    explicit ObjectConfig(lv_obj_t* obj) : m_obj(obj) {}
    
    ObjectConfig& size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
    
    ObjectConfig& pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
    
    ObjectConfig& align(lv_align_t a) {
        lv_obj_set_align(m_obj, a);
        return *this;
    }
    
    ObjectConfig& flex_flow(lv_flex_flow_t flow) {
        lv_obj_set_flex_flow(m_obj, flow);
        return *this;
    }
    
    ObjectConfig& flex_align(lv_flex_align_t main, 
                             lv_flex_align_t cross, 
                             lv_flex_align_t track) {
        lv_obj_set_flex_align(m_obj, main, cross, track);
        return *this;
    }
    
    template<typename F>
    ObjectConfig& on_clicked(F&& callback) {
        connect_event(m_obj, LV_EVENT_CLICKED, std::forward<F>(callback));
        return *this;
    }
    
    ObjectConfig& style(LStyle& style, lv_style_selector_t selector = 0);
    
    lv_obj_t* get() { return m_obj; }
};

inline ObjectConfig configure(lv_obj_t* obj) {
    return ObjectConfig(obj);
}

} // namespace lvgl
