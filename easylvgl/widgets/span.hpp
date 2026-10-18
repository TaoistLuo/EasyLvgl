#pragma once
#include "easylvgl/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace easylvgl {

class LSpan : public LWidgetCRTP<LSpan> {
public:
    using LWidgetCRTP<LSpan>::set_align;

    explicit LSpan(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_spangroup_create(parent ? parent : lv_scr_act())) {}

    lv_span_t* new_span() {
        return lv_spangroup_new_span(m_obj);
    }

    LSpan& set_align(lv_text_align_t align) {
        lv_spangroup_set_align(m_obj, align);
        return *this;
    }

    LSpan& set_overflow(lv_span_overflow_t overflow) {
        lv_spangroup_set_overflow(m_obj, overflow);
        return *this;
    }

    LSpan& set_indent(int32_t indent) {
        lv_spangroup_set_indent(m_obj, indent);
        return *this;
    }

    LSpan& set_mode(lv_span_mode_t mode) {
        lv_spangroup_set_mode(m_obj, mode);
        return *this;
    }

    LSpan& set_mode(int mode) {
        return set_mode(static_cast<lv_span_mode_t>(mode));
    }
    LSpan& set_overflow(int overflow) { lv_spangroup_set_overflow(m_obj, static_cast<lv_span_overflow_t>(overflow)); return *this; }

};

} // namespace easylvgl
