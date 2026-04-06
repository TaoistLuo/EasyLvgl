# Layer 1: 智能封装层详细设计

**版本：** 2.0.0  
**状态：** ✅ 已完成

## 概述

Layer 1是LVGL++的基础层，提供RAII内存管理、类型安全的事件系统和链式API，解决原生C API的核心痛点。

**设计目标：**
- 零运行时开销
- RAII自动内存管理
- 类型安全的事件处理
- 链式API提高可读性
- 与原生C API完全兼容

## 文件结构

```
lvgl++/core/
├── widget.hpp          # LWidget基类 + LWidgetCRTP<Derived>模板
├── style.hpp           # LStyle样式构建器
├── event.hpp           # 事件系统
├── config.hpp          # ObjectConfig（Fluent API）
└── signal_handler.hpp  # SignalHandler（Ctrl+C优雅退出）

lvgl++/
└── lvgl_core.hpp       # Layer 1统一入口（含ObjectConfig::style()延迟实现）
```

**注意：** 不存在`types.hpp`文件。类型定义直接使用LVGL原生类型。

## 核心组件设计

### 1. LWidget基类

**文件：** `lvgl++/core/widget.hpp`

```cpp
#pragma once
#include "lvgl/lvgl.h"
#include <utility>

namespace lvgl {

class LWidget;

template<typename Derived>
class LWidgetCRTP;

// CRTP Fluent辅助宏
#define LVGL_WIDGET_FLUENT_SIZE(CLASS_NAME) \
    CLASS_NAME& set_size(int w, int h) { \
        LWidgetCRTP<CLASS_NAME>::set_size(w, h); \
        return *this; \
    }

#define LVGL_WIDGET_FLUENT_POS(CLASS_NAME) \
    CLASS_NAME& set_pos(int x, int y) { \
        LWidgetCRTP<CLASS_NAME>::set_pos(x, y); \
        return *this; \
    }

#define LVGL_WIDGET_FLUENT_ALIGN(CLASS_NAME) \
    CLASS_NAME& set_align(lv_align_t align_value) { \
        LWidgetCRTP<CLASS_NAME>::set_align(align_value); \
        return *this; \
    }

#define LVGL_WIDGET_FLUENT_GEOMETRY(CLASS_NAME) \
    LVGL_WIDGET_FLUENT_SIZE(CLASS_NAME) \
    LVGL_WIDGET_FLUENT_POS(CLASS_NAME) \
    LVGL_WIDGET_FLUENT_ALIGN(CLASS_NAME)

/**
 * @brief Widget基类，提供RAII内存管理
 * 
 * 所有LVGL对象的C++包装基类，自动管理对象生命周期。
 * 使用移动语义避免拷贝，禁止拷贝构造防止双重释放。
 */
class LWidget {
protected:
    lv_obj_t* m_obj;  ///< 原生LVGL对象指针
    
public:
    explicit LWidget(lv_obj_t* obj) : m_obj(obj) {}
    
    LWidget(LWidget&& other) noexcept : m_obj(other.m_obj) {
        other.m_obj = nullptr;
    }
    
    LWidget& operator=(LWidget&& other) noexcept {
        if (this != &other) {
            if (m_obj) {
                lv_obj_delete(m_obj);  // 同步删除，Ctrl+C安全
            }
            m_obj = other.m_obj;
            other.m_obj = nullptr;
        }
        return *this;
    }
    
    // 禁止拷贝
    LWidget(const LWidget&) = delete;
    LWidget& operator=(const LWidget&) = delete;
    
    /**
     * @brief 析构函数，使用lv_obj_delete同步删除
     * 
     * 同步删除不依赖事件循环，可以安全地在程序退出时调用。
     * 避免Ctrl+C时异步删除导致的崩溃。
     */
    virtual ~LWidget() {
        if (m_obj) {
            lv_obj_delete(m_obj);
            m_obj = nullptr;
        }
    }
    
    lv_obj_t* native() { return m_obj; }
    const lv_obj_t* native() const { return m_obj; }
};

/**
 * @brief CRTP模板基类，提供编译时多态的链式API
 * 
 * @tparam Derived 派生类类型（如LButton, LLabel等）
 * 
 * 使用CRTP模式使得派生类的方法返回Derived&而非LWidget&，
 * 实现编译时多态，避免虚函数开销。
 */
template<typename Derived>
class LWidgetCRTP : public LWidget {
public:
    using LWidget::LWidget;
    using LWidget::operator=;

    Derived& set_size(int w, int h) {
        lv_obj_set_size(m_obj, w, h);
        return static_cast<Derived&>(*this);
    }

    Derived& set_pos(int x, int y) {
        lv_obj_set_pos(m_obj, x, y);
        return static_cast<Derived&>(*this);
    }

    Derived& set_align(lv_align_t align_value) {
        lv_obj_set_align(m_obj, align_value);
        return static_cast<Derived&>(*this);
    }
};

} // namespace lvgl
```

