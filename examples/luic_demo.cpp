/**************************************************************************************************
 * @file        luic_demo.cpp
 * @brief       luic 编译器（.lui -> C++）
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
#include "easylvgl/core/signal_handler.hpp"
#include "main_screen_ui.hpp"
#include "easylvgl/core/platform.hpp"
#include <cstdio>
#include <cstdlib>

using namespace easylvgl;

class MainScreen : public MainScreen_UI {
public:
    void on_btn_login_clicked(lv_event_t* e) override {
        static int count = 0;
        count++;
        printf("Login clicked! count=%d\n", count);
    }

    void on_brightness_value_changed(lv_event_t* e) override {
        int val = lv_slider_get_value(m_brightness->native());
        printf("Brightness: %d\n", val);
        m_progress->set_value(val);
    }

    void on_cb_enable_value_changed(lv_event_t* e) override {
        bool checked = lv_obj_has_state(m_cb_enable->native(), LV_STATE_CHECKED);
        printf("Feature: %s\n", checked ? "ON" : "OFF");
    }
};

int main(void)
{
    lv_init();
    init_desktop_display(800, 480);

    SignalHandler signal_handler;

    printf("=== LVGL++ luic End-to-End Demo ===\n");
    printf("UI generated from main_screen.lui\n");
    printf("Press Ctrl+C to exit gracefully.\n\n");

    MainScreen screen;
    screen.setup();
    screen.bind_events();

    while(signal_handler.is_running()) {
        lv_timer_handler();
        easylvgl::sleep_ms(5);
    }

    printf("\nCleaning up resources...\n");
    return 0;
}
