/**************************************************************************************************
 * @file        stacked_layout.hpp
 * @brief       LStackedLayout 布局器
 * @details     EasyLVGL —— 让 LVGL 开发像 Qt 一样简单：C++ 封装 + 可视化设计器 + 代码生成。
 *
 * @author      taoist.luo
 * @version     1.0.0
 * @date        2026-10-18
 *
 * Copyright (c) 2026 taoist.luo (https://github.com/TaoistLuo/EasyLVGL)
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *************************************************************************************************/

#pragma once
#include "easylvgl/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace easylvgl {

// ============================================================================
// LStackedLayout — Stacked layout (Qt: QStackedLayout)
// Shows exactly one child at a time. All others are hidden.
// ============================================================================
class LStackedLayout : public LWidgetCRTP<LStackedLayout> {
public:
    explicit LStackedLayout(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_obj_create(parent ? parent : lv_scr_act())) {
        lv_obj_set_style_pad_all(m_obj, 6, 0);
    }

    // -- Child management --------------------------------------------------

    int add_widget(lv_obj_t* child) {
        return insert_widget(count(), child);
    }

    int insert_widget(int index, lv_obj_t* child) {
        if (!child) return -1;

        const int old_count = count();
        if (index < 0) index = 0;
        if (index > old_count) index = old_count;

        lv_obj_set_parent(child, m_obj);
        lv_obj_move_to_index(child, static_cast<int32_t>(index));

        if (old_count == 0) {
            m_current_index = 0;
        } else if (m_current_index < 0) {
            m_current_index = 0;
        } else if (index <= m_current_index) {
            ++m_current_index;
        }

        refresh_layout();
        return index;
    }

    bool remove_widget(lv_obj_t* child) {
        if (!child || lv_obj_get_parent(child) != m_obj) return false;

        int removed_idx = index_of(child);
        lv_obj_delete(child);

        const int new_count = count();

        if (new_count == 0) {
            m_current_index = -1;
        } else if (removed_idx < m_current_index) {
            --m_current_index;
        } else if (removed_idx == m_current_index) {
            if (m_current_index >= new_count) {
                m_current_index = new_count - 1;
            }
        }

        refresh_layout();
        return true;
    }

    // -- Current selection -------------------------------------------------

    LStackedLayout& set_current_index(int index) {
        const int cnt = count();
        if (cnt == 0 || index < 0 || index >= cnt) return *this;
        m_current_index = index;
        refresh_layout();
        return *this;
    }

    LStackedLayout& set_current_widget(lv_obj_t* widget) {
        int idx = index_of(widget);
        if (idx >= 0) set_current_index(idx);
        return *this;
    }

    int current_index() const {
        const int cnt = count();
        if (cnt == 0 || m_current_index < 0 || m_current_index >= cnt) return -1;
        return m_current_index;
    }

    lv_obj_t* current_widget() const {
        return widget_at(current_index());
    }

    // -- Queries -----------------------------------------------------------

    int index_of(lv_obj_t* child) const {
        if (!child || lv_obj_get_parent(child) != m_obj) return -1;
        return static_cast<int>(lv_obj_get_index(child));
    }

    lv_obj_t* widget_at(int index) const {
        if (index < 0) return nullptr;
        uint32_t cnt = lv_obj_get_child_count(m_obj);
        if (index >= static_cast<int>(cnt)) return nullptr;
        return lv_obj_get_child(m_obj, static_cast<int32_t>(index));
    }

    int count() const {
        return static_cast<int>(lv_obj_get_child_count(m_obj));
    }

    int page_count() const {
        return count();
    }

    // -- Geometry ----------------------------------------------------------

    LStackedLayout& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        refresh_layout();
        return *this;
    }

    LStackedLayout& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        refresh_layout();
        return *this;
    }

    LStackedLayout& set_width(int w) {
        lv_obj_set_width(m_obj, w);
        refresh_layout();
        return *this;
    }

    LStackedLayout& set_height(int h) {
        lv_obj_set_height(m_obj, h);
        refresh_layout();
        return *this;
    }

    LStackedLayout& set_padding(int pad) {
        lv_obj_set_style_pad_all(m_obj, pad, 0);
        refresh_layout();
        return *this;
    }

private:
    int m_current_index = -1;

    void apply_stacked_geometry(lv_obj_t* child) {
        if (!child) return;

        lv_obj_update_layout(m_obj);

        lv_area_t object_area;
        lv_area_t content_area;
        lv_obj_get_coords(m_obj, &object_area);
        lv_obj_get_content_coords(m_obj, &content_area);
        int width = content_area.x2 - content_area.x1 + 1;
        int height = content_area.y2 - content_area.y1 + 1;
        if (width < 0) width = 0;
        if (height < 0) height = 0;

        lv_obj_set_pos(child, 0, 0);
        lv_obj_set_size(child, width, height);
    }

    void refresh_layout() {
        const int active_index = current_index();
        const uint32_t cnt = lv_obj_get_child_count(m_obj);
        for (uint32_t i = 0; i < cnt; ++i) {
            lv_obj_t* child = lv_obj_get_child(m_obj, static_cast<int32_t>(i));
            if (!child) continue;
            apply_stacked_geometry(child);
            if (static_cast<int>(i) == active_index)
                lv_obj_remove_flag(child, LV_OBJ_FLAG_HIDDEN);
            else
                lv_obj_add_flag(child, LV_OBJ_FLAG_HIDDEN);
        }
    }
};

} // namespace easylvgl
