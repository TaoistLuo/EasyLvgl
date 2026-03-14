#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LAnimImage : public LWidget {
public:
    explicit LAnimImage(lv_obj_t* parent = nullptr)
        : LWidget(lv_animimg_create(parent ? parent : lv_scr_act())) {}

    LAnimImage& src(const void* dsc[], uint8_t cnt) {
        lv_animimg_set_src(m_obj, dsc, cnt);
        return *this;
    }

    LAnimImage& duration(uint32_t duration) {
        lv_animimg_set_duration(m_obj, duration);
        return *this;
    }

    LAnimImage& repeat_count(uint16_t count) {
        lv_animimg_set_repeat_count(m_obj, count);
        return *this;
    }

    LAnimImage& start() {
        lv_animimg_start(m_obj);
        return *this;
    }
};

} // namespace lvgl
