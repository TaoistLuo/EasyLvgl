#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LButtonMatrix : public LWidget {
public:
    explicit LButtonMatrix(lv_obj_t* parent = nullptr)
        : LWidget(lv_buttonmatrix_create(parent ? parent : lv_scr_act())) {}

    LButtonMatrix& map(const char** map) {
        lv_buttonmatrix_set_map(m_obj, map);
        return *this;
    }

    LButtonMatrix& selected(uint16_t btn_id) {
        lv_buttonmatrix_set_selected_button(m_obj, btn_id);
        return *this;
    }

    uint16_t selected() const {
        return lv_buttonmatrix_get_selected_button(m_obj);
    }

    LButtonMatrix& one_checked(bool en) {
        lv_buttonmatrix_set_one_checked(m_obj, en);
        return *this;
    }

    LButtonMatrix& size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    template<typename Func>
    LButtonMatrix& on_click(Func&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<Func>(callback));
        return *this;
    }

    template<typename Func>
    LButtonMatrix& on_value_changed(Func&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<Func>(callback));
        return *this;
    }
};

} // namespace lvgl
