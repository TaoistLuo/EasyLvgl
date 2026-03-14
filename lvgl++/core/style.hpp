#pragma once

/**
 * @file style.hpp
 * @brief 样式构建器，提供链式API设置样式
 * @version 1.0.0
 * @date 2026-03-15
 * 
 * Style类自动管理lv_style_t生命周期，支持链式调用。
 */

#include "lvgl/lvgl.h"

namespace lvgl {

/**
 * @class LStyle
 * @brief 样式构建器
 * 
 * 提供链式API设置LVGL样式属性，自动管理样式生命周期。
 * 
 * @example
 * @code
 * LStyle btn_style;
 * btn_style.bg_color(0x2196F3)
 *          .radius(8)
 *          .pad_all(10)
 *          .apply_to(button);
 * @endcode
 */
class LStyle {
private:
    lv_style_t m_style;      ///< LVGL样式对象
    bool m_initialized;      ///< 初始化标志
    
public:
    /**
     * @brief 构造函数，初始化样式
     */
    LStyle() : m_initialized(true) {
        lv_style_init(&m_style);
    }
    
    /**
     * @brief 析构函数，重置样式
     */
    ~LStyle() {
        if (m_initialized) {
            lv_style_reset(&m_style);
        }
    }
    
    // 禁止拷贝
    LStyle(const LStyle&) = delete;
    LStyle& operator=(const LStyle&) = delete;
    
    /**
     * @brief 移动构造函数
     * @param other 被移动的对象
     */
    LStyle(LStyle&& other) noexcept 
        : m_style(other.m_style), m_initialized(other.m_initialized) {
        other.m_initialized = false;
    }
    
    /**
     * @brief 移动赋值运算符
     * @param other 被移动的对象
     * @return LStyle& 当前对象引用
     */
    LStyle& operator=(LStyle&& other) noexcept {
        if (this != &other) {
            if (m_initialized) {
                lv_style_reset(&m_style);
            }
            m_style = other.m_style;
            m_initialized = other.m_initialized;
            other.m_initialized = false;
        }
        return *this;
    }
    
    /**
     * @brief 设置背景颜色
     * @param hex 十六进制颜色值（如0xFF0000表示红色）
     * @return LStyle& 当前对象引用，支持链式调用
     */
    LStyle& bg_color(uint32_t hex) {
        lv_style_set_bg_color(&m_style, lv_color_hex(hex));
        return *this;
    }
    
    /**
     * @brief 设置背景不透明度
     * @param opa 不透明度（0-255，255为完全不透明）
     * @return LStyle& 当前对象引用
     */
    LStyle& bg_opa(lv_opa_t opa) {
        lv_style_set_bg_opa(&m_style, opa);
        return *this;
    }
    
    /**
     * @brief 设置边框
     * @param width 边框宽度（像素）
     * @param color 边框颜色（十六进制）
     * @return LStyle& 当前对象引用
     */
    LStyle& border(int width, uint32_t color) {
        lv_style_set_border_width(&m_style, width);
        lv_style_set_border_color(&m_style, lv_color_hex(color));
        return *this;
    }
    
    /**
     * @brief 设置边框宽度
     * @param width 边框宽度（像素）
     * @return LStyle& 当前对象引用
     */
    LStyle& border_width(int width) {
        lv_style_set_border_width(&m_style, width);
        return *this;
    }
    
    /**
     * @brief 设置边框颜色
     * @param hex 十六进制颜色值
     * @return LStyle& 当前对象引用
     */
    LStyle& border_color(uint32_t hex) {
        lv_style_set_border_color(&m_style, lv_color_hex(hex));
        return *this;
    }
    
    /**
     * @brief 设置圆角半径
     * @param r 半径值（像素）
     * @return LStyle& 当前对象引用
     */
    LStyle& radius(int r) {
        lv_style_set_radius(&m_style, r);
        return *this;
    }
    
