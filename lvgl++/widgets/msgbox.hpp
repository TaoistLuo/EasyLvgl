#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LMsgBox : public LWidget {
public:
    explicit LMsgBox(lv_obj_t* parent = nullptr)
        : LWidget(lv_msgbox_create(parent)) {}

    LMsgBox& title(const char* txt) {
        lv_msgbox_add_title(m_obj, txt);
        return *this;
    }

    LMsgBox& text(const char* txt) {
        lv_msgbox_add_text(m_obj, txt);
        return *this;
    }

    LMsgBox& add_close_button() {
        lv_msgbox_add_close_button(m_obj);
        return *this;
    }

    lv_obj_t* add_footer_button(const char* txt) {
        return lv_msgbox_add_footer_button(m_obj, txt);
    }

    template<typename Func>
    LMsgBox& on_value_changed(Func&& callback) {
        connect_event(m_obj, LV_EVENT_VALUE_CHANGED, std::forward<Func>(callback));
        return *this;
    }

    void close() {
        lv_msgbox_close(m_obj);
    }
};

} // namespace lvgl
