#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/style.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

/**
 * @brief Bar component (progress bar)
 * 
 * Wraps lv_bar to provide a horizontal or vertical progress bar.
 * 
 * @example
 * LBar bar;
 * bar.range(0, 100)
 *    .value(75)
 *    .size(200, 20);
 */
class LBar : public LWidget {
public:
    /**
     * @brief Constructor
     * @param parent Parent object (nullptr = current screen)
     */
    explicit LBar(lv_obj_t* parent = nullptr)
        : LWidget(lv_bar_create(parent ? parent : lv_scr_act())) {}

    /**
     * @brief Set bar value
     * @param value Value within range
     * @param anim Enable animation (LV_ANIM_ON/OFF)
     * @return LBar& Reference for chaining
     */
    LBar& value(int32_t value, lv_anim_enable_t anim = LV_ANIM_OFF) {
        lv_bar_set_value(m_obj, value, anim);
        return *this;
    }

    /**
     * @brief Get bar value
     * @return int32_t Current value
     */
    int32_t value() const {
        return lv_bar_get_value(m_obj);
    }

    /**
     * @brief Set start value (for range bar)
     * @param start_value Start value
     * @param anim Enable animation
     * @return LBar& Reference for chaining
     */
    LBar& start_value(int32_t start_value, lv_anim_enable_t anim = LV_ANIM_OFF) {
        lv_bar_set_start_value(m_obj, start_value, anim);
        return *this;
    }

    /**
     * @brief Set value range
     * @param min Minimum value
     * @param max Maximum value
     * @return LBar& Reference for chaining
     */
    LBar& range(int32_t min, int32_t max) {
        lv_bar_set_range(m_obj, min, max);
        return *this;
    }

    /**
     * @brief Set bar mode
     * @param mode Mode (LV_BAR_MODE_NORMAL, SYMMETRICAL, RANGE)
     * @return LBar& Reference for chaining
     */
    LBar& mode(lv_bar_mode_t mode) {
        lv_bar_set_mode(m_obj, mode);
        return *this;
    }

    /**
     * @brief Set bar orientation
     * @param horizontal true = horizontal, false = vertical
     * @return LBar& Reference for chaining
     */
    LBar& orientation(bool horizontal) {
        if (horizontal) {
            lv_obj_set_size(m_obj, 200, 20);
        } else {
            lv_obj_set_size(m_obj, 20, 200);
        }
        return *this;
    }

    /**
     * @brief Set size
     * @param w Width
     * @param h Height
     * @return LBar& Reference for chaining
     */
    LBar& size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    /**
     * @brief Set position
     * @param x X coordinate
     * @param y Y coordinate
     * @return LBar& Reference for chaining
     */
    LBar& pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    /**
     * @brief Set alignment
     * @param align Alignment mode
     * @return LBar& Reference for chaining
     */
    LBar& align(lv_align_t align) {
        lv_obj_set_align(m_obj, align);
        return *this;
    }

    /**
     * @brief Set bar colors
     * @param bg_color Background color
     * @param indicator_color Indicator (progress) color
     * @return LBar& Reference for chaining
     */
    LBar& colors(uint32_t bg_color, uint32_t indicator_color) {
        LStyle bg_style;
        bg_style.bg_color(bg_color).apply_to(m_obj, LV_PART_MAIN);
        
        LStyle ind_style;
        ind_style.bg_color(indicator_color).apply_to(m_obj, LV_PART_INDICATOR);
        
        return *this;
    }

    /**
     * @brief Set as indeterminate (loading) bar
     * @param enable true = indeterminate mode
     * @return LBar& Reference for chaining
     */
    LBar& indeterminate(bool enable) {
        if (enable) {
            lv_bar_set_mode(m_obj, LV_BAR_MODE_NORMAL);
            // Start animation for indeterminate effect
            lv_bar_set_value(m_obj, 0, LV_ANIM_ON);
        }
        return *this;
    }
};

} // namespace lvgl
