# Layer 1: 智能封装层详细设计

## 概述

Layer 1是LVGL++的基础层，提供智能指针、类型安全的事件系统和链式API，解决原生C API的核心痛点。

**设计目标：**
- 零运行时开销
- RAII自动内存管理
- 类型安全的事件处理
- 链式API提高可读性
- 与原生C API完全兼容

## 文件结构

```
lvgl++/
├── core/
│   ├── widget.hpp          # Widget基类
│   ├── style.hpp           # Style构建器
│   ├── event.hpp           # 事件系统
│   ├── config.hpp          # ObjectConfig
│   └── types.hpp           # 类型定义
└── lvgl_core.hpp           # 统一入口
```

## 核心组件设计

### 1. Widget基类

**文件：** `lvgl++/core/widget.hpp`

```cpp
#pragma once
#include "lvgl/lvgl.h"
#include <utility>

namespace lvgl {

/**
 * @brief Widget基类，提供RAII内存管理
 * 
 * 所有LVGL对象的C++包装基类，自动管理对象生命周期。
 * 使用移动语义避免拷贝，禁止拷贝构造防止双重释放。
 */
class Widget {
protected:
    lv_obj_t* obj_;  ///< 原生LVGL对象指针
    
public:
    /**
     * @brief 构造函数
     * @param obj 原生LVGL对象指针
     */
    explicit Widget(lv_obj_t* obj) : obj_(obj) {}
    
    /**
     * @brief 移动构造函数
     * @param other 被移动的对象
     */
    Widget(Widget&& other) noexcept : obj_(other.obj_) {
        other.obj_ = nullptr;
    }
    
    /**
     * @brief 移动赋值运算符
     * @param other 被移动的对象
     * @return 当前对象引用
     */
    Widget& operator=(Widget&& other) noexcept {
        if (this != &other) {
            if (obj_) {
                lv_obj_delete_async(obj_);
            }
            obj_ = other.obj_;
            other.obj_ = nullptr;
        }
        return *this;
    }
    
    // 禁止拷贝
    Widget(const Widget&) = delete;
    Widget& operator=(const Widget&) = delete;
    
    /**
     * @brief 析构函数，自动释放LVGL对象
     */
    virtual ~Widget() {
        if (obj_) {
            lv_obj_delete_async(obj_);
        }
    }
    
    /**
     * @brief 获取原生LVGL对象指针
     * @return lv_obj_t* 原生指针
     */
    lv_obj_t* native() { return obj_; }
    
    /**
     * @brief 获取原生LVGL对象指针（const版本）
     * @return const lv_obj_t* 原生指针
     */
    const lv_obj_t* native() const { return obj_; }
    
    /**
     * @brief 检查对象是否有效
     * @return true 对象有效，false 对象为空
     */
    bool is_valid() const { return obj_ != nullptr; }
    
    /**
     * @brief 释放对象所有权，返回原生指针
     * @return lv_obj_t* 原生指针
     * 
     * 调用后Widget不再管理该对象的生命周期
     */
    lv_obj_t* release() {
        lv_obj_t* tmp = obj_;
        obj_ = nullptr;
        return tmp;
    }
};

} // namespace lvgl
```

**设计要点：**

1. **RAII原则**：构造时获取资源，析构时释放资源
2. **移动语义**：支持高效传递，避免不必要的拷贝
3. **禁止拷贝**：防止双重释放导致的崩溃
4. **虚析构函数**：支持多态，派生类可以正确析构
5. **release()方法**：需要转移所有权时使用

### 2. Style构建器

**文件：** `lvgl++/core/style.hpp`

