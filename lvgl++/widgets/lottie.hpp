#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LLottie : public LWidgetCRTP<LLottie> {
public:
    using LWidgetCRTP<LLottie>::set_align;

    explicit LLottie(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_lottie_create(parent ? parent : lv_scr_act())) {}

    LLottie& set_src(const char* src) {
        lv_lottie_set_src_file(m_obj, src);
        return *this;
    }

    LLottie& set_src_data(const void* data, size_t size) {
        lv_lottie_set_src_data(m_obj, data, size);
        return *this;
    }

    LLottie& set_loop(bool loop) {
        lv_lottie_set_loop(m_obj, loop);
        return *this;
    }

    LLottie& set_speed(float speed) {
        lv_lottie_set_speed(m_obj, speed);
        return *this;
    }

    LLottie& play() {
        lv_lottie_play(m_obj);
        return *this;
    }

    LLottie& pause() {
        lv_lottie_pause(m_obj);
        return *this;
    }

    LLottie& stop() {
        lv_lottie_stop(m_obj);
        return *this;
    }

    template<typename Func>
    LLottie& on_complete(Func&& callback) {
        connect_event(m_obj, LV_EVENT_READY, std::forward<Func>(callback));
        return *this;
    }
};

} // namespace lvgl
