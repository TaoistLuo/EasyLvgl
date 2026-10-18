/**************************************************************************************************
 * @file        test_wrappers.cpp
 * @brief       test_wrappers
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

#include "lvgl/lvgl.h"
#include "easylvgl/lvgl_widgets.hpp"
#include <cstdio>
#include <string>

using namespace easylvgl;

static int g_pass = 0;
static int g_fail = 0;

#define CHECK(cond)                                                         \
    do {                                                                    \
        if (cond) { g_pass++; }                                             \
        else { g_fail++; printf("  FAIL: %s  (%s:%d)\n", #cond, __FILE__, __LINE__); } \
    } while (0)

static void initEnv() {
    lv_init();
    static lv_color_t buf[800 * 10];
    lv_display_t* disp = lv_display_create(800, 480);
    lv_display_set_flush_cb(disp, [](lv_display_t*, const lv_area_t*, uint8_t*) {});
    lv_display_set_buffers(disp, buf, nullptr, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
}

static int screenChildCount() {
    return (int)lv_obj_get_child_count(lv_scr_act());
}

static void test_raii() {
    printf("[RAII]\n");
    int before = screenChildCount();
    {
        LButton btn;
        CHECK(screenChildCount() == before + 1);
    }
    CHECK(screenChildCount() == before);

    {
        LVBoxLayout box;
        LLabel lbl(box.native());
        CHECK(screenChildCount() == before + 1);
    }
    CHECK(screenChildCount() == before);
}

static void test_move() {
    printf("[Move semantics]\n");
    int before = screenChildCount();
    {
        LButton a;
        LButton b(std::move(a));
        CHECK(b.native() != nullptr);
        CHECK(a.native() == nullptr);
    }
    CHECK(screenChildCount() == before);
}

static void test_style() {
    printf("[LStyle]\n");
    LButton btn;
    LStyle s;
    s.set_bg_color(0x2196F3).set_radius(8).set_pad_all(10);
    s.apply_to(btn.native());
    CHECK(lv_obj_get_style_radius(btn.native(), 0) == 8);
    CHECK(lv_obj_get_style_pad_top(btn.native(), 0) == 10);
}

static void test_events() {
    printf("[Events]\n");
    LButton btn;
    int hits = 0;
    btn.on_click([&hits](lv_event_t*) { hits++; });
    lv_obj_send_event(btn.native(), LV_EVENT_CLICKED, nullptr);
    CHECK(hits == 1);
}

static void test_widget_props() {
    printf("[Widget props]\n");
    LButton b;
    b.set_text("OK");
    CHECK(std::string(lv_label_get_text(lv_obj_get_child(b.native(), 0))) == "OK");

    LLabel l;
    l.set_text("Hi");
    CHECK(std::string(lv_label_get_text(l.native())) == "Hi");

    LSlider sl;
    sl.set_range(0, 100).set_value(40);
    CHECK(lv_slider_get_value(sl.native()) == 40);

    LBar bar;
    bar.set_range(0, 100).set_value(65);
    CHECK(lv_bar_get_value(bar.native()) == 65);

    LCheckbox cb;
    cb.set_checked(true);
    CHECK(lv_obj_has_state(cb.native(), LV_STATE_CHECKED));

    LSwitch sw;
    sw.set_checked(true);
    CHECK(lv_obj_has_state(sw.native(), LV_STATE_CHECKED));

    LArc arc;
    arc.set_range(0, 100).set_value(70);
    CHECK(lv_arc_get_value(arc.native()) == 70);

    LSpinbox sp;
    sp.set_range(0, 999).set_value(42);
    CHECK(lv_spinbox_get_value(sp.native()) == 42);
}

static void test_regressions() {
    printf("[Regressions]\n");

    // LArc/LLed/LSpinner accept int_pair size (designer stores [w,h])
    LArc arc;
    arc.set_size(120, 120);
    lv_obj_update_layout(arc.native());
    CHECK(lv_obj_get_width(arc.native()) == 120);
    CHECK(lv_obj_get_height(arc.native()) == 120);

    LLed led;
    led.set_size(24, 24);
    lv_obj_update_layout(led.native());
    CHECK(lv_obj_get_width(led.native()) == 24);

    // LGridLayout int overload builds an FR(1) template
    LGridLayout grid;
    grid.set_columns(2).set_rows(2);
    CHECK(lv_obj_get_style_grid_column_dsc_array(grid.native(), 0) != nullptr);

    // LTable cell values
    LTable tbl;
    tbl.set_row_count(2).set_column_count(2);
    tbl.set_cell_value(0, 0, "A");
    tbl.set_cell_value(1, 1, "D");
    CHECK(std::string(lv_table_get_cell_value(tbl.native(), 0, 0)) == "A");
    CHECK(std::string(lv_table_get_cell_value(tbl.native(), 1, 1)) == "D");

    // LTabView add_tab
    LTabView tv;
    lv_obj_t* tab = tv.add_tab("One");
    CHECK(tab != nullptr);

    // LChart add_series + values
    LChart chart;
    chart.set_point_count(3);
    lv_chart_series_t* ser = chart.add_series(0xFF0000, LV_CHART_AXIS_PRIMARY_Y);
    chart.set_next_value(ser, 10);
    CHECK(ser != nullptr);

    // LMenu page + active
    LMenu menu;
    lv_obj_t* page = lv_menu_page_create(menu.native(), "Home");
    CHECK(page != nullptr);

    // LMenuPage wrapper
    LMenuPage mp(menu.native(), "Settings");
    CHECK(mp.native() != nullptr);

    // LTileView + LTile
    LTileView tileview;
    LTile tile(tileview.native(), 0, 0, 2);
    CHECK(tile.native() != nullptr);

    // LCheckbox / LRadioButton vertical centering on resize
    LCheckbox cb;
    cb.set_size(200, 60);
    lv_obj_update_layout(cb.native());
    CHECK(lv_obj_get_style_pad_top(cb.native(), LV_PART_MAIN) > 0);

    LRadioButton rb;
    rb.set_size(200, 60);
    lv_obj_update_layout(rb.native());
    lv_obj_t* ring = lv_obj_get_child(rb.native(), 0);
    CHECK(ring != nullptr);
    CHECK(lv_obj_get_y(ring) > 0);
}

int main() {
    initEnv();
    test_raii();
    test_move();
    test_style();
    test_events();
    test_widget_props();
    test_regressions();
    printf("\n=== Results ===\n  PASS: %d\n  FAIL: %d\n", g_pass, g_fail);
    return g_fail ? 1 : 0;
}
