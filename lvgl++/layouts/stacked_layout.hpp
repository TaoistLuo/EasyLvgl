#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

// ============================================================================
// LStackedLayout — Stacked layout (Qt: QStackedLayout)
// Shows exactly one child at a time. All others are hidden.
// ============================================================================
class LStackedLayout : public LWidgetCRTP<LStackedLayout> {
public:
    explicit LStackedLayout(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_obj_create(parent ? parent : lv_scr_act())) {
        lv_obj_set_flex_flow(m_obj, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(m_obj,
            LV_FLEX_ALIGN_START,
            LV_FLEX_ALIGN_CENTER,
            LV_FLEX_ALIGN_START
        );
    }

    int add(lv_obj_t* child) {
        if (child) {
            lv_obj_set_parent(child, m_obj);
            int idx = (int)m_count;
            m_count++;

            // Hide all except first
            if (idx > 0) {
                lv_obj_add_flag(child, LV_OBJ_FLAG_HIDDEN);
            }

            return idx;
        }
        return -1;
    }

    LStackedLayout& set_current(int index) {
        if (index < 0 || index >= m_count) return *this;
        m_current = index;

        int i = 0;
        uint32_t child_cnt = lv_obj_get_child_count(m_obj);
        for (uint32_t c = 0; c < child_cnt; c++) {
            lv_obj_t* child = lv_obj_get_child(m_obj, c);
            if (child) {
                if (i == index) {
                    lv_obj_remove_flag(child, LV_OBJ_FLAG_HIDDEN);
                } else {
                    lv_obj_add_flag(child, LV_OBJ_FLAG_HIDDEN);
                }
                i++;
            }
        }
        return *this;
    }

    int current() const { return m_current; }
    int count() const { return m_count; }

    LStackedLayout& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LStackedLayout& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }

    LStackedLayout& set_width(int w) {
        lv_obj_set_width(m_obj, w);
        return *this;
    }

    LStackedLayout& set_height(int h) {
        lv_obj_set_height(m_obj, h);
        return *this;
    }

    LStackedLayout& set_padding(int pad) {
        lv_obj_set_style_pad_all(m_obj, pad, 0);
        return *this;
    }

private:
    int m_current = 0;
    int m_count = 0;
};

} // namespace lvgl