**设计要点：**

1. **RAII原则**：构造时获取资源，析构时释放资源
2. **CRTP模式**：`LWidgetCRTP<Derived>`提供编译时多态，无虚函数开销
3. **移动语义**：支持高效传递，避免不必要的拷贝
4. **禁止拷贝**：防止双重释放导致的崩溃
5. **虚析构函数**：LWidget有虚析构，支持派生类正确析构
6. **同步删除**：使用`lv_obj_delete()`而非`lv_obj_delete_async()`，Ctrl+C安全
7. **成员命名**：使用`m_`前缀（如`m_obj`），遵循Qt风格
8. **sizeof(LWidget) ≈ 16**：虚析构vtable指针(8) + m_obj指针(8)

### 2. LStyle构建器

**文件：** `lvgl++/core/style.hpp`

```cpp
#pragma once
#include "lvgl/lvgl.h"

namespace lvgl {

/**
 * @brief 样式构建器，提供链式API设置样式
 * 
 * 自动管理lv_style_t生命周期，所有方法使用set_前缀。
 */
class LStyle {
private:
    lv_style_t m_style;
    bool m_initialized;
    
public:
    LStyle() : m_initialized(true) {
        lv_style_init(&m_style);
    }
    
    ~LStyle() {
        if (m_initialized) {
            lv_style_reset(&m_style);
        }
    }
    
    // 禁止拷贝
    LStyle(const LStyle&) = delete;
    LStyle& operator=(const LStyle&) = delete;
    
    // 移动语义
    LStyle(LStyle&& other) noexcept;
    LStyle& operator=(LStyle&& other) noexcept;
    
    // 颜色与背景
    LStyle& set_bg_color(uint32_t hex);
    LStyle& set_bg_opa(lv_opa_t opa);
    
    // 边框
    LStyle& set_border(int width, uint32_t color);
    LStyle& set_border_width(int width);
    LStyle& set_border_color(uint32_t hex);
    
    // 圆角与内边距
    LStyle& set_radius(int r);
    LStyle& set_pad_all(int pad);
    LStyle& set_pad_top(int pad);
    LStyle& set_pad_bottom(int pad);
    LStyle& set_pad_left(int pad);
    LStyle& set_pad_right(int pad);
    
    // 文本
    LStyle& set_text_color(uint32_t hex);
    LStyle& set_text_font(const lv_font_t* font);
    
    // 阴影
    LStyle& set_shadow_width(int width);
    LStyle& set_shadow_color(uint32_t hex);
    LStyle& set_shadow_offset_x(int offset);
    LStyle& set_shadow_offset_y(int offset);
    
    // 应用
    void apply_to(lv_obj_t* obj, lv_style_selector_t selector = 0);
    
    // 原生访问
    lv_style_t* native();
    const lv_style_t* native() const;
};

} // namespace lvgl
```

**使用示例：**

```cpp
// 链式调用设置样式（注意set_前缀）
LStyle btn_style;
btn_style.set_bg_color(0x2196F3)
         .set_radius(8)
         .set_pad_all(10)
         .set_shadow_width(5)
         .set_shadow_color(0x000000)
         .apply_to(button);

// 不同状态的样式
LStyle pressed_style;
pressed_style.set_bg_color(0x1976D2).apply_to(button, LV_STATE_PRESSED);
```

### 3. 事件系统

**文件：** `lvgl++/core/event.hpp`

