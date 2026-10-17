#pragma once

/**
 * @file label.hpp
 * @brief 标签组件
 * @version 2.0.0
 * @date 2026-03-15
 */

#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/style.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

/**
 * @class LLabel
 * @brief 标签组件，封装lv_label
 */
class LLabel : public LWidgetCRTP<LLabel> {
public:
    explicit LLabel(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_label_create(parent ? parent : lv_scr_act())) {}

    LLabel& set_text(const char* txt) {
        lv_label_set_text(m_obj, txt);
        return *this;
    }
    
    template<typename... Args>
    LLabel& set_text_fmt(const char* fmt, Args&&... args) {
        lv_label_set_text_fmt(m_obj, fmt, std::forward<Args>(args)...);
        return *this;
    }
    
    LLabel& set_color(uint32_t hex) {
        lv_obj_set_style_text_color(m_obj, lv_color_hex(hex), 0);
        return *this;
    }
    
    LLabel& set_font_size(int size) {
        const lv_font_t* font = nullptr;
        switch(size) {
#if LV_FONT_MONTSERRAT_12
            case 12: font = &lv_font_montserrat_12; break;
#endif
#if LV_FONT_MONTSERRAT_14
            case 14: font = &lv_font_montserrat_14; break;
#endif
#if LV_FONT_MONTSERRAT_16
            case 16: font = &lv_font_montserrat_16; break;
#endif
#if LV_FONT_MONTSERRAT_18
            case 18: font = &lv_font_montserrat_18; break;
#endif
#if LV_FONT_MONTSERRAT_20
            case 20: font = &lv_font_montserrat_20; break;
#endif
#if LV_FONT_MONTSERRAT_24
            case 24: font = &lv_font_montserrat_24; break;
#endif
            default: font = LV_FONT_DEFAULT; break;
        }
        if (font) {
            lv_obj_set_style_text_font(m_obj, font, 0);
        }
        return *this;
    }
    
    LLabel& set_long_mode(lv_label_long_mode_t mode) {
        lv_label_set_long_mode(m_obj, mode);
        return *this;
    }

    LLabel& set_long_mode(int mode) {
        return set_long_mode(static_cast<lv_label_long_mode_t>(mode));
    }

    LLabel& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LLabel& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
};

} // namespace lvgl
