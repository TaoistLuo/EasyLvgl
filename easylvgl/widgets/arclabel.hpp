/**************************************************************************************************
 * @file        arclabel.hpp
 * @brief       LArclabel 组件封装（LVGL 9.6 lv_arclabel 原生能力）
 * @details     EasyLVGL —— C++ 封装 + 可视化设计器 + 代码生成。MIT License。
 *************************************************************************************************/

#pragma once
#include "easylvgl/core/widget.hpp"
#include "easylvgl/core/style.hpp"
#include "easylvgl/core/event.hpp"
#include "lvgl/lvgl.h"

namespace easylvgl {

class LArclabel : public LWidgetCRTP<LArclabel> {
public:
    using LWidgetCRTP<LArclabel>::set_size;

    explicit LArclabel(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_arclabel_create(parent ? parent : lv_scr_act())) {}

    LArclabel& set_text(const char* text) {
        lv_arclabel_set_text(m_obj, text);
        return *this;
    }

    LArclabel& set_angle_start(int start) {
        lv_arclabel_set_angle_start(m_obj, start);
        return *this;
    }

    LArclabel& set_angle_size(int size) {
        lv_arclabel_set_angle_size(m_obj, size);
        return *this;
    }

    LArclabel& set_offset(int offset) {
        lv_arclabel_set_offset(m_obj, offset);
        return *this;
    }

    LArclabel& set_dir(int dir) {
        lv_arclabel_set_dir(m_obj, static_cast<lv_arclabel_dir_t>(dir));
        return *this;
    }

    LArclabel& set_recolor(bool en) {
        lv_arclabel_set_recolor(m_obj, en);
        return *this;
    }

    LArclabel& set_radius(int radius) {
        lv_arclabel_set_radius(m_obj, static_cast<uint32_t>(radius));
        return *this;
    }

    LArclabel& set_center_offset_x(int x) {
        lv_arclabel_set_center_offset_x(m_obj, static_cast<uint32_t>(x));
        return *this;
    }

    LArclabel& set_center_offset_y(int y) {
        lv_arclabel_set_center_offset_y(m_obj, static_cast<uint32_t>(y));
        return *this;
    }

    LArclabel& set_text_vertical_align(int align) {
        lv_arclabel_set_text_vertical_align(m_obj, static_cast<lv_arclabel_text_align_t>(align));
        return *this;
    }

    LArclabel& set_text_horizontal_align(int align) {
        lv_arclabel_set_text_horizontal_align(m_obj, static_cast<lv_arclabel_text_align_t>(align));
        return *this;
    }

    LArclabel& set_overflow(int overflow) {
        lv_arclabel_set_overflow(m_obj, static_cast<lv_arclabel_overflow_t>(overflow));
        return *this;
    }

    LArclabel& set_end_overlap(bool overlap) {
        lv_arclabel_set_end_overlap(m_obj, overlap);
        return *this;
    }
};

} // namespace easylvgl
