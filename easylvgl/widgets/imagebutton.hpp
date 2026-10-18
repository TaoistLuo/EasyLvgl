#pragma once
#include "easylvgl/core/widget.hpp"
#include "easylvgl/core/event.hpp"
#include "lvgl/lvgl.h"

namespace easylvgl {

class LImageButton : public LWidgetCRTP<LImageButton> {
public:
    explicit LImageButton(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_imagebutton_create(parent ? parent : lv_scr_act())) {}

    LImageButton& set_src(lv_imagebutton_state_t state, const void* src) {
        lv_imagebutton_set_src(m_obj, state, nullptr, src, nullptr);
        return *this;
    }

    LImageButton& set_state(lv_imagebutton_state_t state) {
        lv_imagebutton_set_state(m_obj, state);
        return *this;
    }

    LImageButton& set_state(int state) {
        return set_state(static_cast<lv_imagebutton_state_t>(state));
    }

    LImageButton& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    template<typename Func>
    LImageButton& on_click(Func&& callback) {
        connect_event(m_obj, LV_EVENT_CLICKED, std::forward<Func>(callback));
        return *this;
    }
};

} // namespace easylvgl
