/**************************************************************************************************
 * @file        event.hpp
 * @brief       事件系统
 * @details     EasyLVGL —— 让 LVGL 开发像 Qt 一样简单：C++ 封装 + 可视化设计器 + 代码生成。
 *
 * @author      taoist.luo
 * @version     1.0.0
 * @date        2026-10-18
 *
 * Copyright (c) 2026 taoist.luo (https://github.com/TaoistLuo/EasyLVGL)
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *************************************************************************************************/

#pragma once
#include "lvgl/lvgl.h"
#include <functional>
#include <utility>
#include <type_traits>

namespace easylvgl {

/**
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
