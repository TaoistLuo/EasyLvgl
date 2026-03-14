#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LSpan : public LWidget {
public:
    explicit LSpan(lv_obj_t* parent = nullptr)
        : LWidget(lv_spangroup_create(parent ? parent : lv_scr_act())) {}

    lv_span_t* new_span() {
        return lv_spangroup_new_span(m_obj);
    }

    LSpan& align(lv_text_align_t align) {
        lv_spangroup_set_align(m_obj, align);
        return *this;
    }

    LSpan& overflow(lv_span_overflow_t overflow) {
        lv_spangroup_set_overflow(m_obj, overflow);
        return *this;
    }

    LSpan& indent(int32_t indent) {
        lv_spangroup_set_indent(m_obj, indent);
        return *this;
    }

    LSpan& mode(lv_span_mode_t mode) {
        lv_spangroup_set_mode(m_obj, mode);
        return *this;
    }
};

} // namespace lvgl
