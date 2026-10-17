#pragma once
#include "lvgl++/core/widget.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LTileView : public LWidgetCRTP<LTileView> {
public:
    explicit LTileView(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_tileview_create(parent ? parent : lv_scr_act())) {}

    lv_obj_t* add_tile(uint8_t col_id, uint8_t row_id, lv_dir_t dir) {
        return lv_tileview_add_tile(m_obj, col_id, row_id, dir);
    }

    LTileView& set_tile(lv_obj_t* tile_obj, lv_anim_enable_t anim = LV_ANIM_OFF) {
        lv_tileview_set_tile(m_obj, tile_obj, anim);
        return *this;
    }

    LTileView& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }

    LTileView& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return *this;
    }
};

class LTile : public LWidgetCRTP<LTile> {
public:
    explicit LTile(lv_obj_t* tileview, int col, int row, int dir)
        : LWidgetCRTP(lv_tileview_add_tile(tileview ? tileview : lv_scr_act(),
                                          (uint8_t)col, (uint8_t)row, (lv_dir_t)dir)) {}

    LTile& set_size(int w, int h) { lv_obj_set_size(m_obj, w, h); return *this; }
    LTile& set_pos(int x, int y) { lv_obj_set_pos(m_obj, x, y); return *this; }
};

} // namespace lvgl
