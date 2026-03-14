#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/style.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

/**
 * @brief LImage component
 * 
 * Wraps lv_image to display images from various sources.
 * Supports file paths, C arrays, symbols, and runtime buffers.
 * 
 * @example
 * LImage img;
 * img.src("path/to/image.png")
 *   .size(100, 100)
 *   .align(LV_ALIGN_CENTER);
 */
class LImage : public LWidget {
public:
    /**
     * @brief Constructor
     * @param parent Parent object (nullptr = current screen)
     */
    explicit LImage(lv_obj_t* parent = nullptr)
        : LWidget(lv_image_create(parent ? parent : lv_scr_act())) {}

    /**
     * @brief Set image source
     * @param src LImage source (file path, C array, or symbol)
     * @return LImage& Reference for chaining
     */
    LImage& src(const void* src) {
        lv_image_set_src(m_obj, src);
        return *this;
    }

    /**
     * @brief Set image source from file path
     * @param path File path (e.g., "A:/image.png")
     * @return LImage& Reference for chaining
     */
    LImage& src(const char* path) {
        lv_image_set_src(m_obj, path);
        return *this;
    }

    /**
     * @brief Set image offset
     * @param x X offset
     * @param y Y offset
     * @return LImage& Reference for chaining
     */
    LImage& offset(int x, int y) {
        lv_image_set_offset_x(m_obj, x);
        lv_image_set_offset_y(m_obj, y);
        return *this;
    }

    /**
     * @brief Set image rotation angle
     * @param angle Angle in 0.1 degree units (e.g., 450 = 45 degrees)
     * @return LImage& Reference for chaining
     */
    LImage& rotation(int16_t angle) {
        lv_image_set_rotation(m_obj, angle);
        return *this;
    }

    /**
     * @brief Set image pivot point for rotation
     * @param x Pivot X coordinate
     * @param y Pivot Y coordinate
     * @return LImage& Reference for chaining
     */
    LImage& pivot(int x, int y) {
        lv_image_set_pivot(m_obj, x, y);
        return *this;
    }

    /**
     * @brief Set image scale (zoom)
     * @param zoom Scale factor (256 = 100%, 512 = 200%, 128 = 50%)
     * @return LImage& Reference for chaining
     */
    LImage& scale(uint16_t zoom) {
        lv_image_set_scale(m_obj, zoom);
        return *this;
    }

    /**
     * @brief Set image scale with separate X and Y
     * @param zoom_x X scale factor
     * @param zoom_y Y scale factor
     * @return LImage& Reference for chaining
     */
    LImage& scale(uint16_t zoom_x, uint16_t zoom_y) {
        lv_image_set_scale_x(m_obj, zoom_x);
        lv_image_set_scale_y(m_obj, zoom_y);
        return *this;
    }

    /**
     * @brief Set anti-aliasing
     * @param enable true = enable, false = disable
     * @return LImage& Reference for chaining
     */
    LImage& antialias(bool enable) {
        lv_obj_set_style_image_opa(m_obj, enable ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
        return *this;
    }

    /**
     * @brief Set image opacity
     * @param opa Opacity (0-255, 0 = transparent, 255 = opaque)
     * @return LImage& Reference for chaining
     */
    LImage& opacity(uint8_t opa) {
        lv_obj_set_style_image_opa(m_obj, opa, 0);
        return *this;
    }

    /**
     * @brief Set image recolor
     * @param color Color to blend with image
     * @param intensity Intensity (0-255, 0 = no effect, 255 = full recolor)
     * @return LImage& Reference for chaining
     */
    LImage& recolor(uint32_t color, uint8_t intensity) {
        lv_obj_set_style_image_recolor(m_obj, lv_color_hex(color), 0);
        lv_obj_set_style_image_recolor_opa(m_obj, intensity, 0);
        return *this;
    }

    /**
     * @brief Set size mode
     * @param w Width (LV_SIZE_CONTENT = auto)
     * @param h Height (LV_SIZE_CONTENT = auto)
     * @return LImage& Reference for chaining
     */
    LImage& size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    /**
     * @brief Set position
     * @param x X coordinate
     * @param y Y coordinate
     * @return LImage& Reference for chaining
     */
    LImage& pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    /**
     * @brief Set alignment
     * @param align Alignment mode
     * @return LImage& Reference for chaining
     */
    LImage& align(lv_align_t align) {
        lv_obj_set_align(m_obj, align);
        return *this;
    }

    /**
     * @brief Register click event callback
     * @param callback Event callback function
     * @return LImage& Reference for chaining
     */
    template<typename Func>
    LImage& on_click(Func&& callback) {
        lv_obj_add_flag(m_obj, LV_OBJ_FLAG_CLICKABLE);
        connect_event(m_obj, LV_EVENT_CLICKED, std::forward<Func>(callback));
        return *this;
    }
};

} // namespace lvgl
