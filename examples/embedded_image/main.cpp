/**************************************************************************************************
 * @file        main.cpp
 * @brief       示例程序
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

/* 图片资源编译期嵌入示例（等价 Qt 的 qrc）
 *
 * screen.lui 里：
 *   "resources": { "caret": "assets/caret_down.png" }
 *   LImage 的 "src": "#caret"
 *
 * scripts/assets/convert_assets.py 把 PNG 转成 lv_image_dsc_t 常量，
 * luic 生成 set_src(&caret) —— 最终固件里没有图片文件，也没有 lv_fs 依赖。
 */
#include "easylvgl/lvgl_widgets.hpp"
#include "easylvgl/core/signal_handler.hpp"
#include "screen_ui.hpp"
#include "easylvgl/core/platform.hpp"
#include <cstdio>

using namespace easylvgl;

int main(void) {
    lv_init();
    init_desktop_display(EmbeddedImage_UI::kScreenWidth, EmbeddedImage_UI::kScreenHeight);

    SignalHandler signal_handler;

    printf("=== 图片嵌入示例 ===\n");
    printf("caret_down.png 已转换为 lv_image_dsc_t，代码里直接用 &caret，\n");
    printf("运行时不读取任何图片文件（可用 lsof 验证）。Ctrl+C 退出。\n\n");

    EmbeddedImage_UI ui;
    ui.setup();
    ui.bind_events();

    while (signal_handler.is_running()) {
        lv_timer_handler();
        easylvgl::sleep_ms(5);
    }
    printf("\n退出。\n");
    return 0;
}
