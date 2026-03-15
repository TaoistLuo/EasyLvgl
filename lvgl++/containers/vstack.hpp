#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LVStack : public LWidgetCRTP<LVStack> {
public:
    explicit LVStack(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_obj_create(parent ? parent : lv_scr_act())) {
        lv_obj_set_flex_flow(m_obj, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(m_obj, 
            LV_FLEX_ALIGN_START,
            LV_FLEX_ALIGN_CENTER,
            LV_FLEX_ALIGN_START
        );
    }
    
    LVStack& set_spacing(int gap) {
        lv_obj_set_style_pad_row(m_obj, gap, 0);
        return *this;
    }
    
    LVStack& set_padding(int pad) {
        lv_obj_set_style_pad_all(m_obj, pad, 0);
        return *this;
    }
    
    LVStack& set_align_items(lv_flex_align_t align) {
        lv_obj_set_flex_align(m_obj, 
            LV_FLEX_ALIGN_START,
            align,
            LV_FLEX_ALIGN_START
        );
        return *this;
    }
    
    LVStack& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
    
    LVStack& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
    
    LVStack& set_align(lv_align_t a) {
        lv_obj_set_align(m_obj, a);
        return *this;
    }
};

class LHStack : public LWidgetCRTP<LHStack> {
public:
    explicit LHStack(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_obj_create(parent ? parent : lv_scr_act())) {
        lv_obj_set_flex_flow(m_obj, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(m_obj, 
            LV_FLEX_ALIGN_START,
            LV_FLEX_ALIGN_CENTER,
            LV_FLEX_ALIGN_START
        );
    }
    
    LHStack& set_spacing(int gap) {
        lv_obj_set_style_pad_column(m_obj, gap, 0);
        return *this;
    }
    
    LHStack& set_padding(int pad) {
        lv_obj_set_style_pad_all(m_obj, pad, 0);
        return *this;
    }
    
    LHStack& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
    
    LHStack& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
    
    LHStack& set_align(lv_align_t a) {
        lv_obj_set_align(m_obj, a);
        return *this;
    }
};

} // namespace lvgl
