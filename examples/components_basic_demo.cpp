/**************************************************************************************************
 * @file        components_basic_demo.cpp
 * @brief       components_basic_demo
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
#include "easylvgl/core/signal_handler.hpp"
#include "easylvgl/core/platform.hpp"
#include <cstdio>

using namespace easylvgl;

int main(void) {
    lv_init();
    lv_display_t* disp = lv_sdl_window_create(800, 600); (void)disp;
    lv_sdl_mouse_create();

    SignalHandler signal_handler;

    printf("=== LVGL++ Basic Components Demo ===\n");
    printf("Press Ctrl+C to exit gracefully.\n\n");

    LLabel title;
    title.set_text("LVGL++ Components Demo")
         .set_pos(10, 10)
         .set_color(0x2196F3);

    LButton btn1;
    btn1.set_text("Primary Button")
        .set_size(150, 50)
        .set_pos(10, 50)
        .primary()
        .on_click([](lv_event_t*) {
            printf("Primary button clicked!\n");
        });

    LButton btn2;
    btn2.set_text("Secondary")
        .set_size(150, 50)
        .set_pos(170, 50)
        .on_click([](lv_event_t*) {
            printf("Secondary button clicked!\n");
        });

    LSlider slider;
    slider.set_range(0, 100)
          .set_value(50)
          .set_size(310, 10)
          .set_pos(10, 120)
          .on_change([](lv_event_t* e) {
              auto* obj = (lv_obj_t*)lv_event_get_target(e);
              int val = lv_slider_get_value(obj);
              printf("Slider value: %d\n", val);
          });

    LCheckbox cb;
    cb.set_text("Enable Feature")
      .set_pos(10, 150)
      .set_checked(true)
      .on_change([](lv_event_t* e) {
          auto* obj = (lv_obj_t*)lv_event_get_target(e);
          bool checked = lv_obj_has_state(obj, LV_STATE_CHECKED);
          printf("Checkbox: %s\n", checked ? "checked" : "unchecked");
      });

    LSwitch sw;
    sw.set_pos(200, 150)
      .on_change([](lv_event_t* e) {
          auto* obj = (lv_obj_t*)lv_event_get_target(e);
          bool on = lv_obj_has_state(obj, LV_STATE_CHECKED);
          printf("Switch: %s\n", on ? "ON" : "OFF");
      });

    LBar bar;
    bar.set_range(0, 100)
       .set_value(75, LV_ANIM_ON)
       .set_size(310, 20)
       .set_pos(10, 200);

    LArc arc;
    arc.set_range(0, 100)
       .set_value(60)
       .set_rotation(270)
       .set_size(100)
       .set_pos(10, 240);

    LSpinner spinner;
    spinner.set_size(50)
           .set_pos(130, 265);

    LLed led_green;
    led_green.set_color(0x00FF00)
             .set_brightness(255)
             .on()
             .set_size(30)
             .set_pos(200, 270);

    LLed led_red;
    led_red.set_color(0xFF0000)
           .set_brightness(200)
           .off()
           .set_size(30)
           .set_pos(250, 270);

    while(signal_handler.is_running()) {
        lv_timer_handler();
        easylvgl::sleep_ms(5);
    }

    printf("\nCleaning up resources...\n");
    return 0;
}
