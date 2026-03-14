#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/style.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

/**
 * @brief Arc component (circular progress/slider)
 * 
 * Wraps lv_arc to provide a circular arc with adjustable range and value.
 * Can be used as progress indicator or circular slider.
 * 
 * @example
 * LArc arc;
 * arc.range(0, 100)
 *    .value(75)
 *    .bg_angles(0, 360)
 *    .rotation(270);
 */
class LArc : public LWidget {
public:
    /**
     * @brief Constructor
     * @param parent Parent object (nullptr = current screen)
     */
    explicit LArc(lv_obj_t* parent = nullptr)
        : LWidget(lv_arc_create(parent ? parent : lv_scr_act())) {}

    /**
     * @brief Set arc value
     * @param value Value within range
     * @return LArc& Reference for chaining
     */
    LArc& value(int16_t value) {
        lv_arc_set_value(m_obj, value);
        return *this;
    }

    /**
     * @brief Get arc value
     * @return int16_t Current value
     */
    int16_t value() const {
        return lv_arc_get_value(m_obj);
    }

    /**
     * @brief Set value range
     * @param min Minimum value
     * @param max Maximum value
     * @return LArc& Reference for chaining
     */
    LArc& range(int16_t min, int16_t max) {
        lv_arc_set_range(m_obj, min, max);
        return *this;
    }

    /**
     * @brief Set arc mode
     * @param mode Mode (LV_ARC_MODE_NORMAL, SYMMETRICAL, REVERSE)
     * @return LArc& Reference for chaining
     */
    LArc& mode(lv_arc_mode_t mode) {
        lv_arc_set_mode(m_obj, mode);
        return *this;
    }

    /**
     * @brief Set arc rotation offset
     * @param rotation Rotation angle in degrees (0-360)
     * @return LArc& Reference for chaining
     */
    LArc& rotation(uint16_t rotation) {
        lv_arc_set_rotation(m_obj, rotation);
        return *this;
    }

    /**
     * @brief Set background arc angles
     * @param start Start angle in degrees
     * @param end End angle in degrees
     * @return LArc& Reference for chaining
     */
    LArc& bg_angles(uint16_t start, uint16_t end) {
        lv_arc_set_bg_angles(m_obj, start, end);
        return *this;
    }

    /**
     * @brief Set foreground arc angles
     * @param start Start angle in degrees
     * @param end End angle in degrees
     * @return LArc& Reference for chaining
     */
    LArc& angles(uint16_t start, uint16_t end) {
        lv_arc_set_angles(m_obj, start, end);
        return *this;
    }

    /**
     * @brief Set arc change rate (for animation)
     * @param rate Change rate
     * @return LArc& Reference for chaining
     */
    LArc& change_rate(uint16_t rate) {
        lv_arc_set_change_rate(m_obj, rate);
        return *this;
    }

    /**
     * @brief Set size
     * @param size Size (width and height, arc is square)
     * @return LArc& Reference for chaining
     */
    LArc& size(int size) {
        lv_obj_set_size(m_obj, size, size);
        return *this;
    }

    /**
     * @brief Set position
     * @param x X coordinate
     * @param y Y coordinate
     * @return LArc& Reference for chaining
     */
    LArc& pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    /**
     * @brief Set alignment
     * @param align Alignment mode
     * @return LArc& Reference for chaining
     */
    LArc& align(lv_align_t align) {
        lv_obj_set_align(m_obj, align);
        return *this;
    }

    /**
     * @brief Register value changed event callback
     * @param callback Event callback function
     * @return LArc& Reference for chaining
     */
    template<typename Func>
    LArc& on_change(Func&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<Func>(callback));
        return *this;
    }

    /**
     * @brief Enable/disable knob (interactive mode)
     * @param enable true = enable knob, false = disable (indicator only)
     * @return LArc& Reference for chaining
     */
    LArc& knob_enabled(bool enable) {
        if (!enable) {
            lv_obj_remove_flag(m_obj, LV_OBJ_FLAG_CLICKABLE);
        } else {
            lv_obj_add_flag(m_obj, LV_OBJ_FLAG_CLICKABLE);
        }
        return *this;
    }

    /**
     * @brief Set arc colors
     * @param bg_color Background arc color
     * @param fg_color Foreground arc color
     * @return LArc& Reference for chaining
     */
    LArc& colors(uint32_t bg_color, uint32_t fg_color) {
        LStyle bg_style;
        bg_style.bg_color(bg_color).apply_to(m_obj, LV_PART_MAIN);
        
        LStyle fg_style;
        fg_style.bg_color(fg_color).apply_to(m_obj, LV_PART_INDICATOR);
        
        return *this;
    }
};

} // namespace lvgl
