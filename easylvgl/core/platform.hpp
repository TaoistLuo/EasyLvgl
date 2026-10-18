/**************************************************************************************************
 * @file        platform.hpp
 * @brief       跨平台基础函数
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

#pragma once
#include "lvgl/lvgl.h"
#include <thread>
#include <chrono>
#include <cstdlib>

namespace easylvgl {

/** 跨平台毫秒睡眠（替代 POSIX 的 usleep，Windows/MinGW 同样可用） */
inline void sleep_ms(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

/** 读取整型环境变量，不存在时返回 fallback（跨平台，不依赖 GNU 的 `?:` 扩展） */
inline int env_int(const char *name, int fallback) {
    const char *value = std::getenv(name);
    return value ? std::atoi(value) : fallback;
}

/**
 * 创建桌面端窗口显示与默认输入设备。
 *
 * 后端选择完全由 lv_conf.h 决定，三平台共用同一套调用：
 *   - Windows：LVGL 原生 Win32 后端（lv_windows_*），不依赖 SDL
 *   - Linux/macOS 桌面：SDL2（lv_sdl_*）
 *
 * 窗口尺寸可分别用环境变量 EASYLVGL_WINDOW_WIDTH / EASYLVGL_WINDOW_HEIGHT 覆盖，
 * 为兼容旧用法也接受 LV_SDL_VIDEO_WIDTH / LV_SDL_VIDEO_HEIGHT。
 *
 * @return 创建的 display，失败返回 nullptr。
 */
inline lv_display_t *init_desktop_display(int default_width, int default_height)
{
    int width = env_int("EASYLVGL_WINDOW_WIDTH", env_int("LV_SDL_VIDEO_WIDTH", default_width));
    int height = env_int("EASYLVGL_WINDOW_HEIGHT", env_int("LV_SDL_VIDEO_HEIGHT", default_height));

#if LV_USE_SDL
    lv_display_t *disp = lv_sdl_window_create(width, height);
    lv_sdl_mouse_create();
    return disp;
#elif LV_USE_WINDOWS
    lv_display_t *disp = lv_windows_create_display(
        L"EasyLVGL", width, height, 100, false, true);
    if(disp) {
        lv_windows_acquire_pointer_indev(disp);
        lv_windows_acquire_keypad_indev(disp);
    }
    return disp;
#else
    (void)width;
    (void)height;
    return nullptr;
#endif
}

} // namespace easylvgl
