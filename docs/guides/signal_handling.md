# 信号处理与优雅退出

## 📋 概述

本文档说明 LVGL++ 如何处理 Ctrl+C (SIGINT) 信号，以及如何实现优雅退出。

---

## 🐛 问题背景

### Ctrl+C 默认行为

```cpp
int main() {
    LButton btn;  // 栈上对象
    
    while(1) {
        lv_timer_handler();
    }
    // ❌ Ctrl+C 时，程序立即终止
    // ❌ 不执行析构函数
    // ❌ 可能崩溃
}
```

**问题：**
1. Ctrl+C 发送 SIGINT 信号
2. 程序立即终止，跳过栈展开
3. C++ 对象的析构函数不被调用
4. LVGL 内部可能还在引用这些对象
5. 导致悬空指针、内存访问错误

---

## ✅ 解决方案

### 使用 SignalHandler 类

LVGL++ 提供了 `SignalHandler` 工具类，用于优雅退出。

```cpp
#include "easylvgl/core/signal_handler.hpp"

int main() {
    lv_init();
    lv_display_t* disp = lv_sdl_window_create(800, 600);
    lv_sdl_mouse_create();

    // 创建信号处理器
    SignalHandler signal_handler;

    LButton btn;
    btn.set_text("Click Me").set_size(150, 50);
    
    printf("Press Ctrl+C to exit gracefully.\n");
    
    // 使用 is_running() 控制循环
    while(signal_handler.is_running()) {
        lv_timer_handler();
        usleep(5000);
    }
    
    printf("Cleaning up resources...\n");
    // ✅ 析构函数正常执行
    
    return 0;
}
```

---

## 🔧 工作原理

### 1. 注册信号处理器

```cpp
SignalHandler signal_handler;
```

构造函数自动注册 SIGINT 和 SIGTERM 信号处理器：

```cpp
signal(SIGINT, signal_callback);
signal(SIGTERM, signal_callback);
```

### 2. 信号回调函数

```cpp
static void signal_callback(int signum) {
    printf("\nReceived signal %d, exiting gracefully...\n", signum);
    s_running.store(false);  // 设置退出标志
}
```

### 3. 主循环检查标志

```cpp
while(signal_handler.is_running()) {  // 检查标志位
    lv_timer_handler();
    usleep(5000);
}
```

### 4. 优雅退出

- 循环退出后，程序正常返回
- 栈展开正常进行
- 析构函数按逆序调用
- 资源正确释放

---

## 📊 行为对比

### 默认行为（不使用 SignalHandler）

```
用户按 Ctrl+C
    ↓
SIGINT 信号
    ↓
程序立即终止
    ↓
❌ 跳过析构函数
❌ 资源未释放
❌ 可能崩溃
```

### 使用 SignalHandler

```
用户按 Ctrl+C
    ↓
SIGINT 信号
    ↓
调用 signal_callback
    ↓
设置 s_running = false
    ↓
while 循环检测到标志
    ↓
退出循环
    ↓
✅ 执行析构函数
✅ 释放资源
✅ 正常退出
```

---

## 💡 API 参考

### SignalHandler 类

```cpp
namespace lvgl {

class SignalHandler {
public:
    /**
     * @brief 构造函数，注册信号处理器
     * 
     * 自动注册 SIGINT 和 SIGTERM 信号处理器。
     */
    SignalHandler();
    
    /**
     * @brief 检查程序是否应该继续运行
     * @return bool true = 继续运行，false = 应该退出
     */
    bool is_running() const;
    
    /**
     * @brief 请求程序退出
     */
    void request_exit();
};

} // namespace lvgl
```

---

## 📝 使用示例

### 基础用法

```cpp
#include "easylvgl/lvgl_widgets.hpp"
#include "easylvgl/core/signal_handler.hpp"

int main() {
    lv_init();
    lv_sdl_window_create(800, 600);
    lv_sdl_mouse_create();

    SignalHandler signal_handler;

    LButton btn;
    btn.set_text("Hello").set_size(100, 40);
    
    while(signal_handler.is_running()) {
        lv_timer_handler();
        usleep(5000);
    }
    
    return 0;
}
```

### 手动请求退出

```cpp
SignalHandler signal_handler;

LButton exit_btn;
exit_btn.set_text("Exit")
        .on_click([&signal_handler](auto*) {
            signal_handler.request_exit();  // 手动请求退出
        });

while(signal_handler.is_running()) {
    lv_timer_handler();
    usleep(5000);
}
```

### 多个退出条件

```cpp
SignalHandler signal_handler;
bool user_requested_exit = false;

while(signal_handler.is_running() && !user_requested_exit) {
    lv_timer_handler();
    usleep(5000);
}
```

---

## ⚠️ 注意事项

### 1. 退出延迟

使用 SignalHandler 后，Ctrl+C 不会立即退出，而是在下一次循环迭代时退出。

**延迟时间：** 最多 5ms（取决于 `usleep(5000)` 的值）

### 2. kill -9 无法防止

`kill -9` (SIGKILL) 是操作系统强制终止，无法被捕获，仍然会立即退出。

### 3. 信号安全性

信号处理器中只能调用信号安全的函数。`SignalHandler` 使用 `std::atomic<bool>` 确保线程安全。

### 4. 单例模式

`SignalHandler` 使用静态成员变量，多个实例共享同一个退出标志。

---

## 🎯 最佳实践

### 1. 总是使用 SignalHandler

```cpp
int main() {
    lv_init();
    // ...
    
    SignalHandler signal_handler;  // ✅ 推荐
    
    while(signal_handler.is_running()) {
        lv_timer_handler();
        usleep(5000);
    }
    
    return 0;
}
```

### 2. 提示用户

```cpp
printf("Press Ctrl+C to exit gracefully.\n");
```

### 3. 清理提示

```cpp
while(signal_handler.is_running()) {
    lv_timer_handler();
    usleep(5000);
}

printf("Cleaning up resources...\n");
```

---

## 🔍 技术细节

### 析构函数改进

LVGL++ 的 `LWidget` 析构函数使用同步删除：

```cpp
virtual ~LWidget() {
    if (m_obj) {
        lv_obj_delete(m_obj);  // 同步删除，不依赖事件循环
        m_obj = nullptr;
    }
}
```

**优点：**
- 不依赖事件循环
- 可以在程序退出时安全调用
- 配合 SignalHandler 实现完全优雅退出

---

## 📚 相关文档

- [LVGL++ 编码规范](coding_style.md)
- [luic 编译器指南](luic_guild.md)
---

## 🎉 总结

### 问题
- Ctrl+C 默认立即终止程序
- 跳过析构函数
- 可能导致崩溃

### 解决方案
- 使用 `SignalHandler` 类
- 将 Ctrl+C 变成"请求退出"
- 优雅退出，正确释放资源

### 使用方法
```cpp
SignalHandler signal_handler;
while(signal_handler.is_running()) {
    lv_timer_handler();
    usleep(5000);
}
```

**简单、安全、有效！** ✅
