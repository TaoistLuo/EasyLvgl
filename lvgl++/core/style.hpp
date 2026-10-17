#pragma once

/**
 * @file style.hpp
 * @brief 样式构建器，提供链式API设置样式
 * @version 2.0.0
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
 * btn_style.set_bg_color(0x2196F3)
 *          .set_radius(8)
 *          .set_pad_all(10)
 *          .apply_to(button);
 * @endcode
 */

/**
 * @brief 按像素字号取已编译的 Montserrat 字体（lv_conf.h 里启用的那批）
 * @param px 字号，未精确命中时回退到最近的可用字号
 */
inline const lv_font_t* font_by_size(int px) {
    switch (px) {
        case 12: return &lv_font_montserrat_12;
        case 14: return &lv_font_montserrat_14;
        case 16: return &lv_font_montserrat_16;
        case 18: return &lv_font_montserrat_18;
        case 20: return &lv_font_montserrat_20;
        case 22: return &lv_font_montserrat_22;
        case 24: return &lv_font_montserrat_24;
        case 26: return &lv_font_montserrat_26;
        case 28: return &lv_font_montserrat_28;
        case 30: return &lv_font_montserrat_30;
        case 32: return &lv_font_montserrat_32;
        case 34: return &lv_font_montserrat_34;
        case 36: return &lv_font_montserrat_36;
        case 38: return &lv_font_montserrat_38;
        case 40: return &lv_font_montserrat_40;
        case 42: return &lv_font_montserrat_42;
        case 44: return &lv_font_montserrat_44;
        case 46: return &lv_font_montserrat_46;
        case 48: return &lv_font_montserrat_48;
        default: break;
    }
        if (px <= 12) return &lv_font_montserrat_12;
        if (px <= 14) return &lv_font_montserrat_14;
        if (px <= 16) return &lv_font_montserrat_16;
        if (px <= 18) return &lv_font_montserrat_18;
        if (px <= 20) return &lv_font_montserrat_20;
        if (px <= 22) return &lv_font_montserrat_22;
        if (px <= 24) return &lv_font_montserrat_24;
        if (px <= 26) return &lv_font_montserrat_26;
        if (px <= 28) return &lv_font_montserrat_28;
        if (px <= 30) return &lv_font_montserrat_30;
        if (px <= 32) return &lv_font_montserrat_32;
        if (px <= 34) return &lv_font_montserrat_34;
        if (px <= 36) return &lv_font_montserrat_36;
        if (px <= 38) return &lv_font_montserrat_38;
        if (px <= 40) return &lv_font_montserrat_40;
        if (px <= 42) return &lv_font_montserrat_42;
        if (px <= 44) return &lv_font_montserrat_44;
        if (px <= 46) return &lv_font_montserrat_46;
    return &lv_font_montserrat_48;
}

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
    LStyle& set_bg_color(uint32_t hex) {
        lv_style_set_bg_color(&m_style, lv_color_hex(hex));
        return *this;
    }
    
    /**
     * @brief 设置背景不透明度
     * @param opa 不透明度（0-255，255为完全不透明）
     * @return LStyle& 当前对象引用
     */
    LStyle& set_bg_opa(lv_opa_t opa) {
        lv_style_set_bg_opa(&m_style, opa);
        return *this;
    }
    
    /**
     * @brief 设置边框
     * @param width 边框宽度（像素）
     * @param color 边框颜色（十六进制）
     * @return LStyle& 当前对象引用
     */
    LStyle& set_border(int width, uint32_t color) {
        lv_style_set_border_width(&m_style, width);
        lv_style_set_border_color(&m_style, lv_color_hex(color));
        return *this;
    }
    
    /**
     * @brief 设置边框宽度
     * @param width 边框宽度（像素）
     * @return LStyle& 当前对象引用
     */
    LStyle& set_border_width(int width) {
        lv_style_set_border_width(&m_style, width);
        return *this;
    }
    
    /**
     * @brief 设置边框颜色
     * @param hex 十六进制颜色值
     * @return LStyle& 当前对象引用
     */
    LStyle& set_border_color(uint32_t hex) {
        lv_style_set_border_color(&m_style, lv_color_hex(hex));
        return *this;
    }
    
    /**
     * @brief 设置圆角半径
     * @param r 半径值（像素）
     * @return LStyle& 当前对象引用
     */
    LStyle& set_radius(int r) {
        lv_style_set_radius(&m_style, r);
        return *this;
    }
    
    /**
     * @brief 设置所有方向的内边距
     * @param pad 内边距值（像素）
     * @return LStyle& 当前对象引用
     */
    LStyle& set_pad_all(int pad) {
        lv_style_set_pad_all(&m_style, pad);
        return *this;
    }
    
    /**
     * @brief 设置顶部内边距
     * @param pad 内边距值
     * @return LStyle& 当前对象引用
     */
    LStyle& set_pad_top(int pad) {
        lv_style_set_pad_top(&m_style, pad);
        return *this;
    }
    
    /**
     * @brief 设置底部内边距
     * @param pad 内边距值
     * @return LStyle& 当前对象引用
     */
    LStyle& set_pad_bottom(int pad) {
        lv_style_set_pad_bottom(&m_style, pad);
        return *this;
    }
    
    /**
     * @brief 设置左侧内边距
     * @param pad 内边距值
     * @return LStyle& 当前对象引用
     */
    LStyle& set_pad_left(int pad) {
        lv_style_set_pad_left(&m_style, pad);
        return *this;
    }
    
    /**
     * @brief 设置右侧内边距
     * @param pad 内边距值
     * @return LStyle& 当前对象引用
     */
    LStyle& set_pad_right(int pad) {
        lv_style_set_pad_right(&m_style, pad);
        return *this;
    }
    
    /**
     * @brief 设置文本颜色
     * @param hex 十六进制颜色值
     * @return LStyle& 当前对象引用
     */
    LStyle& set_text_color(uint32_t hex) {
        lv_style_set_text_color(&m_style, lv_color_hex(hex));
        return *this;
    }
    
    /**
     * @brief 设置文本字体
     * @param font 字体指针
     * @return LStyle& 当前对象引用
     */
    LStyle& set_text_font(const lv_font_t* font) {
        lv_style_set_text_font(&m_style, font);
        return *this;
    }
    
    /**
     * @brief 设置阴影宽度
     * @param width 阴影宽度（像素）
     * @return LStyle& 当前对象引用
     */
    LStyle& set_shadow_width(int width) {
        lv_style_set_shadow_width(&m_style, width);
        return *this;
    }
    
    /**
     * @brief 设置阴影颜色
     * @param hex 十六进制颜色值
     * @return LStyle& 当前对象引用
     */
    LStyle& set_shadow_color(uint32_t hex) {
        lv_style_set_shadow_color(&m_style, lv_color_hex(hex));
        return *this;
    }
    
    /**
     * @brief 设置阴影X偏移
     * @param offset X偏移量（像素）
     * @return LStyle& 当前对象引用
     */
    LStyle& set_shadow_offset_x(int offset) {
        lv_style_set_shadow_offset_x(&m_style, offset);
        return *this;
    }
    
    /**
     * @brief 设置阴影Y偏移
     * @param offset Y偏移量（像素）
     * @return LStyle& 当前对象引用
     */
    LStyle& set_shadow_offset_y(int offset) {
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

    // === 通用样式属性（与设计器属性面板一一对应，供 luic 生成样式 ===
    LStyle& set_bg_grad_color(uint32_t hex) { lv_style_set_bg_grad_color(&m_style, lv_color_hex(hex)); return *this; }
    LStyle& set_bg_grad_dir(int v) { lv_style_set_bg_grad_dir(&m_style, (lv_grad_dir_t)v); return *this; }
    LStyle& set_bg_main_stop(int v) { lv_style_set_bg_main_stop(&m_style, v); return *this; }
    LStyle& set_bg_grad_stop(int v) { lv_style_set_bg_grad_stop(&m_style, v); return *this; }
    LStyle& set_bg_image_opa(int v) { lv_style_set_bg_image_opa(&m_style, v); return *this; }
    LStyle& set_bg_image_recolor(uint32_t hex) { lv_style_set_bg_image_recolor(&m_style, lv_color_hex(hex)); return *this; }
    LStyle& set_bg_image_recolor_opa(int v) { lv_style_set_bg_image_recolor_opa(&m_style, v); return *this; }
    LStyle& set_border_opa(int v) { lv_style_set_border_opa(&m_style, v); return *this; }
    LStyle& set_border_side(int v) { lv_style_set_border_side(&m_style, (lv_border_side_t)v); return *this; }
    LStyle& set_border_post(bool v) { lv_style_set_border_post(&m_style, v); return *this; }
    LStyle& set_outline_color(uint32_t hex) { lv_style_set_outline_color(&m_style, lv_color_hex(hex)); return *this; }
    LStyle& set_outline_width(int v) { lv_style_set_outline_width(&m_style, v); return *this; }
    LStyle& set_outline_opa(int v) { lv_style_set_outline_opa(&m_style, v); return *this; }
    LStyle& set_outline_pad(int v) { lv_style_set_outline_pad(&m_style, v); return *this; }
    LStyle& set_shadow_ofs_x(int v) { lv_style_set_shadow_ofs_x(&m_style, v); return *this; }
    LStyle& set_shadow_ofs_y(int v) { lv_style_set_shadow_ofs_y(&m_style, v); return *this; }
    LStyle& set_shadow_spread(int v) { lv_style_set_shadow_spread(&m_style, v); return *this; }
    LStyle& set_shadow_opa(int v) { lv_style_set_shadow_opa(&m_style, v); return *this; }
    LStyle& set_pad_row(int v) { lv_style_set_pad_row(&m_style, v); return *this; }
    LStyle& set_pad_column(int v) { lv_style_set_pad_column(&m_style, v); return *this; }
    LStyle& set_margin_top(int v) { lv_style_set_margin_top(&m_style, v); return *this; }
    LStyle& set_margin_bottom(int v) { lv_style_set_margin_bottom(&m_style, v); return *this; }
    LStyle& set_margin_left(int v) { lv_style_set_margin_left(&m_style, v); return *this; }
    LStyle& set_margin_right(int v) { lv_style_set_margin_right(&m_style, v); return *this; }
    LStyle& set_min_width(int v) { lv_style_set_min_width(&m_style, v); return *this; }
    LStyle& set_max_width(int v) { lv_style_set_max_width(&m_style, v); return *this; }
    LStyle& set_min_height(int v) { lv_style_set_min_height(&m_style, v); return *this; }
    LStyle& set_max_height(int v) { lv_style_set_max_height(&m_style, v); return *this; }
    LStyle& set_clip_corner(bool v) { lv_style_set_clip_corner(&m_style, v); return *this; }
    LStyle& set_opa(int v) { lv_style_set_opa(&m_style, v); return *this; }
    LStyle& set_blend_mode(int v) { lv_style_set_blend_mode(&m_style, (lv_blend_mode_t)v); return *this; }
    LStyle& set_base_dir(int v) { lv_style_set_base_dir(&m_style, (lv_base_dir_t)v); return *this; }
    LStyle& set_text_opa(int v) { lv_style_set_text_opa(&m_style, v); return *this; }
    LStyle& set_text_letter_space(int v) { lv_style_set_text_letter_space(&m_style, v); return *this; }
    LStyle& set_text_line_space(int v) { lv_style_set_text_line_space(&m_style, v); return *this; }
    LStyle& set_text_align(int v) { lv_style_set_text_align(&m_style, (lv_text_align_t)v); return *this; }
    LStyle& set_text_decor(int v) { lv_style_set_text_decor(&m_style, (lv_text_decor_t)v); return *this; }
    LStyle& set_transform_rotation(int v) { lv_style_set_transform_rotation(&m_style, v); return *this; }
    LStyle& set_transform_scale_x(int v) { lv_style_set_transform_scale_x(&m_style, v); return *this; }
    LStyle& set_transform_scale_y(int v) { lv_style_set_transform_scale_y(&m_style, v); return *this; }
    LStyle& set_transform_pivot_x(int v) { lv_style_set_transform_pivot_x(&m_style, v); return *this; }
    LStyle& set_transform_pivot_y(int v) { lv_style_set_transform_pivot_y(&m_style, v); return *this; }
    LStyle& set_translate_x(int v) { lv_style_set_translate_x(&m_style, v); return *this; }
    LStyle& set_translate_y(int v) { lv_style_set_translate_y(&m_style, v); return *this; }
    LStyle& set_transform_skew_x(int v) { lv_style_set_transform_skew_x(&m_style, v); return *this; }
    LStyle& set_transform_skew_y(int v) { lv_style_set_transform_skew_y(&m_style, v); return *this; }
    LStyle& set_line_color(uint32_t hex) { lv_style_set_line_color(&m_style, lv_color_hex(hex)); return *this; }
    LStyle& set_line_width(int v) { lv_style_set_line_width(&m_style, v); return *this; }
    LStyle& set_line_opa(int v) { lv_style_set_line_opa(&m_style, v); return *this; }
    LStyle& set_line_dash_width(int v) { lv_style_set_line_dash_width(&m_style, v); return *this; }
    LStyle& set_line_dash_gap(int v) { lv_style_set_line_dash_gap(&m_style, v); return *this; }
    LStyle& set_line_rounded(bool v) { lv_style_set_line_rounded(&m_style, v); return *this; }
    LStyle& set_arc_color(uint32_t hex) { lv_style_set_arc_color(&m_style, lv_color_hex(hex)); return *this; }
    LStyle& set_arc_width(int v) { lv_style_set_arc_width(&m_style, v); return *this; }
    LStyle& set_arc_opa(int v) { lv_style_set_arc_opa(&m_style, v); return *this; }
    LStyle& set_arc_rounded(bool v) { lv_style_set_arc_rounded(&m_style, v); return *this; }
    LStyle& set_image_opa(int v) { lv_style_set_image_opa(&m_style, v); return *this; }
    LStyle& set_image_recolor(uint32_t hex) { lv_style_set_image_recolor(&m_style, lv_color_hex(hex)); return *this; }
    LStyle& set_image_recolor_opa(int v) { lv_style_set_image_recolor_opa(&m_style, v); return *this; }
    LStyle& set_color(uint32_t hex) { lv_style_set_line_color(&m_style, lv_color_hex(hex)); return *this; }
    LStyle& set_opacity(int v) { lv_style_set_image_opa(&m_style, v); return *this; }
    LStyle& set_width(int v) { lv_style_set_line_width(&m_style, v); return *this; }
    LStyle& set_dash_width(int v) { lv_style_set_line_dash_width(&m_style, v); return *this; }
    LStyle& set_dash_gap(int v) { lv_style_set_line_dash_gap(&m_style, v); return *this; }
    LStyle& set_rounded(bool v) { lv_style_set_line_rounded(&m_style, v); return *this; }
    LStyle& set_col_spacing(int v) { lv_style_set_pad_column(&m_style, v); return *this; }
    LStyle& set_row_spacing(int v) { lv_style_set_pad_row(&m_style, v); return *this; }
    LStyle& set_padding(int v) { lv_style_set_pad_all(&m_style, v); return *this; }
    LStyle& set_bg_image_src(const void* src) { lv_style_set_bg_image_src(&m_style, src); return *this; }
    LStyle& set_bg_image_tiled(bool v) { lv_style_set_bg_image_tiled(&m_style, v); return *this; }

    LStyle& set_text_font_size(int px) { lv_style_set_text_font(&m_style, font_by_size(px)); return *this; }

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
