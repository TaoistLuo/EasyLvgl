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

extern "C" {
#include "driver_backends.h"
}
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
 * 后端选择由构建配置（Kconfig defconfig）与运行平台共同决定，实现位于
 * src/lib/display_backends/；本函数只按可用顺序挑选，不直接调用平台 API。
 *
 * 窗口尺寸可分别用环境变量 EASYLVGL_WINDOW_WIDTH / EASYLVGL_WINDOW_HEIGHT 覆盖，
 * 为兼容旧用法也接受 LV_SDL_VIDEO_WIDTH / LV_SDL_VIDEO_HEIGHT。
 *
 * @return 创建的 display，失败返回 nullptr。
 */
inline lv_display_t *init_desktop_display(int default_width, int default_height)
{
    int width = env_int("EASYLVGL_WINDOW_WIDTH", default_width);
    int height = env_int("EASYLVGL_WINDOW_HEIGHT", default_height);

    /* 窗口与输入设备交由 port 的 backends 层创建：
     * src/lib/display_backends/{sdl,win32,fbdev,drm,x11,wayland,glfw3}.c
     * 本封装只按可用性顺序挑一个，不直接调用平台 API。 */
    driver_backends_register();
    static const char *candidates[] = {"WIN32", "SDL", "X11", "WAYLAND", "FBDEV", "DRM", "GLFW3", nullptr};
    for(int i = 0; candidates[i]; i++) {
        if(!driver_backends_is_supported(const_cast<char *>(candidates[i]))) continue;
        if(driver_backends_init_backend(const_cast<char *>(candidates[i])) != 0) continue;
        lv_display_t *disp = lv_display_get_default();
        if(disp) {
            lv_display_set_resolution(disp, width > 0 ? width : lv_display_get_horizontal_resolution(disp),
                                      height > 0 ? height : lv_display_get_vertical_resolution(disp));
        }
        return disp;
    }
    return nullptr;
}

} // namespace easylvgl