    /**
     * @brief 设置所有方向的内边距
     * @param pad 内边距值（像素）
     * @return LStyle& 当前对象引用
     */
    LStyle& pad_all(int pad) {
        lv_style_set_pad_all(&m_style, pad);
        return *this;
    }
    
    /**
     * @brief 设置顶部内边距
     * @param pad 内边距值
     * @return LStyle& 当前对象引用
     */
    LStyle& pad_top(int pad) {
        lv_style_set_pad_top(&m_style, pad);
        return *this;
    }
    
    /**
     * @brief 设置底部内边距
     * @param pad 内边距值
     * @return LStyle& 当前对象引用
     */
    LStyle& pad_bottom(int pad) {
        lv_style_set_pad_bottom(&m_style, pad);
        return *this;
    }
    
    /**
     * @brief 设置左侧内边距
     * @param pad 内边距值
     * @return LStyle& 当前对象引用
     */
    LStyle& pad_left(int pad) {
        lv_style_set_pad_left(&m_style, pad);
        return *this;
    }
    
    /**
     * @brief 设置右侧内边距
     * @param pad 内边距值
     * @return LStyle& 当前对象引用
     */
    LStyle& pad_right(int pad) {
        lv_style_set_pad_right(&m_style, pad);
        return *this;
    }
    
    /**
     * @brief 设置文本颜色
     * @param hex 十六进制颜色值
     * @return LStyle& 当前对象引用
     */
    LStyle& text_color(uint32_t hex) {
        lv_style_set_text_color(&m_style, lv_color_hex(hex));
        return *this;
    }
    
    /**
     * @brief 设置文本字体
     * @param font 字体指针
     * @return LStyle& 当前对象引用
     */
    LStyle& text_font(const lv_font_t* font) {
        lv_style_set_text_font(&m_style, font);
        return *this;
    }
    
    /**
     * @brief 设置阴影宽度
     * @param width 阴影宽度（像素）
     * @return LStyle& 当前对象引用
     */
    LStyle& shadow_width(int width) {
        lv_style_set_shadow_width(&m_style, width);
        return *this;
    }
    
    /**
     * @brief 设置阴影颜色
     * @param hex 十六进制颜色值
     * @return LStyle& 当前对象引用
     */
    LStyle& shadow_color(uint32_t hex) {
        lv_style_set_shadow_color(&m_style, lv_color_hex(hex));
        return *this;
    }
    
    /**
     * @brief 设置阴影X偏移
     * @param offset X偏移量（像素）
     * @return LStyle& 当前对象引用
     */
    LStyle& shadow_offset_x(int offset) {
        lv_style_set_shadow_offset_x(&m_style, offset);
        return *this;
    }
    
    /**
     * @brief 设置阴影Y偏移
     * @param offset Y偏移量（像素）
     * @return LStyle& 当前对象引用
     */
    LStyle& shadow_offset_y(int offset) {
        lv_style_set_shadow_offset_y(&m_style, offset);
        return *this;
    }
    
    /**
     * @brief 应用样式到对象
     * @param obj 目标对象
     * @param selector 样式选择器（默认0表示所有状态）
     * 
     * 常用选择器：
     * - 0 或 LV_PART_MAIN: 默认状态
     * - LV_STATE_PRESSED: 按下状态
     * - LV_STATE_CHECKED: 选中状态
     * - LV_STATE_FOCUSED: 聚焦状态
     */
    void apply_to(lv_obj_t* obj, lv_style_selector_t selector = 0) {
        lv_obj_add_style(obj, &m_style, selector);
    }
    
    /**
     * @brief 获取原生样式指针
     * @return lv_style_t* 原生样式指针
     * 
     * 用于与原生LVGL API交互。
     */
    lv_style_t* native() { return &m_style; }
    
    /**
     * @brief 获取原生样式指针（const版本）
     * @return const lv_style_t* 原生样式指针
     */
    const lv_style_t* native() const { return &m_style; }
};

} // namespace lvgl
