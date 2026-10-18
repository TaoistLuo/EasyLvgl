/**************************************************************************************************
 * @file        signal_handler.hpp
 * @brief       信号处理与优雅退出
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
#include <signal.h>
#include <atomic>
#include <cstdio>

namespace easylvgl {

/**
 * @class SignalHandler
 * 
 * 使用方法：
 * @code
 * SignalHandler signal_handler;
 * 
 * while(signal_handler.is_running()) {
 *     lv_timer_handler();
 *     usleep(5000);
 * }
 * @endcode
 */
class SignalHandler {
private:
    // 使用函数内静态变量避免多重定义
    static std::atomic<bool>& get_running_flag() {
        static std::atomic<bool> s_running(true);
        return s_running;
    }
    
    static void signal_callback(int signum) {
        printf("\nReceived signal %d, exiting gracefully...\n", signum);
        get_running_flag().store(false);
    }
    
public:
    /**
     * 
     * 自动注册 SIGINT 和 SIGTERM 信号处理器。
     */
    SignalHandler() {
        get_running_flag().store(true);
        signal(SIGINT, signal_callback);
        signal(SIGTERM, signal_callback);
    }
    
    /**
     * @return bool true = 继续运行，false = 应该退出
     */
    bool is_running() const {
        return get_running_flag().load();
    }
    
    /**
     */
    void request_exit() {
        get_running_flag().store(false);
    }
};

} // namespace easylvgl
