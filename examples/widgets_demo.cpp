/**************************************************************************************************
 * @file        widgets_demo.cpp
 * @brief       widgets_demo
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
#include "easylvgl/core/signal_handler.hpp"
#include <cstdio>
#include <cstring>
#include "easylvgl/core/platform.hpp"

// luic-generated UI for each component (examples/widgets/*.lui)
#include "animimage_ui.hpp"
#include "arc_ui.hpp"
#include "bar_ui.hpp"
#include "button_ui.hpp"
#include "buttonmatrix_ui.hpp"
#include "calendar_ui.hpp"
#include "canvas_ui.hpp"
#include "chart_ui.hpp"
#include "checkbox_ui.hpp"
#include "combobox_ui.hpp"
#include "grid_ui.hpp"
#include "groupbox_ui.hpp"
#include "hbox_ui.hpp"
#include "imagebutton_ui.hpp"
#include "image_ui.hpp"
#include "keyboard_ui.hpp"
#include "label_ui.hpp"
#include "led_ui.hpp"
#include "lineedit_ui.hpp"
#include "line_ui.hpp"
#include "list_ui.hpp"
#include "menu_ui.hpp"
#include "msgbox_ui.hpp"
#include "radiobutton_ui.hpp"
#include "roller_ui.hpp"
#include "scale_ui.hpp"
#include "slider_ui.hpp"
#include "span_ui.hpp"
#include "spinbox_ui.hpp"
#include "spinner_ui.hpp"
#include "stacked_ui.hpp"
#include "switch_ui.hpp"
#include "table_ui.hpp"
#include "tabview_ui.hpp"
#include "textedit_ui.hpp"
#include "tileview_ui.hpp"
#include "vbox_ui.hpp"
#include "win_ui.hpp"

using namespace easylvgl;

template <typename UI>
static void run_ui() {
    init_desktop_display(UI::kScreenWidth, UI::kScreenHeight);
    UI ui;
    ui.setup(nullptr);
    while (true) { lv_timer_handler(); easylvgl::sleep_ms(5); }
}

int main(int argc, char** argv) {
    lv_init();
    SignalHandler signal_handler;

    const char* name = (argc > 1) ? argv[1] : "label";
    printf("Widget gallery: %s\n", name);
    if (!strcmp(name, "animimage")) { run_ui<AnimImage_UI>(); }
    if (!strcmp(name, "arc")) { run_ui<Arc_UI>(); }
    if (!strcmp(name, "bar")) { run_ui<Bar_UI>(); }
    if (!strcmp(name, "button")) { run_ui<Button_UI>(); }
    if (!strcmp(name, "buttonmatrix")) { run_ui<ButtonMatrix_UI>(); }
    if (!strcmp(name, "calendar")) { run_ui<Calendar_UI>(); }
    if (!strcmp(name, "canvas")) { run_ui<Canvas_UI>(); }
    if (!strcmp(name, "chart")) { run_ui<Chart_UI>(); }
    if (!strcmp(name, "checkbox")) { run_ui<Checkbox_UI>(); }
    if (!strcmp(name, "combobox")) { run_ui<Combobox_UI>(); }
    if (!strcmp(name, "grid")) { run_ui<Grid_UI>(); }
    if (!strcmp(name, "groupbox")) { run_ui<GroupBox_UI>(); }
    if (!strcmp(name, "hbox")) { run_ui<HBox_UI>(); }
    if (!strcmp(name, "imagebutton")) { run_ui<ImageButton_UI>(); }
    if (!strcmp(name, "image")) { run_ui<Image_UI>(); }
    if (!strcmp(name, "keyboard")) { run_ui<Keyboard_UI>(); }
    if (!strcmp(name, "label")) { run_ui<Label_UI>(); }
    if (!strcmp(name, "led")) { run_ui<Led_UI>(); }
    if (!strcmp(name, "lineedit")) { run_ui<LineEdit_UI>(); }
    if (!strcmp(name, "line")) { run_ui<Line_UI>(); }
    if (!strcmp(name, "list")) { run_ui<List_UI>(); }
    if (!strcmp(name, "menu")) { run_ui<Menu_UI>(); }
    if (!strcmp(name, "msgbox")) { run_ui<MsgBox_UI>(); }
    if (!strcmp(name, "radiobutton")) { run_ui<RadioButton_UI>(); }
    if (!strcmp(name, "roller")) { run_ui<Roller_UI>(); }
    if (!strcmp(name, "scale")) { run_ui<Scale_UI>(); }
    if (!strcmp(name, "slider")) { run_ui<Slider_UI>(); }
    if (!strcmp(name, "span")) { run_ui<Span_UI>(); }
    if (!strcmp(name, "spinbox")) { run_ui<Spinbox_UI>(); }
    if (!strcmp(name, "spinner")) { run_ui<Spinner_UI>(); }
    if (!strcmp(name, "stacked")) { run_ui<Stacked_UI>(); }
    if (!strcmp(name, "switch")) { run_ui<Switch_UI>(); }
    if (!strcmp(name, "table")) { run_ui<Table_UI>(); }
    if (!strcmp(name, "tabview")) { run_ui<TabView_UI>(); }
    if (!strcmp(name, "textedit")) { run_ui<TextEdit_UI>(); }
    if (!strcmp(name, "tileview")) { run_ui<TileView_UI>(); }
    if (!strcmp(name, "vbox")) { run_ui<VBox_UI>(); }
    if (!strcmp(name, "win")) { run_ui<Win_UI>(); }
    printf("unknown component: %s\n", name);
    return 1;
}
