#pragma once
#include "easylvgl/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace easylvgl {

// ============================================================================
// LVBoxLayout — Vertical box layout (Qt: QVBoxLayout)
// Arranges children top-to-bottom in a column.
// ============================================================================
class LVBoxLayout : public LWidgetCRTP<LVBoxLayout> {
public:
    explicit LVBoxLayout(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_obj_create(parent ? parent : lv_scr_act())) {
        lv_obj_set_flex_flow(m_obj, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(m_obj,
            LV_FLEX_ALIGN_START,
            LV_FLEX_ALIGN_CENTER,
            LV_FLEX_ALIGN_START
        );
    }

    int add_widget(lv_obj_t* child) {
        return insert_widget(count(), child);
    }

    int insert_widget(int index, lv_obj_t* child) {
        if (!child) return -1;
        const int child_count = count();
        if (index < 0) index = 0;
        if (index > child_count) index = child_count;

        lv_obj_set_parent(child, m_obj);
        lv_obj_move_to_index(child, static_cast<int32_t>(index));
        return index;
    }

    bool remove_widget(lv_obj_t* child) {
        if (!child || lv_obj_get_parent(child) != m_obj) return false;
        lv_obj_delete(child);
        return true;
    }

    int count() const {
        return static_cast<int>(lv_obj_get_child_count(m_obj));
    }

    int index_of(lv_obj_t* child) const {
        if (!child || lv_obj_get_parent(child) != m_obj) return -1;
        return static_cast<int>(lv_obj_get_index(child));
    }

    lv_obj_t* widget_at(int index) const {
        if (index < 0 || index >= count()) return nullptr;
        return lv_obj_get_child(m_obj, static_cast<int32_t>(index));
    }

    LVBoxLayout& set_spacing(int gap) {
        lv_obj_set_style_pad_row(m_obj, gap, 0);
        return *this;
    }

    LVBoxLayout& set_padding(int pad) {
        lv_obj_set_style_pad_all(m_obj, pad, 0);
        return *this;
    }

    LVBoxLayout& set_padding(int top, int bottom, int left, int right) {
        lv_obj_set_style_pad_top(m_obj, top, 0);
        lv_obj_set_style_pad_bottom(m_obj, bottom, 0);
        lv_obj_set_style_pad_left(m_obj, left, 0);
        lv_obj_set_style_pad_right(m_obj, right, 0);
        return *this;
    }

    LVBoxLayout& set_align_main(lv_flex_align_t align) {
        lv_obj_set_flex_align(m_obj, align,
            lv_obj_get_style_flex_cross_place(m_obj, 0),
            lv_obj_get_style_flex_track_place(m_obj, 0));
        return *this;
    }

    LVBoxLayout& set_align_main(int align) {
        return set_align_main(static_cast<lv_flex_align_t>(align));
    }

    LVBoxLayout& set_align_cross(lv_flex_align_t align) {
        lv_obj_set_flex_align(m_obj,
            lv_obj_get_style_flex_main_place(m_obj, 0),
            align,
            lv_obj_get_style_flex_track_place(m_obj, 0));
        return *this;
    }

    LVBoxLayout& set_align_cross(int align) {
        return set_align_cross(static_cast<lv_flex_align_t>(align));
    }

    LVBoxLayout& set_align_track(lv_flex_align_t align) {
        lv_obj_set_flex_align(m_obj,
            lv_obj_get_style_flex_main_place(m_obj, 0),
            lv_obj_get_style_flex_cross_place(m_obj, 0),
            align);
        return *this;
    }

    LVBoxLayout& set_align_track(int align) {
        return set_align_track(static_cast<lv_flex_align_t>(align));
    }

    LVBoxLayout& set_align(lv_flex_align_t main_align, lv_flex_align_t cross_align, lv_flex_align_t track_align = LV_FLEX_ALIGN_START) {
        lv_obj_set_flex_align(m_obj, main_align, cross_align, track_align);
        return *this;
    }

    LVBoxLayout& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LVBoxLayout& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    LVBoxLayout& set_align(lv_align_t align_value) {
        lv_obj_set_align(m_obj, align_value);
        return *this;
    }

    LVBoxLayout& set_growth() {
        lv_obj_set_flex_grow(m_obj, 1);
        return *this;
    }
};

} // namespace easylvgl
