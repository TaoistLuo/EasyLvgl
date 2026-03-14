#pragma once

/**
 * @file vstack.hpp
 * @brief 垂直堆叠布局容器
 * @version 1.0.0
 * @date 2026-03-15
 */

#include "lvgl++/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

/**
 * @class LVStack
 * @brief 垂直堆叠布局容器
 * 
 * 使用Flex布局，子元素垂直排列。
 */
class LVStack : public LWidget {
public:
    explicit LVStack(lv_obj_t* parent = nullptr)
        : LWidget(lv_obj_create(parent ? parent : lv_scr_act())) {
        lv_obj_set_flex_flow(m_obj, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(m_obj, 
            LV_FLEX_ALIGN_START,
            LV_FLEX_ALIGN_CENTER,
            LV_FLEX_ALIGN_START
        );
    }
    
    LVStack& spacing(int gap) {
        lv_obj_set_style_pad_row(m_obj, gap, 0);
        return *this;
    }
    
    LVStack& padding(int pad) {
        lv_obj_set_style_pad_all(m_obj, pad, 0);
        return *this;
    }
    
    LVStack& align_items(lv_flex_align_t align) {
        lv_obj_set_flex_align(m_obj, 
            LV_FLEX_ALIGN_START,
            align,
            LV_FLEX_ALIGN_START
        );
        return *this;
    }
    
    LVStack& size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
    
    LVStack& pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
    
    LVStack& align(lv_align_t a) {
        lv_obj_set_align(m_obj, a);
        return *this;
    }
};

/**
 * @class LHStack
 * @brief 水平堆叠布局容器
 */
class LHStack : public LWidget {
public:
    explicit LHStack(lv_obj_t* parent = nullptr)
        : LWidget(lv_obj_create(parent ? parent : lv_scr_act())) {
        lv_obj_set_flex_flow(m_obj, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(m_obj, 
            LV_FLEX_ALIGN_START,
            LV_FLEX_ALIGN_CENTER,
            LV_FLEX_ALIGN_START
        );
    }
    
    LHStack& spacing(int gap) {
        lv_obj_set_style_pad_column(m_obj, gap, 0);
        return *this;
    }
    
    LHStack& padding(int pad) {
        lv_obj_set_style_pad_all(m_obj, pad, 0);
        return *this;
    }
    
    LHStack& size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
    
    LHStack& pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
    
    LHStack& align(lv_align_t a) {
        lv_obj_set_align(m_obj, a);
        return *this;
    }
};

} // namespace lvgl
