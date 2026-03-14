#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LCanvas : public LWidget {
public:
    explicit LCanvas(lv_obj_t* parent = nullptr)
        : LWidget(lv_canvas_create(parent ? parent : lv_scr_act())) {}

    LCanvas& buffer(void* buf, int32_t w, int32_t h, lv_color_format_t cf) {
        lv_canvas_set_buffer(m_obj, buf, w, h, cf);
        return *this;
    }

    LCanvas& px(int32_t x, int32_t y, uint32_t color, lv_opa_t opa = LV_OPA_COVER) {
        lv_canvas_set_px(m_obj, x, y, lv_color_hex(color), opa);
        return *this;
    }

    LCanvas& fill_bg(uint32_t color, lv_opa_t opa = LV_OPA_COVER) {
        lv_canvas_fill_bg(m_obj, lv_color_hex(color), opa);
        return *this;
    }
};

} // namespace lvgl
