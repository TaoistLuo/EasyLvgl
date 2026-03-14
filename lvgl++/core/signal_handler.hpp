#pragma once

/**
 * @file signal_handler.hpp
 * @brief 信号处理工具，用于优雅退出
 * @version 1.0.0
 * @date 2026-03-15
 * 
 * 提供信号处理功能，确保程序在接收到 SIGINT/SIGTERM 时优雅退出。
 * 避免 Ctrl+C 导致的崩溃问题。
 */

#include <signal.h>
#include <atomic>
#include <cstdio>

namespace lvgl {

/**
 * @class SignalHandler
 * @brief 信号处理器，用于优雅退出
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
     * @brief 构造函数，注册信号处理器
     * 
     * 自动注册 SIGINT 和 SIGTERM 信号处理器。
     */
    SignalHandler() {
        get_running_flag().store(true);
        signal(SIGINT, signal_callback);
        signal(SIGTERM, signal_callback);
    }
    
    /**
     * @brief 检查程序是否应该继续运行
     * @return bool true = 继续运行，false = 应该退出
     */
    bool is_running() const {
        return get_running_flag().load();
    }
    
    /**
     * @brief 请求程序退出
     */
    void request_exit() {
        get_running_flag().store(false);
    }
};

} // namespace lvgl
