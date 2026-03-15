#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/style.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LImage : public LWidgetCRTP<LImage> {
public:
    explicit LImage(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_image_create(parent ? parent : lv_scr_act())) {}

    LImage& set_src(const void* src) {
        lv_image_set_src(m_obj, src);
        return *this;
    }

    LImage& set_src(const char* path) {
        lv_image_set_src(m_obj, path);
        return *this;
    }

    LImage& set_offset(int x, int y) {
        lv_image_set_offset_x(m_obj, x);
        lv_image_set_offset_y(m_obj, y);
        return *this;
    }

    LImage& set_rotation(int16_t angle) {
        lv_image_set_rotation(m_obj, angle);
        return *this;
    }

    LImage& set_pivot(int x, int y) {
        lv_image_set_pivot(m_obj, x, y);
        return *this;
    }

    LImage& set_scale(uint16_t zoom) {
        lv_image_set_scale(m_obj, zoom);
        return *this;
    }

    LImage& set_scale(uint16_t zoom_x, uint16_t zoom_y) {
        lv_image_set_scale_x(m_obj, zoom_x);
        lv_image_set_scale_y(m_obj, zoom_y);
        return *this;
    }

    LImage& set_antialias(bool enable) {
        lv_obj_set_style_image_opa(m_obj, enable ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
        return *this;
    }

    LImage& set_opacity(uint8_t opa) {
        lv_obj_set_style_image_opa(m_obj, opa, 0);
        return *this;
    }

    LImage& set_recolor(uint32_t color, uint8_t intensity) {
        lv_obj_set_style_image_recolor(m_obj, lv_color_hex(color), 0);
        lv_obj_set_style_image_recolor_opa(m_obj, intensity, 0);
        return *this;
    }

    LImage& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LImage& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    LImage& set_align(lv_align_t align) {
        lv_obj_set_align(m_obj, align);
        return *this;
    }

    template<typename Func>
    LImage& on_click(Func&& callback) {
        lv_obj_add_flag(m_obj, LV_OBJ_FLAG_CLICKABLE);
        connect_event(m_obj, LV_EVENT_CLICKED, std::forward<Func>(callback));
        return *this;
    }
};

} // namespace lvgl
