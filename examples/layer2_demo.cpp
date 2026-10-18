/**************************************************************************************************
 * @file        layer2_demo.cpp
 * @brief       layer2_demo
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

#include "easylvgl/lvgl_widgets.hpp"
#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "easylvgl/core/platform.hpp"
#include <stdio.h>

using namespace easylvgl;

int main(void)
{
    lv_init();
    init_desktop_display(800, 480);

    printf("=== LVGL++ Layer 2 Demo ===\n");

    LVBoxLayout main_container;
    main_container.set_spacing(20)
                  .set_padding(30)
                  .set_size(400, 500)
                  .set_align(LV_ALIGN_CENTER);

    LLabel title(main_container.native());
    title.set_text("LVGL++ Components")
         .set_color(0x2196F3)
         .set_font_size(24);

    LLabel subtitle(main_container.native());
    subtitle.set_text("Modern C++ UI Framework")
            .set_color(0x666666)
            .set_font_size(14);

    LHBoxLayout button_row(main_container.native());
    button_row.set_spacing(10);

    LButton btn1(button_row.native());
    btn1.set_text("Primary")
        .set_size(100, 40)
        .primary()
        .on_click([](auto*) {
            printf("Primary button clicked!\n");
        });

    LButton btn2(button_row.native());
    btn2.set_text("Success")
        .set_size(100, 40)
        .success()
        .on_click([](auto*) {
            printf("Success button clicked!\n");
        });

    LButton btn3(button_row.native());
    btn3.set_text("Danger")
        .set_size(100, 40)
        .danger()
        .on_click([](auto*) {
            printf("Danger button clicked!\n");
        });

    LHBoxLayout button_row2(main_container.native());
    button_row2.set_spacing(10);

    LButton btn4(button_row2.native());
    btn4.set_text("Warning")
        .set_size(100, 40)
        .warning()
        .on_click([](auto*) {
            printf("Warning button clicked!\n");
        });

    LButton btn5(button_row2.native());
    btn5.set_text("Disabled")
        .set_size(100, 40)
        .set_disabled();

    static int counter = 0;
    LLabel counter_label(main_container.native());
    counter_label.set_text_fmt("Counter: %d", counter)
                 .set_color(0x333333);

    LButton increment_btn(main_container.native());
    increment_btn.set_text("Increment Counter")
                 .set_size(200, 50)
                 .primary()
                 .on_click([&counter_label](auto*) {
                     counter++;
                     lv_label_set_text_fmt(counter_label.native(), "Counter: %d", counter);
                     printf("Counter: %d\n", counter);
                 });

    LLabel info(main_container.native());
    info.set_text("Click buttons to test!")
        .set_color(0x999999)
        .set_font_size(12);

    printf("Layer 2 demo started!\n");
    printf("Features:\n");
    printf("- LButton component with themes\n");
    printf("- LLabel component with colors\n");
    printf("- LVBoxLayout/LHBoxLayout layouts\n");
    printf("- Event handling\n");

    while(1) {
        lv_timer_handler();
        easylvgl::sleep_ms(5);
    }
    
    return 0;
}
