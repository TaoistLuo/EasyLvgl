/**************************************************************************************************
 * @file        qrcode.hpp
 * @brief       LQrcode 组件封装（LVGL 9.6 lv_qrcode 原生能力）
 * @details     EasyLVGL —— C++ 封装 + 可视化设计器 + 代码生成。MIT License。
 *************************************************************************************************/

#pragma once
#include "easylvgl/core/widget.hpp"
#include "easylvgl/core/style.hpp"
#include "easylvgl/core/event.hpp"
#include "lvgl/lvgl.h"

namespace easylvgl {

class LQrcode : public LWidgetCRTP<LQrcode> {
public:
    using LWidgetCRTP<LQrcode>::set_size;

    explicit LQrcode(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_qrcode_create(parent ? parent : lv_scr_act())) {}

    LQrcode& set_data(const char* data) {
        lv_qrcode_set_data(m_obj, data);
        return *this;
    }

    LQrcode& set_dark_color(uint32_t color) {
        lv_qrcode_set_dark_color(m_obj, lv_color_hex(color));
        return *this;
    }

    LQrcode& set_light_color(uint32_t color) {
        lv_qrcode_set_light_color(m_obj, lv_color_hex(color));
        return *this;
    }

    LQrcode& set_quiet_zone(bool enable) {
        lv_qrcode_set_quiet_zone(m_obj, enable);
        return *this;
    }

    LQrcode& set_update_mode(int mode) {
        lv_qrcode_set_update_mode(m_obj, static_cast<lv_qrcode_update_mode_t>(mode));
        return *this;
    }
};

} // namespace easylvgl
