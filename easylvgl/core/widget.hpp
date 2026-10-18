/**************************************************************************************************
 * @file        widget.hpp
 * @brief       组件基类（LWidget / LWidgetCRTP）
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
#include <utility>

namespace easylvgl {

class LWidget;

template<typename Derived>
class LWidgetCRTP;

#define LVGL_WIDGET_FLUENT_SIZE(CLASS_NAME) \
    CLASS_NAME& set_size(int w, int h) { \
        LWidgetCRTP<CLASS_NAME>::set_size(w, h); \
        return *this; \
    }

#define LVGL_WIDGET_FLUENT_POS(CLASS_NAME) \
    CLASS_NAME& set_pos(int x, int y) { \
        LWidgetCRTP<CLASS_NAME>::set_pos(x, y); \
        return *this; \
    }

#define LVGL_WIDGET_FLUENT_ALIGN(CLASS_NAME) \
    CLASS_NAME& set_align(lv_align_t align_value) { \
        LWidgetCRTP<CLASS_NAME>::set_align(align_value); \
        return *this; \
    }

#define LVGL_WIDGET_FLUENT_GEOMETRY(CLASS_NAME) \
    LVGL_WIDGET_FLUENT_SIZE(CLASS_NAME) \
    LVGL_WIDGET_FLUENT_POS(CLASS_NAME) \
    LVGL_WIDGET_FLUENT_ALIGN(CLASS_NAME)

/**
 * @class LWidget
 * 
 * 提供自动内存管理、移动语义和类型安全的基础设施。
 * 
 * @example
 * @code
 * LWidget btn(lv_button_create(parent));
 * // 自动管理生命周期，无需手动delete
 * @endcode
 */
class LWidget {
protected:
    lv_obj_t* m_obj;  ///< 原生LVGL对象指针
    
public:
    /**
     * @param obj 原生LVGL对象指针
     * 
     * 获取对象的所有权，析构时自动释放。
     */
    explicit LWidget(lv_obj_t* obj) : m_obj(obj) {}
    
    /**
     * @param other 被移动的对象
     * 
     * 转移对象所有权，原对象不再管理该LVGL对象。
     */
    LWidget(LWidget&& other) noexcept : m_obj(other.m_obj) {
        other.m_obj = nullptr;
    }
    
    /**
     * @param other 被移动的对象
     * @return LWidget& 当前对象引用
     * 
     * 释放当前对象，转移新对象的所有权。
     */
    LWidget& operator=(LWidget&& other) noexcept {
        if (this != &other) {
            if (m_obj) {
                lv_obj_delete(m_obj);
            }
            m_obj = other.m_obj;
            other.m_obj = nullptr;
        }
        return *this;
    }
    
    // 禁止拷贝
    LWidget(const LWidget&) = delete;
    LWidget& operator=(const LWidget&) = delete;
    
    /**
     * 
     * 使用lv_obj_delete同步删除，避免Ctrl+C时崩溃。
     * 同步删除不依赖事件循环，可以安全地在程序退出时调用。
     */
    virtual ~LWidget() {
        if (m_obj) {
            lv_obj_delete(m_obj);
            m_obj = nullptr;
        }
    }
    
    /**
     * @return lv_obj_t* 原生指针
     * 
     * 用于与原生LVGL API交互。
     */
    lv_obj_t* native() { return m_obj; }
    
    /**
     * @return const lv_obj_t* 原生指针
     */
    const lv_obj_t* native() const { return m_obj; }
};

template<typename Derived>
class LWidgetCRTP : public LWidget {
public:
    using LWidget::LWidget;
    using LWidget::operator=;

    Derived& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return static_cast<Derived&>(*this);
    }

    Derived& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return static_cast<Derived&>(*this);
    }

    Derived& set_align(lv_align_t align_value) {
        lv_obj_set_align(m_obj, align_value);
        return static_cast<Derived&>(*this);
    }
};

} // namespace easylvgl
