#pragma once
#include <thread>
#include <chrono>

namespace lvgl {

/** 跨平台毫秒睡眠（替代 POSIX 的 usleep，Windows/MinGW 同样可用） */
inline void sleep_ms(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

} // namespace lvgl
