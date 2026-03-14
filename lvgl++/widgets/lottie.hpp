#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

/**
 * @brief LLottie component (Lottie animation player)
 * 
 * Wraps lv_lottie to provide Lottie animation playback with fluent API.
 * Requires LVGL to be compiled with LV_USE_LOTTIE enabled.
 * 
 * @example
 * LLottie lottie;
 * lottie.src("animation.json")
 *       .size(200, 200)
 *       .loop(true);
 */
class LLottie : public LWidget {
public:
    /**
     * @brief Constructor
     * @param parent Parent object (nullptr = current screen)
     */
    explicit LLottie(lv_obj_t* parent = nullptr)
        : LWidget(lv_lottie_create(parent ? parent : lv_scr_act())) {}

    /**
     * @brief Set animation source (file path or data)
     * @param src Animation source
     * @return LLottie& Reference for chaining
     */
    LLottie& src(const char* src) {
        lv_lottie_set_src_file(m_obj, src);
        return *this;
    }

    /**
     * @brief Set animation data from buffer
     * @param data Animation data buffer
     * @param size Data size
     * @return LLottie& Reference for chaining
     */
    LLottie& src_data(const void* data, size_t size) {
        lv_lottie_set_src_data(m_obj, data, size);
        return *this;
    }

    /**
     * @brief Set loop mode
     * @param loop true = loop, false = play once
     * @return LLottie& Reference for chaining
     */
    LLottie& loop(bool loop) {
        lv_lottie_set_loop(m_obj, loop);
        return *this;
    }

    /**
     * @brief Set playback speed
     * @param speed Speed multiplier (1.0 = normal)
     * @return LLottie& Reference for chaining
     */
    LLottie& speed(float speed) {
        lv_lottie_set_speed(m_obj, speed);
        return *this;
    }

    /**
     * @brief Play animation
     * @return LLottie& Reference for chaining
     */
    LLottie& play() {
        lv_lottie_play(m_obj);
        return *this;
    }

    /**
     * @brief Pause animation
     * @return LLottie& Reference for chaining
     */
    LLottie& pause() {
        lv_lottie_pause(m_obj);
        return *this;
    }

    /**
     * @brief Stop animation
     * @return LLottie& Reference for chaining
     */
    LLottie& stop() {
        lv_lottie_stop(m_obj);
        return *this;
    }

    /**
     * @brief Set size
     * @param w Width
     * @param h Height
     * @return LLottie& Reference for chaining
     */
    LLottie& size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    /**
     * @brief Set position
     * @param x X coordinate
     * @param y Y coordinate
     * @return LLottie& Reference for chaining
     */
    LLottie& pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    /**
     * @brief Register complete event callback
     * @param callback Event callback function
     * @return LLottie& Reference for chaining
     */
    template<typename Func>
    LLottie& on_complete(Func&& callback) {
        connect_event(m_obj, LV_EVENT_READY, std::forward<Func>(callback));
        return *this;
    }
};

} // namespace lvgl
