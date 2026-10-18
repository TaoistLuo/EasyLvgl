#pragma once
#include "easylvgl/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace easylvgl {

class LAnimImage : public LWidgetCRTP<LAnimImage> {
public:
    explicit LAnimImage(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_animimg_create(parent ? parent : lv_scr_act())) {}

    LAnimImage& set_src(const void* dsc[], uint8_t cnt) {
        lv_animimg_set_src(m_obj, dsc, cnt);
        return *this;
    }

    LAnimImage& set_duration(uint32_t duration) {
        lv_animimg_set_duration(m_obj, duration);
        return *this;
    }

    LAnimImage& set_repeat_count(uint16_t count) {
        lv_animimg_set_repeat_count(m_obj, count);
        return *this;
    }

    LAnimImage& start() {
        lv_animimg_start(m_obj);
        return *this;
    }

    LAnimImage& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    LAnimImage& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
};

} // namespace easylvgl
