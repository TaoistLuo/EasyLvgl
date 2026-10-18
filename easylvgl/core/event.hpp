#pragma once

/**
 * @file event.hpp
 * @brief 类型安全的事件系统
 * @version 1.0.0
 * @date 2026-03-15
 */

#include "lvgl/lvgl.h"
#include <functional>
#include <utility>
#include <type_traits>

namespace easylvgl {

/**
 * @brief 类型安全的事件连接函数
 * 
 * @tparam F 回调函数类型（自动推导）
 * @param obj 目标对象
 * @param code 事件代码
 * @param callback 回调函数（支持lambda、函数指针、函数对象）
 */
template<typename F>
void connect_event(lv_obj_t* obj, lv_event_code_t code, F&& callback) {
    using CallbackType = typename std::decay<F>::type;
    auto* cb = new CallbackType(std::forward<F>(callback));
    
    lv_obj_add_event_cb(obj, [](lv_event_t* e) {
        auto* f = static_cast<CallbackType*>(lv_event_get_user_data(e));
        (*f)(e);
    }, code, cb);

    // Register cleanup: delete the heap-allocated callback when the object is deleted
    lv_obj_add_event_cb(obj, [](lv_event_t* e) {
        auto* f = static_cast<CallbackType*>(lv_event_get_user_data(e));
        delete f;
    }, LV_EVENT_DELETE, cb);
}

/**
 * @brief 简化的点击事件连接（无参数版本）
 * 
 * @tparam F 回调函数类型
 * @param obj 目标对象
 * @param callback 回调函数
 */
template<typename F>
void on_clicked(lv_obj_t* obj, F&& callback) {
    using CallbackType = typename std::decay<F>::type;
    auto* cb = new CallbackType(std::forward<F>(callback));
    
    lv_obj_add_event_cb(obj, [](lv_event_t* e) {
        auto* f = static_cast<CallbackType*>(lv_event_get_user_data(e));
        (*f)();
    }, LV_EVENT_CLICKED, cb);

    lv_obj_add_event_cb(obj, [](lv_event_t* e) {
        auto* f = static_cast<CallbackType*>(lv_event_get_user_data(e));
        delete f;
    }, LV_EVENT_DELETE, cb);
}

/**
 * @brief 值改变事件连接
 * 
 * @tparam F 回调函数类型
 * @param obj 目标对象
 * @param callback 回调函数
 */
template<typename F>
void on_value_changed(lv_obj_t* obj, F&& callback) {
    connect_event(obj, LV_EVENT_VALUE_CHANGED, std::forward<F>(callback));
}

} // namespace easylvgl
