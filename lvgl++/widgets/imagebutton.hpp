#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LImageButton : public LWidget {
public:
    explicit LImageButton(lv_obj_t* parent = nullptr)
        : LWidget(lv_imagebutton_create(parent ? parent : lv_scr_act())) {}

    LImageButton& src(lv_imagebutton_state_t state, const void* src) {
        lv_imagebutton_set_src(m_obj, state, nullptr, src, nullptr);
        return *this;
    }

    LImageButton& state(lv_imagebutton_state_t state) {
        lv_imagebutton_set_state(m_obj, state);
        return *this;
    }

    template<typename Func>
    LImageButton& on_click(Func&& callback) {
        connect_event(m_obj, LV_EVENT_CLICKED, std::forward<Func>(callback));
        return *this;
    }
};

} // namespace lvgl
