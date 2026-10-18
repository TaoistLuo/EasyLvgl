#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl++/core/style.hpp"
#include "lvgl++/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

/**
 * @brief LObj —— 通用容器（对应 LVGL 的 lv_obj）
 *
 * 主要用途是**组件根容器**：默认是"裸"对象——去掉主题样式（透明背景、
 * 无边框/圆角/内边距），关闭滚动，尺寸随内容自适应，因此不会给组件内部
 * 带来任何额外视觉；放到父级 flex/grid 里就是普通的一个布局项。
 */
class LObj : public LWidgetCRTP<LObj> {
public:
    explicit LObj(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_obj_create(parent ? parent : lv_scr_act())) {
        lv_obj_remove_style_all(m_obj);
        lv_obj_remove_flag(m_obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_size(m_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    }
};

} // namespace lvgl
