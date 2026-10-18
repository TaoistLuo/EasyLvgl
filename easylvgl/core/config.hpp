/**************************************************************************************************
 * @file        config.hpp
 * @brief       全局配置
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
#include "event.hpp"

namespace easylvgl {

class LStyle;

/**
 * @class ObjectConfig
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

} // namespace easylvgl