```cpp
#pragma once
#include "lvgl/lvgl.h"

namespace lvgl {

/**
 * @brief 样式构建器，提供链式API设置样式
 * 
 * 自动管理lv_style_t生命周期，支持链式调用。
 */
class Style {
private:
    lv_style_t style_;
    bool initialized_;
    
public:
    /**
     * @brief 构造函数，初始化样式
     */
    Style() : initialized_(true) {
        lv_style_init(&style_);
    }
    
    /**
     * @brief 析构函数，重置样式
     */
    ~Style() {
        if (initialized_) {
            lv_style_reset(&style_);
        }
    }
    
    // 禁止拷贝
    Style(const Style&) = delete;
    Style& operator=(const Style&) = delete;
    
    // 移动语义
    Style(Style&& other) noexcept 
        : style_(other.style_), initialized_(other.initialized_) {
        other.initialized_ = false;
    }
    
    /**
     * @brief 设置背景颜色
     * @param hex 十六进制颜色值（如0xFF0000表示红色）
     * @return Style& 当前对象引用，支持链式调用
     */
    Style& bg_color(uint32_t hex) {
        lv_style_set_bg_color(&style_, lv_color_hex(hex));
        return *this;
    }
    
    /**
     * @brief 设置背景不透明度
     * @param opa 不透明度（0-255）
     * @return Style& 当前对象引用
     */
    Style& bg_opa(lv_opa_t opa) {
        lv_style_set_bg_opa(&style_, opa);
        return *this;
    }
    
    /**
     * @brief 设置边框
     * @param width 边框宽度
     * @param color 边框颜色（十六进制）
     * @return Style& 当前对象引用
     */
    Style& border(int width, uint32_t color) {
        lv_style_set_border_width(&style_, width);
        lv_style_set_border_color(&style_, lv_color_hex(color));
        return *this;
    }
    
    /**
     * @brief 设置圆角半径
     * @param r 半径值
     * @return Style& 当前对象引用
     */
    Style& radius(int r) {
        lv_style_set_radius(&style_, r);
        return *this;
    }
    
    /**
     * @brief 设置所有方向的内边距
     * @param pad 内边距值
     * @return Style& 当前对象引用
     */
    Style& pad_all(int pad) {
        lv_style_set_pad_all(&style_, pad);
        return *this;
    }
    
    /**
     * @brief 设置文本颜色
     * @param hex 十六进制颜色值
     * @return Style& 当前对象引用
     */
    Style& text_color(uint32_t hex) {
        lv_style_set_text_color(&style_, lv_color_hex(hex));
        return *this;
    }
    
    /**
     * @brief 设置文本字体
     * @param font 字体指针
     * @return Style& 当前对象引用
     */
    Style& text_font(const lv_font_t* font) {
        lv_style_set_text_font(&style_, font);
        return *this;
    }
    
    /**
     * @brief 设置阴影宽度
     * @param width 阴影宽度
     * @return Style& 当前对象引用
     */
    Style& shadow_width(int width) {
        lv_style_set_shadow_width(&style_, width);
        return *this;
    }
    
    /**
     * @brief 设置阴影颜色
     * @param hex 十六进制颜色值
     * @return Style& 当前对象引用
     */
    Style& shadow_color(uint32_t hex) {
        lv_style_set_shadow_color(&style_, lv_color_hex(hex));
        return *this;
    }
    
    /**
     * @brief 应用样式到对象
     * @param obj 目标对象
     * @param selector 样式选择器（默认0表示所有状态）
     */
    void apply_to(lv_obj_t* obj, lv_style_selector_t selector = 0) {
        lv_obj_add_style(obj, &style_, selector);
    }
    
    /**
     * @brief 获取原生样式指针
     * @return lv_style_t* 原生样式指针
     */
    lv_style_t* native() { return &style_; }
};

} // namespace lvgl
```

**使用示例：**

```cpp
// 链式调用设置样式
Style btn_style;
btn_style.bg_color(0x2196F3)
         .radius(8)
         .pad_all(10)
         .shadow_width(5)
         .shadow_color(0x000000)
         .apply_to(button);

// 不同状态的样式
Style pressed_style;
pressed_style.bg_color(0x1976D2).apply_to(button, LV_STATE_PRESSED);
```

### 3. 事件系统

**文件：** `lvgl++/core/event.hpp`

```cpp
#pragma once
#include "lvgl/lvgl.h"
#include <functional>
#include <utility>

namespace lvgl {

/**
 * @brief 类型安全的事件连接函数
 * 
 * @tparam F 回调函数类型（自动推导）
 * @param obj 目标对象
 * @param code 事件代码
 * @param callback 回调函数（支持lambda、函数指针、函数对象）
 * 
 * @example
 * connect_event(btn, LV_EVENT_CLICKED, [](lv_event_t* e) {
 *     printf("Button clicked!\n");
 * });
 */
template<typename F>
void connect_event(lv_obj_t* obj, lv_event_code_t code, F&& callback) {
    // 在堆上分配回调对象
    auto* cb = new typename std::decay<F>::type(std::forward<F>(callback));
    
    // 注册LVGL事件回调
    lv_obj_add_event_cb(obj, [](lv_event_t* e) {
        // 从user_data获取回调对象
        auto* f = static_cast<typename std::decay<F>::type*>(
            lv_event_get_user_data(e)
        );
        // 调用回调
        (*f)(e);
    }, code, cb);
}

/**
 * @brief 简化的点击事件连接
 * 
 * @tparam F 回调函数类型
 * @param obj 目标对象
 * @param callback 回调函数（无参数版本）
 */
template<typename F>
void on_clicked(lv_obj_t* obj, F&& callback) {
    auto* cb = new typename std::decay<F>::type(std::forward<F>(callback));
    
    lv_obj_add_event_cb(obj, [](lv_event_t* e) {
        auto* f = static_cast<typename std::decay<F>::type*>(
            lv_event_get_user_data(e)
        );
        (*f)();  // 无参数调用
    }, LV_EVENT_CLICKED, cb);
}

/**
 * @brief 值改变事件连接
 * 
 * @tparam F 回调函数类型
 * @param obj 目标对象
 * @param callback 回调函数
 */
template<typename F>
void on_value_changed(lv_obj_t* obj, F&& callback) {
    connect_event(obj, LV_EVENT_VALUE_CHANGED, std::forward<F>(callback));
}

} // namespace lvgl
```

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

/**
 * @brief 对象配置器，提供链式API配置对象属性
 * 
 * 用于临时配置对象，不管理对象生命周期。
 */
