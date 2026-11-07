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
#include "simulator_settings.h"
/* port 的 backends 层在创建窗口时读取该结构，定义在 src/lib/driver_backends.c */
extern simulator_settings_t settings;
}
#include <chrono>
#include <cstdlib>
#include <cstring>

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
    int width = env_int("EASYLVGL_WINDOW_WIDTH", env_int("LV_SDL_VIDEO_WIDTH", default_width));
    int height = env_int("EASYLVGL_WINDOW_HEIGHT", env_int("LV_SDL_VIDEO_HEIGHT", default_height));
    if(width <= 0) width = 800;
    if(height <= 0) height = 480;

    settings.window_width = static_cast<uint32_t>(width);
    settings.window_height = static_cast<uint32_t>(height);

    /* 窗口与输入设备交由 port 的 backends 层创建：
     * src/lib/display_backends/{sdl,win32,fbdev,drm,x11,wayland,glfw3}.c
     * 本封装只按可用性顺序挑一个，不直接调用平台 API。 */
    driver_backends_register();
    static const char *candidates[] = {"WIN32", "SDL", "X11", "WAYLAND", "FBDEV", "DRM", "GLFW3", nullptr};
    for(int i = 0; candidates[i]; i++) {
        /* port 的 is_supported() 会就地把名字转大写，必须传入可写缓冲而不是字符串字面量 */
        char name[16];
        const size_t len = std::strlen(candidates[i]);
        if(len == 0 || len >= sizeof(name)) continue;
        std::memcpy(name, candidates[i], len + 1);

        if(!driver_backends_is_supported(name)) continue;
        if(driver_backends_init_backend(name) != 0) continue;
        return lv_display_get_default();
    }
    return nullptr;
}

} // namespace easylvgl
