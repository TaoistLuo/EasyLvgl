/**************************************************************************************************
 * @file        layer1_demo.cpp
 * @brief       layer1_demo
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

#include "easylvgl/lvgl_core.hpp"
#include "easylvgl/core/signal_handler.hpp"
#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "easylvgl/core/platform.hpp"
#include <stdio.h>

using namespace easylvgl;

static const char *getenv_default(const char *name, const char *dflt)
{
    return getenv(name) ? : dflt;
}

#if LV_USE_SDL
static void lv_linux_disp_init(void)
{
    const int width = atoi(getenv("LV_SDL_VIDEO_WIDTH") ? : "800");
    const int height = atoi(getenv("LV_SDL_VIDEO_HEIGHT") ? : "480");
    lv_sdl_window_create(width, height);
}
#else
#error Unsupported configuration
#endif

int main(void)
{
    lv_init();
    lv_linux_disp_init();
    lv_sdl_mouse_create();

    SignalHandler signal_handler;

    printf("=== LVGL++ Layer 1 Demo ===\n");
    printf("Press Ctrl+C to exit gracefully.\n\n");

    auto btn = configure(lv_button_create(lv_screen_active()))
        .size(120, 50)
        .pos(10, 50)
        .get();

    auto label = configure(lv_label_create(btn))
        .get();
    lv_obj_center(label);
    lv_label_set_text(label, "Button");

    connect_event(btn, LV_EVENT_CLICKED, [](lv_event_t* e) {
        printf("Button clicked!\n");
    });

    LStyle default_style;
    default_style.set_bg_color(0x2196F3)
                 .set_border_width(2)
                 .set_border_color(0x1976D2)
                 .set_radius(5)
                 .apply_to(btn);

    LStyle pressed_style;
    pressed_style.set_bg_color(0x1976D2).apply_to(btn, LV_STATE_PRESSED);

    auto label2 = configure(lv_label_create(lv_scr_act()))
        .pos(10, 10)
        .get();
    lv_label_set_text(label2, "LVGL++ Layer 1 Demo");
    
    printf("Demo running. Press Ctrl+C to exit gracefully.\n");

    while(signal_handler.is_running()) {
        lv_timer_handler();
        easylvgl::sleep_ms(5);
    }

    printf("\nCleaning up resources...\n");
    return 0;
}
