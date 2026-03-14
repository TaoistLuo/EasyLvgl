#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LTileView : public LWidget {
public:
    explicit LTileView(lv_obj_t* parent = nullptr)
        : LWidget(lv_tileview_create(parent ? parent : lv_scr_act())) {}

    lv_obj_t* add_tile(uint8_t col_id, uint8_t row_id, lv_dir_t dir) {
        return lv_tileview_add_tile(m_obj, col_id, row_id, dir);
    }

    LTileView& set_tile(lv_obj_t* tile_obj, lv_anim_enable_t anim = LV_ANIM_OFF) {
        lv_tileview_set_tile(m_obj, tile_obj, anim);
        return *this;
    }
};

} // namespace lvgl