```cpp
#pragma once
#include "lvgl/lvgl.h"
#include <functional>
#include <utility>
#include <type_traits>

namespace lvgl {

/**
 * @brief 类型安全的事件连接函数
 * 
 * 自动注册LV_EVENT_DELETE清理handler，释放堆上回调。
 */
template<typename F>
void connect_event(lv_obj_t* obj, lv_event_code_t code, F&& callback) {
    using CallbackType = typename std::decay<F>::type;
    auto* cb = new CallbackType(std::forward<F>(callback));
    
    // 注册事件回调
    lv_obj_add_event_cb(obj, [](lv_event_t* e) {
        auto* f = static_cast<CallbackType*>(lv_event_get_user_data(e));
        (*f)(e);
    }, code, cb);

    // 注册清理回调：对象删除时释放堆上回调
    lv_obj_add_event_cb(obj, [](lv_event_t* e) {
        auto* f = static_cast<CallbackType*>(lv_event_get_user_data(e));
        delete f;
    }, LV_EVENT_DELETE, cb);
}

/**
 * @brief 简化的点击事件连接（无参数版本）
 */
template<typename F>
void on_clicked(lv_obj_t* obj, F&& callback) {
    using CallbackType = typename std::decay<F>::type;
    auto* cb = new CallbackType(std::forward<F>(callback));
    
    lv_obj_add_event_cb(obj, [](lv_event_t* e) {
        auto* f = static_cast<CallbackType*>(lv_event_get_user_data(e));
        (*f)();  // 无参数调用
    }, LV_EVENT_CLICKED, cb);

    // LV_EVENT_DELETE清理
    lv_obj_add_event_cb(obj, [](lv_event_t* e) {
        auto* f = static_cast<CallbackType*>(lv_event_get_user_data(e));
        delete f;
    }, LV_EVENT_DELETE, cb);
}

/**
 * @brief 值改变事件连接
 */
template<typename F>
void on_value_changed(lv_obj_t* obj, F&& callback) {
    connect_event(obj, LV_EVENT_VALUE_CHANGED, std::forward<F>(callback));
}

} // namespace lvgl
```

**关键特性：LV_EVENT_DELETE清理**

每个`connect_event`调用都会同时注册一个`LV_EVENT_DELETE`事件handler，当LVGL对象被删除时自动释放堆上分配的回调对象。这避免了手动管理回调生命周期的复杂性。

**使用示例：**

```cpp
// 完整事件信息
connect_event(slider, LV_EVENT_VALUE_CHANGED, [](lv_event_t* e) {
    lv_obj_t* slider = lv_event_get_target(e);
    int value = lv_slider_get_value(slider);
    printf("Slider value: %d\n", value);
});

// 简化的点击事件
on_clicked(button, []() {
    printf("Button clicked!\n");
});

// 捕获外部变量
int counter = 0;
on_clicked(button, [&counter]() {
    counter++;
    printf("Count: %d\n", counter);
});
```

### 4. ObjectConfig（Fluent API）

**文件：** `lvgl++/core/config.hpp`

```cpp
#pragma once
#include "lvgl/lvgl.h"
#include "event.hpp"

namespace lvgl {

class LStyle;  // 前置声明

/**
 * @brief 对象配置器，提供链式API配置对象属性
 * 
 * 用于临时配置对象，不管理对象生命周期。
 * style()方法在lvgl_core.hpp中延迟实现（避免循环依赖）。
 */
class ObjectConfig {
private:
    lv_obj_t* m_obj;
    
public:
    explicit ObjectConfig(lv_obj_t* obj) : m_obj(obj) {}
    
    ObjectConfig& size(int w, int h);
    ObjectConfig& pos(int x, int y);
    ObjectConfig& align(lv_align_t a);
    ObjectConfig& flex_flow(lv_flex_flow_t flow);
    ObjectConfig& flex_align(lv_flex_align_t main, 
                             lv_flex_align_t cross, 
                             lv_flex_align_t track);
    
    template<typename F>
    ObjectConfig& on_clicked(F&& callback);
    
    // style()声明 - 在lvgl_core.hpp中实现
    ObjectConfig& style(LStyle& style, lv_style_selector_t selector = 0);
    
    lv_obj_t* get();
};

inline ObjectConfig configure(lv_obj_t* obj) {
    return ObjectConfig(obj);
}

} // namespace lvgl
```

**使用示例：**

```cpp
// 链式配置对象
auto btn = configure(lv_button_create(parent))
    .size(120, 50)
    .pos(100, 100)
    .on_clicked([](auto*) { printf("Clicked!\n"); })
    .get();

// 配置布局容器
auto container = configure(lv_obj_create(parent))
    .size(300, 400)
    .flex_flow(LV_FLEX_FLOW_COLUMN)
    .flex_align(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START)
    .get();
```

### 5. SignalHandler（信号处理）

**文件：** `lvgl++/core/signal_handler.hpp`

