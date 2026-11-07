/**************************************************************************************************
 * @file        barcode.hpp
 * @brief       LBarcode 组件封装（LVGL 9.6 lv_barcode 原生能力）
 * @details     EasyLVGL —— C++ 封装 + 可视化设计器 + 代码生成。MIT License。
 *************************************************************************************************/

#pragma once
#include "easylvgl/core/widget.hpp"
#include "easylvgl/core/style.hpp"
#include "easylvgl/core/event.hpp"
#include "lvgl/lvgl.h"

namespace easylvgl {

class LBarcode : public LWidgetCRTP<LBarcode> {
public:
    using LWidgetCRTP<LBarcode>::set_size;

    explicit LBarcode(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_barcode_create(parent ? parent : lv_scr_act())) {}

    LBarcode& set_data(const char* data) {
        lv_barcode_update(m_obj, data);
        return *this;
    }

    LBarcode& set_dark_color(uint32_t color) {
        lv_barcode_set_dark_color(m_obj, lv_color_hex(color));
        return *this;
    }

    LBarcode& set_light_color(uint32_t color) {
        lv_barcode_set_light_color(m_obj, lv_color_hex(color));
        return *this;
    }

    LBarcode& set_scale(int scale) {
        lv_barcode_set_scale(m_obj, static_cast<uint16_t>(scale));
        return *this;
    }

    LBarcode& set_direction(int dir) {
        lv_barcode_set_direction(m_obj, static_cast<lv_dir_t>(dir));
        return *this;
    }

    LBarcode& set_tiled(bool tiled) {
        lv_barcode_set_tiled(m_obj, tiled);
        return *this;
    }

    LBarcode& set_encoding(int encoding) {
        lv_barcode_set_encoding(m_obj, static_cast<lv_barcode_encoding_t>(encoding));
        return *this;
    }

    LBarcode& set_update_mode(int mode) {
        lv_barcode_set_update_mode(m_obj, static_cast<lv_barcode_update_mode_t>(mode));
        return *this;
    }
};

} // namespace easylvgl
