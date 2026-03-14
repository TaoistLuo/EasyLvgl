#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/style.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

/**
 * @brief LDropdown component (select from list)
 * 
 * Wraps lv_dropdown to provide a dropdown selection list with fluent API.
 * 
 * @example
 * LDropdown dd;
 * dd.options("Option 1\nOption 2\nOption 3")
 *   .selected(0)
 *   .on_change([](auto* e) {
 *       uint16_t idx = lv_dropdown_get_selected(lv_event_get_target(e));
 *       printf("Selected: %d\n", idx);
 *   });
 */
class LDropdown : public LWidget {
public:
    /**
     * @brief Constructor
     * @param parent Parent object (nullptr = current screen)
     */
    explicit LDropdown(lv_obj_t* parent = nullptr)
        : LWidget(lv_dropdown_create(parent ? parent : lv_scr_act())) {}

    /**
     * @brief Set dropdown options
     * @param opts Options separated by newline (\n)
     * @return LDropdown& Reference for chaining
     */
    LDropdown& options(const char* opts) {
        lv_dropdown_set_options(m_obj, opts);
        return *this;
    }

    /**
     * @brief Add single option
     * @param opt Option text
     * @param pos Position to insert (LV_DROPDOWN_POS_LAST = end)
     * @return LDropdown& Reference for chaining
     */
    LDropdown& add_option(const char* opt, uint32_t pos = LV_DROPDOWN_POS_LAST) {
        lv_dropdown_add_option(m_obj, opt, pos);
        return *this;
    }

    /**
     * @brief Clear all options
     * @return LDropdown& Reference for chaining
     */
    LDropdown& clear_options() {
        lv_dropdown_clear_options(m_obj);
        return *this;
    }

    /**
     * @brief Set selected option by index
     * @param idx Option index (0-based)
     * @return LDropdown& Reference for chaining
     */
    LDropdown& selected(uint16_t idx) {
        lv_dropdown_set_selected(m_obj, idx);
        return *this;
    }

    /**
     * @brief Get selected option index
     * @return uint16_t Selected index
     */
    uint16_t selected() const {
        return lv_dropdown_get_selected(m_obj);
    }

    /**
     * @brief Get selected option text
     * @param buf Buffer to store text
     * @param buf_size Buffer size
     */
    void selected_text(char* buf, uint32_t buf_size) const {
        lv_dropdown_get_selected_str(m_obj, buf, buf_size);
    }

    /**
     * @brief Get option count
     * @return uint16_t Number of options
     */
    uint16_t option_count() const {
        return lv_dropdown_get_option_count(m_obj);
    }

    /**
     * @brief Set dropdown direction
     * @param dir Direction (LV_DIR_BOTTOM, LV_DIR_TOP, LV_DIR_LEFT, LV_DIR_RIGHT)
     * @return LDropdown& Reference for chaining
     */
    LDropdown& direction(lv_dir_t dir) {
        lv_dropdown_set_dir(m_obj, dir);
        return *this;
    }

    /**
     * @brief Set symbol (icon) on the button
     * @param symbol Symbol string (e.g., LV_SYMBOL_DOWN)
     * @return LDropdown& Reference for chaining
     */
    LDropdown& symbol(const char* symbol) {
        lv_dropdown_set_symbol(m_obj, symbol);
        return *this;
    }

    /**
     * @brief Open/close dropdown list
     * @param open true = open, false = close
     * @return LDropdown& Reference for chaining
     */
    LDropdown& open(bool open = true) {
        if (open) {
            lv_dropdown_open(m_obj);
        } else {
            lv_dropdown_close(m_obj);
        }
        return *this;
    }

    /**
     * @brief Set size
     * @param w Width
     * @param h Height
     * @return LDropdown& Reference for chaining
     */
    LDropdown& size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    /**
     * @brief Set position
     * @param x X coordinate
     * @param y Y coordinate
     * @return LDropdown& Reference for chaining
     */
    LDropdown& pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    /**
     * @brief Register change event callback
     * @param callback Event callback function
     * @return LDropdown& Reference for chaining
     */
    template<typename Func>
    LDropdown& on_change(Func&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<Func>(callback));
        return *this;
    }

    /**
     * @brief Enable/disable
     * @param enabled true = enabled, false = disabled
     * @return LDropdown& Reference for chaining
     */
    LDropdown& enabled(bool enabled) {
        if (enabled) {
            lv_obj_remove_state(m_obj, LV_STATE_DISABLED);
        } else {
            lv_obj_add_state(m_obj, LV_STATE_DISABLED);
        }
        return *this;
    }
};

} // namespace lvgl
