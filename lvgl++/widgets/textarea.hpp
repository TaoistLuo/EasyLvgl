#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/style.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

/**
 * @brief LTextArea component (multi-line text input)
 * 
 * Wraps lv_textarea to provide a text input field with fluent API.
 * Supports single-line and multi-line modes, placeholder, password mode, etc.
 * 
 * @example
 * LTextArea ta;
 * ta.placeholder("Enter your name...")
 *   .one_line(true)
 *   .max_length(50)
 *   .on_change([](auto* e) {
 *       const char* text = lv_textarea_get_text(lv_event_get_target(e));
 *       printf("Text: %s\n", text);
 *   });
 */
class LTextArea : public LWidget {
public:
    /**
     * @brief Constructor
     * @param parent Parent object (nullptr = current screen)
     */
    explicit LTextArea(lv_obj_t* parent = nullptr)
        : LWidget(lv_textarea_create(parent ? parent : lv_scr_act())) {}

    /**
     * @brief Set text content
     * @param txt Text string
     * @return LTextArea& Reference for chaining
     */
    LTextArea& text(const char* txt) {
        lv_textarea_set_text(m_obj, txt);
        return *this;
    }

    /**
     * @brief Get text content
     * @return const char* Text string
     */
    const char* text() const {
        return lv_textarea_get_text(m_obj);
    }

    /**
     * @brief Set placeholder text
     * @param txt Placeholder string
     * @return LTextArea& Reference for chaining
     */
    LTextArea& placeholder(const char* txt) {
        lv_textarea_set_placeholder_text(m_obj, txt);
        return *this;
    }

    /**
     * @brief Set single-line mode
     * @param enable true = single line, false = multi-line
     * @return LTextArea& Reference for chaining
     */
    LTextArea& one_line(bool enable) {
        lv_textarea_set_one_line(m_obj, enable);
        return *this;
    }

    /**
     * @brief Set password mode (show dots instead of characters)
     * @param enable true = password mode, false = normal
     * @return LTextArea& Reference for chaining
     */
    LTextArea& password_mode(bool enable) {
        lv_textarea_set_password_mode(m_obj, enable);
        return *this;
    }

    /**
     * @brief Set maximum text length
     * @param len Maximum length (0 = unlimited)
     * @return LTextArea& Reference for chaining
     */
    LTextArea& max_length(uint32_t len) {
        lv_textarea_set_max_length(m_obj, len);
        return *this;
    }

    /**
     * @brief Add text at cursor position
     * @param txt Text to add
     * @return LTextArea& Reference for chaining
     */
    LTextArea& add_text(const char* txt) {
        lv_textarea_add_text(m_obj, txt);
        return *this;
    }

    /**
     * @brief Clear all text
     * @return LTextArea& Reference for chaining
     */
    LTextArea& clear() {
        lv_textarea_set_text(m_obj, "");
        return *this;
    }

    /**
     * @brief Set cursor position
     * @param pos Position (0 = start)
     * @return LTextArea& Reference for chaining
     */
    LTextArea& cursor_pos(uint32_t pos) {
        lv_textarea_set_cursor_pos(m_obj, pos);
        return *this;
    }

    /**
     * @brief Show/hide cursor
     * @param show true = show, false = hide
     * @return LTextArea& Reference for chaining
     */
    LTextArea& show_cursor(bool show) {
        if (show) {
            lv_textarea_set_cursor_click_pos(m_obj, true);
        } else {
            lv_textarea_set_cursor_click_pos(m_obj, false);
        }
        return *this;
    }

    /**
     * @brief Set text alignment
     * @param align Alignment (LV_TEXT_ALIGN_LEFT, CENTER, RIGHT)
     * @return LTextArea& Reference for chaining
     */
    LTextArea& text_align(lv_text_align_t align) {
        lv_obj_set_style_text_align(m_obj, align, 0);
        return *this;
    }

    /**
     * @brief Set size
     * @param w Width
     * @param h Height
     * @return LTextArea& Reference for chaining
     */
    LTextArea& size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    /**
     * @brief Set position
     * @param x X coordinate
     * @param y Y coordinate
     * @return LTextArea& Reference for chaining
     */
    LTextArea& pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    /**
     * @brief Register change event callback
     * @param callback Event callback function
     * @return LTextArea& Reference for chaining
     */
    template<typename Func>
    LTextArea& on_change(Func&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<Func>(callback));
        return *this;
    }

    /**
     * @brief Register value changed event callback
     * @param callback Event callback function
     * @return LTextArea& Reference for chaining
     */
    template<typename Func>
    LTextArea& on_value_changed(Func&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<Func>(callback));
        return *this;
    }

    /**
     * @brief Register ready event callback (Enter key pressed)
     * @param callback Event callback function
     * @return LTextArea& Reference for chaining
     */
    template<typename Func>
    LTextArea& on_ready(Func&& callback) {
        connect_event(m_obj, LV_EVENT_READY, std::forward<Func>(callback));
        return *this;
    }

    /**
     * @brief Enable/disable
     * @param enabled true = enabled, false = disabled
     * @return LTextArea& Reference for chaining
     */
    LTextArea& enabled(bool enabled) {
        if (enabled) {
            lv_obj_remove_state(m_obj, LV_STATE_DISABLED);
        } else {
            lv_obj_add_state(m_obj, LV_STATE_DISABLED);
        }
        return *this;
    }
};

} // namespace lvgl
