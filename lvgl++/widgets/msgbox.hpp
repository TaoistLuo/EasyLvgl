#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LMsgBox : public LWidgetCRTP<LMsgBox> {
public:
    explicit LMsgBox(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_msgbox_create(parent)) {}

    LMsgBox& set_title(const char* txt) {
        lv_msgbox_add_title(m_obj, txt);
        return *this;
    }

    LMsgBox& set_text(const char* txt) {
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
        if (m_obj) {
            lv_msgbox_close(m_obj);
            m_obj = nullptr;
        }
    }

    ~LMsgBox() override {
        if (m_obj) {
            lv_msgbox_close(m_obj);
            m_obj = nullptr;
        }
    }

};

} // namespace lvgl