```cpp
#pragma once
#include <signal.h>
#include <atomic>
#include <cstdio>

namespace lvgl {

/**
 * @brief 信号处理器，用于优雅退出
 * 
 * 注册SIGINT/SIGTERM处理，配合事件循环实现安全退出。
 * 配合lv_obj_delete()同步删除，避免Ctrl+C崩溃。
 */
class SignalHandler {
public:
    SignalHandler();  // 注册SIGINT/SIGTERM
    bool is_running() const;
    void request_exit();
};

} // namespace lvgl
```

**使用示例：**

```cpp
#include "lvgl++/core/signal_handler.hpp"

lvgl::SignalHandler signal_handler;
while (signal_handler.is_running()) {
    lv_timer_handler();
    usleep(5000);
}
// 退出后，LWidget析构函数调用lv_obj_delete()同步安全删除
```

## 统一入口头文件

**文件：** `lvgl++/lvgl_core.hpp`

```cpp
#pragma once

#include "core/widget.hpp"
#include "core/style.hpp"
#include "core/event.hpp"
#include "core/config.hpp"
// 注意：signal_handler.hpp 不在此处包含，需要时单独引入

namespace lvgl {

// 版本信息
constexpr int VERSION_MAJOR = 1;
constexpr int VERSION_MINOR = 0;
constexpr int VERSION_PATCH = 0;

// ObjectConfig::style() 延迟实现（需要LStyle完整定义）
inline ObjectConfig& ObjectConfig::style(LStyle& s, lv_style_selector_t selector) {
    s.apply_to(m_obj, selector);
    return *this;
}

} // namespace lvgl
```

**设计要点：**

- `signal_handler.hpp`不包含在`lvgl_core.hpp`中，需要时由用户手动引入
- `ObjectConfig::style()`方法延迟到`lvgl_core.hpp`中实现，因为`config.hpp`只有`LStyle`的前置声明

## 性能分析

### 内存开销

| 组件 | 栈开销 | 说明 |
|------|--------|------|
| LWidget | ~16字节 | vtable指针(8) + m_obj指针(8) |
| LStyle | 取决于LVGL | lv_style_t + m_initialized |
| ObjectConfig | 8字节 | m_obj指针 |
| Event回调 | sizeof(F) | 每个回调堆分配 |

### 运行时开销

- **LWidget构造/析构**：1次函数调用（内联后为0）
- **LStyle链式调用**：编译期优化为连续调用
- **Event连接**：1次堆分配 + 1次LVGL API调用 + 1次DELETE handler注册
- **ObjectConfig**：完全内联，零开销

### 编译优化

```cpp
// 编译器优化示例
LStyle().set_bg_color(0xFF0000).set_radius(8).apply_to(btn);

// 优化后等价于：
lv_style_t style;
lv_style_init(&style);
lv_style_set_bg_color(&style, lv_color_hex(0xFF0000));
lv_style_set_radius(&style, 8);
lv_obj_add_style(btn, &style, 0);
```

## 使用示例

### 完整示例

```cpp
#include "lvgl++/lvgl_core.hpp"
#include "lvgl++/core/signal_handler.hpp"
using namespace lvgl;

int main() {
    lv_init();
    // ... 显示初始化 ...
    
    SignalHandler signal_handler;
    
    // 创建按钮
    auto btn = configure(lv_button_create(lv_scr_act()))
        .size(120, 50)
        .align(LV_ALIGN_CENTER)
        .on_clicked([](auto*) {
            printf("Button clicked!\n");
        })
        .get();
    
    // 设置样式
    LStyle btn_style;
    btn_style.set_bg_color(0x2196F3)
             .set_radius(8)
             .set_pad_all(10)
             .apply_to(btn);
    
    // 按下状态样式
    LStyle pressed_style;
    pressed_style.set_bg_color(0x1976D2)
                 .apply_to(btn, LV_STATE_PRESSED);
    
    // 创建标签
    auto label = lv_label_create(btn);
    lv_label_set_text(label, "Click Me");
    lv_obj_center(label);
    
    // 主循环
    while (signal_handler.is_running()) {
        lv_timer_handler();
        usleep(5000);
    }
    
    return 0;
    // btn_style, pressed_style, label等自动析构
    // lv_obj_delete()同步删除，安全退出
}
```

## 相关文档

- [layer2-design.md](./layer2-design.md) - Layer 2 组件库设计
- [json-ui-format.md](./json-ui-format.md) - JSON UI 格式规范
