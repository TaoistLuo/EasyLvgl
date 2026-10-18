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

#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "easylvgl/core/platform.hpp"
#include <stdio.h>
#include <stdlib.h>

static const char *getenv_default(const char *name, const char *dflt)
{
    const char *value = getenv(name);
    return value ? value : dflt;
}

/* 跨平台显示/输入初始化：
 *   Linux 嵌入式 -> fbdev / DRM
 *   桌面（Linux/macOS/Windows）-> 统一走 easylvgl::init_desktop_display()（SDL 或 Win32 后端） */
static void lv_display_init(void)
{
#if LV_USE_LINUX_FBDEV
    const char *device = getenv_default("LV_LINUX_FBDEV_DEVICE", "/dev/fb0");
    lv_display_t * disp = lv_linux_fbdev_create();

    lv_linux_fbdev_set_file(disp, device);
#elif LV_USE_LINUX_DRM
    const char *device = getenv_default("LV_LINUX_DRM_CARD", "/dev/dri/card0");
    lv_display_t * disp = lv_linux_drm_create();

    lv_linux_drm_set_file(disp, device, -1);
#elif LV_USE_SDL || LV_USE_WINDOWS
    easylvgl::init_desktop_display(1280, 720);
#else
#error Unsupported configuration
#endif
}

// Old LObject/LStyle removed - use new LVGL++ API
// #include "easylvgl/lvgl_widgets.hpp"

int main(void)
{
    lv_init();

    /* Display device init (Linux fbdev/DRM, SDL on desktop, Win32 backend) */
    lv_display_init();
    //初始化触摸
    // lv_indev_t * touch;
    // touch = lv_evdev_create(LV_INDEV_TYPE_POINTER,"/dev/input/event4");

    /*Create a Demo*/
    lv_demo_widgets();
    // lv_demo_widgets_start_slideshow();

    // Old LObject/LStyle code removed
    // To see new LVGL++ API examples, run:
    //   ./layer1_demo
    //   ./layer2_demo
    //   ./complete_demo
    while(1) {
        lv_timer_handler();
        easylvgl::sleep_ms(5);
    }
    return 0;
}