class ObjectConfig {
private:
    lv_obj_t* obj_;
    
public:
    /**
     * @brief 构造函数
     * @param obj 要配置的对象
     */
    explicit ObjectConfig(lv_obj_t* obj) : obj_(obj) {}
    
    /**
     * @brief 设置大小
     * @param w 宽度
     * @param h 高度
     * @return ObjectConfig& 当前对象引用
     */
    ObjectConfig& size(int w, int h) {
        lv_obj_set_size(obj_, w, h);
        return *this;
    }
    
    /**
     * @brief 设置位置
     * @param x X坐标
     * @param y Y坐标
     * @return ObjectConfig& 当前对象引用
     */
    ObjectConfig& pos(int x, int y) {
        lv_obj_set_pos(obj_, x, y);
        return *this;
    }
    
    /**
     * @brief 设置对齐方式
     * @param a 对齐方式
     * @return ObjectConfig& 当前对象引用
     */
    ObjectConfig& align(lv_align_t a) {
        lv_obj_set_align(obj_, a);
        return *this;
    }
    
    /**
     * @brief 设置Flex布局流向
     * @param flow 流向
     * @return ObjectConfig& 当前对象引用
     */
    ObjectConfig& flex_flow(lv_flex_flow_t flow) {
        lv_obj_set_flex_flow(obj_, flow);
        return *this;
    }
    
    /**
     * @brief 设置Flex对齐
     * @param main 主轴对齐
     * @param cross 交叉轴对齐
     * @param track 轨道对齐
     * @return ObjectConfig& 当前对象引用
     */
    ObjectConfig& flex_align(lv_flex_align_t main, 
                             lv_flex_align_t cross, 
                             lv_flex_align_t track) {
        lv_obj_set_flex_align(obj_, main, cross, track);
        return *this;
    }
    
    /**
     * @brief 添加点击事件
     * @tparam F 回调函数类型
     * @param callback 回调函数
     * @return ObjectConfig& 当前对象引用
     */
    template<typename F>
    ObjectConfig& on_clicked(F&& callback) {
        connect_event(obj_, LV_EVENT_CLICKED, std::forward<F>(callback));
        return *this;
    }
    
    /**
     * @brief 应用样式
     * @param style 样式对象
     * @param selector 样式选择器
     * @return ObjectConfig& 当前对象引用
     */
    ObjectConfig& style(Style& style, lv_style_selector_t selector = 0) {
        style.apply_to(obj_, selector);
        return *this;
    }
    
    /**
     * @brief 获取原生对象指针
     * @return lv_obj_t* 原生指针
     */
    lv_obj_t* get() { return obj_; }
};

/**
 * @brief 辅助函数，创建ObjectConfig
 * @param obj 要配置的对象
 * @return ObjectConfig 配置器对象
 */
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

## 统一入口头文件

**文件：** `lvgl++/lvgl_core.hpp`

```cpp
#pragma once

/**
 * @file lvgl_core.hpp
 * @brief LVGL++ Layer 1统一入口
 * 
 * 包含所有Layer 1核心组件
 */

#include "core/widget.hpp"
#include "core/style.hpp"
#include "core/event.hpp"
#include "core/config.hpp"

namespace lvgl {

// 版本信息
constexpr int VERSION_MAJOR = 1;
constexpr int VERSION_MINOR = 0;
constexpr int VERSION_PATCH = 0;

} // namespace lvgl
```

## 性能分析

### 内存开销

| 组件 | 栈开销 | 堆开销 | 说明 |
|------|--------|--------|------|
| Widget | 8字节 | 0 | 仅存储指针 |
| Style | 取决于LVGL | 0 | lv_style_t大小 |
| ObjectConfig | 8字节 | 0 | 临时对象 |
| Event回调 | 0 | sizeof(F) | 每个回调 |

### 运行时开销

- **Widget构造/析构**：1次函数调用（内联后为0）
- **Style链式调用**：编译期优化为连续调用
- **Event连接**：1次堆分配 + 1次LVGL API调用
- **ObjectConfig**：完全内联，零开销

### 编译优化

```cpp
// 编译器优化示例
Style().bg_color(0xFF0000).radius(8).apply_to(btn);

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
using namespace lvgl;

void create_ui() {
    // 创建按钮
    auto btn = configure(lv_button_create(lv_scr_act()))
        .size(120, 50)
        .align(LV_ALIGN_CENTER)
        .on_clicked([](auto*) {
            printf("Button clicked!\n");
        })
        .get();
    
    // 设置样式
    Style btn_style;
    btn_style.bg_color(0x2196F3)
             .radius(8)
             .pad_all(10)
             .apply_to(btn);
    
    // 按下状态样式
    Style pressed_style;
    pressed_style.bg_color(0x1976D2)
                 .apply_to(btn, LV_STATE_PRESSED);
    
    // 创建标签
    auto label = lv_label_create(btn);
    lv_label_set_text(label, "Click Me");
    lv_obj_center(label);
}
```

## 下一步

- [03-layer2-design.md](./03-layer2-design.md) - Layer 2组件库设计
- [04-implementation-plan.md](./04-implementation-plan.md) - 实施计划
