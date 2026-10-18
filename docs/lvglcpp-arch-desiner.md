# LVGL++ 系统架构设计文档

**版本:** 2.0.0  
**日期:** 2026-04-01  
**状态:** 可用（核心功能已完成；测试与文档持续完善中）

---

## 1. 项目概述

### 1.1 项目定位

LVGL++ 是一个面向嵌入式 GUI 开发的**全链路工具链**，围绕 LVGL 9.2 构建，提供从可视化设计到代码生成的完整开发闭环：

```
┌─────────────┐     ┌──────────┐     ┌──────────────┐     ┌────────────┐
│  Qt Designer │     │ VSCode   │     │  luic 编译器  │     │ C++ 运行时  │
│  (帧缓冲区   │────▶│ Extension│────▶│  JSON → C++  │────▶│  LVGL++    │
│   LVGL渲染)  │     │ (启动器)  │     │  代码生成     │     │  Wrapper   │
└─────────────┘     └──────────┘     └──────────────┘     └────────────┘
        │                  │                                       │
        ▼                  ▼                                       ▼
   .lui (JSON)        调起设计进程                             嵌入式设备渲染
                    + 自动触发 luic
```

### 1.2 解决的核心问题

| 痛点 | 解决方案 |
|------|----------|
| LVGL C API 冗长、易错 | C++ RAII 封装 + 链式 API，代码量减少 75% |
| 手写 UI 代码效率低 | JSON 描述 + 编译器自动生成 C++ |
| UI 布局调整需反复编译 | 可视化拖拽设计，实时预览 |
| 事件回调缺乏类型安全 | 模板化事件系统，编译时检查 |

---

## 2. 封装思想与设计哲学

> **"封装的本质不是隐藏 C API，而是在正确的抽象层次上重新表达 C API 的意图。"**

LVGL++ 的封装不是简单的 C++ "翻译层"——它是一套经过深思熟虑的抽象体系，每一层都有明确的存在理由。

### 2.1 为什么封装 LVGL？

LVGL 是一个优秀的 C 语言嵌入式 GUI 库，但它的 C API 天然带有以下问题：

```c
// 典型的 LVGL C 代码 — 创建一个带样式的按钮
lv_obj_t* btn = lv_button_create(lv_screen_active());       // 1. 手动管理生命周期
lv_obj_set_size(btn, 120, 50);                              // 2. 命令式调用，无法链式
lv_obj_set_pos(btn, 100, 200);                              // 3. 每次都传 btn
lv_obj_add_event_cb(btn, callback, LV_EVENT_CLICKED, NULL); // 4. 回调是 C 函数指针

static lv_style_t style;                                    // 5. 手动 init/reset
lv_style_init(&style);
lv_style_set_bg_color(&style, lv_color_hex(0x2196F3));
lv_style_set_radius(&style, 8);
lv_obj_add_style(btn, &style, 0);
// 6. 没有 style reset → 内存泄漏
// 7. 忘了 lv_obj_delete(btn) → 泄漏
```

**六个核心问题：**

| # | 问题 | 本质 |
|---|------|------|
| 1 | **生命周期无保障** | `lv_obj_t*` 是裸指针，忘记 delete 就泄漏 |
| 2 | **API 不连贯** | 每次调用都重复传对象，无法链式表达 |
| 3 | **无类型区分** | 按钮、滑块、标签都是 `lv_obj_t*`，编译器无法区分 |
| 4 | **回调是 C 风格** | 无法用 lambda，需要静态函数 + user_data 转发 |
| 5 | **样式手动管理** | `lv_style_init` / `lv_style_reset` 必须配对 |
| 6 | **代码冗长** | 完成同样功能需要 3-5 倍代码量 |

### 2.2 封装的五大原则

```
┌───────────────────────────────────────────────────────────────┐
│                     LVGL++ 封装原则体系                        │
│                                                               │
│   ┌──────────┐  ┌──────────┐  ┌──────────┐                   │
│   │ RAII     │  │ 零成本   │  │ 类型安全  │                   │
│   │ 自动管理  │  │ 抽象     │  │ 编译时检查│                   │
│   └──────────┘  └──────────┘  └──────────┘                   │
│                                                               │
│   ┌──────────┐  ┌──────────────────────┐                     │
│   │ 流畅API  │  │ 向下兼容             │                     │
│   │ 链式调用  │  │ native() 桥接回 C    │                     │
│   └──────────┘  └──────────────────────┘                     │
└───────────────────────────────────────────────────────────────┘
```

#### 原则一：RAII — 资源获取即初始化

**核心思想：** 让对象的生命周期等于 C++ 对象的生命周期。构造即获取，析构即释放。

```cpp
// LVGL 的 C API — 你必须记住配对调用
lv_obj_t* btn = lv_button_create(parent);  // 获取
// ... 100 行代码 ...
lv_obj_delete(btn);  // 必须手动释放，忘了就泄漏

// LVGL++ — 生命周期自动管理
{
    LButton btn;          // 构造 → lv_button_create()
    btn.set_text("Click");
}                         // 离开作用域 → 析构 → lv_obj_delete()
                           // 不可能忘记，不可能泄漏
```

**实现原理：**

```cpp
class LWidget {
protected:
    lv_obj_t* m_obj;       // 持有原生指针

public:
    explicit LWidget(lv_obj_t* obj) : m_obj(obj) {}  // 获取所有权

    virtual ~LWidget() {                              // 析构自动释放
        if (m_obj) {
            lv_obj_delete(m_obj);    // 同步删除，不用 lv_obj_delete_async
            m_obj = nullptr;         // 避免 Ctrl+C 时的异步崩溃
        }
    }
};
```

**为什么用 `lv_obj_delete` 而非 `lv_obj_delete_async`？**
- `lv_obj_delete_async` 依赖事件循环，在程序退出（Ctrl+C）时事件循环已停止，导致崩溃
- `lv_obj_delete` 是同步删除，不依赖事件循环，可以安全地在析构中调用
- 这是从实际 Ctrl+C 崩溃 bug 中总结出的设计决策

**移动语义 — 转移所有权而非共享：**

```cpp
// 移动构造 — 所有权转移
LWidget(LWidget&& other) noexcept : m_obj(other.m_obj) {
    other.m_obj = nullptr;  // 源对象置空，防止双重释放
}

// 移动赋值 — 先释放旧的，再接收新的
LWidget& operator=(LWidget&& other) noexcept {
    if (this != &other) {
        if (m_obj) lv_obj_delete(m_obj);  // 释放当前资源
        m_obj = other.m_obj;
        other.m_obj = nullptr;
    }
    return *this;
}

// 禁止拷贝 — 防止两个 C++ 对象管理同一个 LVGL 对象
LWidget(const LWidget&) = delete;
LWidget& operator=(const LWidget&) = delete;
```

**为什么禁止拷贝？** 因为 LVGL 对象的父子树结构意味着一个对象只能有一个父对象。如果允许拷贝，两个 C++ 对象持有同一个 `lv_obj_t*`，第一个析构后第二个会 use-after-free。

#### 原则二：零成本抽象

**核心思想：** C++ 封装不应该引入任何运行时开销。编译后的代码应该和手写 C 代码一样快。

```
C++ 源码                     编译器优化后              等价的 C 代码
─────────                    ──────────               ──────────
LButton btn;           →    lv_button_create()    →   lv_obj_t* btn = lv_button_create();
btn.set_text("OK");    →    lv_label_set_text()   →   lv_label_set_text(label, "OK");
btn.set_size(100,50);  →    lv_obj_set_size()     →   lv_obj_set_size(btn, 100, 50);
```

**如何做到零成本：**

1. **Header-Only**: 所有代码在头文件中，编译器可以看到完整实现，进行内联优化
2. **CRTP 替代虚函数**: 编译时多态，无虚表开销（见下节）
3. **内联构造/析构**: `lv_obj_create` / `lv_obj_delete` 直接调用，无中间层
4. **模板事件连接**: 编译时生成回调适配代码，无运行时类型擦除
5. **极小内存开销**: `LWidget` 含 vtable 指针 + `m_obj`，`sizeof(LWidget) ≈ 16` 字节

```cpp
// LWidget 含有：vtable 指针 (8 字节) + m_obj (8 字节) ≈ 16 字节
// LButton 额外只有一个 m_label 指针 (8 字节)
// LSlider 没有额外成员
```

> **注意：** 由于 `LWidget` 使用了 `virtual` 析构函数，它拥有一个 vtable 指针，因此 `sizeof(LWidget) ≈ 16` 字节（而非 `sizeof(void*) == 8`）。虚析构函数是必要的——它确保子类析构时正确调用基类的 `lv_obj_delete` 清理逻辑。

#### 原则三：类型安全 — 让编译器帮你抓 bug

**核心思想：** 在 LVGL C API 中，按钮、标签、滑块都是 `lv_obj_t*`，编译器无法区分。LVGL++ 给每种组件一个独立的 C++ 类型。

```c
// C API — 编译器不知道这是按钮
lv_obj_t* btn = lv_button_create(parent);
lv_label_set_text(btn, "Hello");   // ✅ 编译通过！但运行时行为未定义
                                     // 按钮不是标签，但类型系统无法阻止你

lv_obj_t* slider = lv_slider_create(parent);
lv_button_create(slider);          // ✅ 编译通过！但滑块上不能创建按钮子对象
```

```cpp
// LVGL++ — 编译器知道每种组件能做什么
LButton btn;
btn.set_text("Hello");             // ✅ LButton 有 set_text()
btn.set_range(0, 100);             // ❌ 编译错误！LButton 没有 set_range()

LSlider slider;
slider.set_range(0, 100);          // ✅ LSlider 有 set_range()
slider.set_text("Hello");          // ❌ 编译错误！LSlider 没有 set_text()
```

**CRTP 实现编译时类型安全的链式调用：**

```cpp
// 传统虚函数方案的问题：
class LWidget {
public:
    virtual LWidget& set_size(int w, int h) {  // 返回 LWidget&
        lv_obj_set_size(m_obj, w, h);
        return *this;
    }
};

class LButton : public LWidget {
public:
    LButton& set_text(const char* t) { return *this; }
};

// 链式调用中断！
btn.set_size(100, 50)   // 返回 LWidget&
    .set_text("OK");    // ❌ LWidget 没有 set_text()！类型被截断了！
```

```cpp
// CRTP 方案 — 解决类型截断：
template<typename Derived>
class LWidgetCRTP : public LWidget {
public:
    Derived& set_size(int w, int h) {           // 返回 Derived&
        lv_obj_set_size(m_obj, w, h);
        return static_cast<Derived&>(*this);     // 精确类型转换
    }
};

class LButton : public LWidgetCRTP<LButton> {   // 把自己传给基类
public:
    LButton& set_text(const char* t) { return *this; }
};

// 链式调用完美工作！
btn.set_size(100, 50)   // 返回 LButton&
    .set_text("OK");    // ✅ LButton 有 set_text()！
```

**CRTP 的本质：** 父类模板参数化子类类型，使父类方法能返回子类的精确引用。这是**编译时多态**——没有虚表、没有运行时开销，但有完整的类型安全。

```
┌─────────────────────────────────────────────────────────┐
│                CRTP 类型传递链                            │
│                                                         │
│   LWidget                                               │
│     ▲                                                   │
│     │ 继承                                               │
│     │                                                   │
│   LWidgetCRTP<LButton>   ← LButton 作为模板参数传入       │
│     ▲                                                   │
│     │ 继承                                               │
│     │                                                   │
│   LButton                                                │
│     │                                                   │
│     │ set_size() 返回: static_cast<LButton&>(*this)      │
│     │            而不是: LWidget&                        │
│     │                                                   │
│     └─▶ .set_text() ✅ 类型完整，链式不截断               │
└─────────────────────────────────────────────────────────┘
```

#### 原则四：流畅 API (Fluent API) — 代码即文档

**核心思想：** 方法返回 `*this` 的引用，使多个调用可以链式写在一起。代码读起来像自然语言描述。

```cpp
// C API — 命令式，每次都指定目标
lv_obj_t* btn = lv_button_create(parent);
lv_obj_set_size(btn, 120, 50);
lv_label_set_text(label, "Login");
lv_style_t s; lv_style_init(&s);
lv_style_set_bg_color(&s, lv_color_hex(0x2196F3));
lv_style_set_radius(&s, 8);
lv_obj_add_style(btn, &s, 0);
lv_obj_add_event_cb(btn, on_click, LV_EVENT_CLICKED, NULL);

// LVGL++ — 声明式，链式表达意图
LButton btn;
btn.set_text("Login")
   .set_size(120, 50)
   .primary()              // 一键应用主题色
   .on_click([](auto*) {   // 内联 lambda
       printf("Login!\n");
   });
```

**设计方法命名的哲学：**

```cpp
// 命名原则：简洁 > 冗长，动词 > 名词，语义 > 实现

// ✅ 我们的选择
btn.set_text("OK");       // 语义清晰
btn.primary();             // 意图明确：应用主题色
btn.on_click([]{...});     // 自然语言顺序

// ❌ 可能的其他选择
btn.setText("OK");         // Java 风格，不必要的大小写混用
btn.applyPrimaryTheme();   // 过于冗长
btn.addEventListener(...); // 冗长且不直观
```

**预设主题样式 — 减少重复：**

```cpp
LButton& primary() {  // 蓝色主题
    LStyle s;
    s.set_bg_color(0x2196F3).set_radius(8).set_pad_all(10).apply_to(m_obj);
    return *this;
}
LButton& danger() {   // 红色主题
    LStyle s;
    s.set_bg_color(0xF44336).set_radius(8).set_pad_all(10).apply_to(m_obj);
    return *this;
}
LButton& success() {  // 绿色主题
    LStyle s;
    s.set_bg_color(0x4CAF50).set_radius(8).set_pad_all(10).apply_to(m_obj);
    return *this;
}

// 使用：
btn.primary();    // 一行代替六行样式代码
btn.danger();     // 语义化，意图明确
```

#### 原则五：向下兼容 — native() 桥接

**核心思想：** 封装不应该成为牢笼。当 C++ API 不够用时，必须能无缝回到 C API。

```cpp
// 所有组件都有 native() 方法
LButton btn;
lv_obj_t* raw = btn.native();       // 获取原生指针

// 可以直接调用任何 LVGL C API
lv_obj_add_flag(raw, LV_OBJ_FLAG_HIDDEN);
lv_obj_move_to_index(raw, 2);
lv_obj_set_style_anim_time(raw, 500, 0);

// 甚至可以传给第三方库
some_lvgl_plugin_do_something(raw);
```

**为什么这很重要：**
- LVGL 有数百个 API，不可能全部封装
- 嵌入式项目中经常需要直接操作底层
- 迁移成本为零：可以逐步引入 LVGL++，不必一次性重写

### 2.3 封装的层次结构

LVGL++ 的封装不是一坨代码，而是**分层的抽象**，每一层构建在上一层之上：

```
┌─────────────────────────────────────────────────────────────┐
│ Layer 3: 代码生成 (luic)                                     │
│ JSON → IR → C++ 类                                          │
│ 自动生成 setup() + bind_events() + virtual 事件槽            │
│                                                             │
│ 用户只需写业务逻辑，不用写任何 UI 构建代码                      │
├─────────────────────────────────────────────────────────────┤
│ Layer 2: 组件库 (35 Widgets + 4 Layouts = 39 组件)            │
│ LButton, LLabel, LSlider, LVBoxLayout, LGridLayout, ...     │
│ 每个组件封装特定的 LVGL 对象类型 + 特有 API                    │
│                                                             │
│ 提供类型安全的组件级 API                                      │
├─────────────────────────────────────────────────────────────┤
│ Layer 1: 核心基础设施                                        │
│ ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐        │
│ │ LWidget  │ │ LStyle   │ │ Event    │ │ Object   │        │
│ │ (RAII)   │ │ (Builder)│ │ (Lambda) │ │ Config   │        │
│ └──────────┘ └──────────┘ └──────────┘ └──────────┘        │
│                                                             │
│ 提供最基础的抽象：生命周期、样式、事件、配置                     │
├─────────────────────────────────────────────────────────────┤
│ Layer 0: LVGL 9.2 C API (未修改)                            │
└─────────────────────────────────────────────────────────────┘
```

**每一层的封装目标：**

| 层级 | 封装了什么 | 解决了什么问题 |
|------|-----------|--------------|
| **Layer 0** | (LVGL 原始 API) | — |
| **Layer 1** | 生命周期 (RAII) + 样式 (Builder) + 事件 (Lambda) | 内存泄漏、代码冗长、回调不便 |
| **Layer 2** | 组件类型 (CRTP) + 特有 API | 无类型区分、API 不直观 |
| **Layer 3** | 整个 UI 树 (JSON → C++) | 重复的 UI 构建代码 |

### 2.4 事件系统的封装原理

LVGL 的 C 回调机制通过 `user_data` 传递上下文。LVGL++ 用模板和堆分配将 C++ lambda 桥接到这个机制：

```
C++ Lambda          堆上的副本           C 函数指针          LVGL 事件系统
──────────         ──────────          ──────────          ──────────
                                     ┌──────────────┐
[捕获变量](e){  ──new复制──▶  cb_ptr  │              │
  业务逻辑                       │  lambda桥接   │──LVGL调用──▶  lv_event_t*
}                                │  (类型擦除)   │
                                     │              │
                                     └──────────────┘
```

```cpp
// 核心实现 — 模板化的 C→C++ 桥接
template<typename F>
void connect_event(lv_obj_t* obj, lv_event_code_t code, F&& callback) {
    // 1. 将 lambda 复制到堆上（因为原 lambda 可能在栈上销毁）
    auto* cb = new typename std::decay<F>::type(std::forward<F>(callback));

    // 2. 注册一个静态 C 函数作为 LVGL 回调
    //    通过 user_data 传递堆上的 lambda 指针
    lv_obj_add_event_cb(obj, [](lv_event_t* e) {
        // 3. 从 user_data 恢复 lambda 指针
        auto* f = static_cast<typename std::decay<F>::type*>(
            lv_event_get_user_data(e)
        );
        // 4. 调用 lambda
        (*f)(e);
    }, code, cb);  // cb 作为 user_data 传入
}
```

**为什么用 `new` 而不是 `std::shared_ptr`？**
- LVGL 的 `user_data` 是 `void*`，只能存裸指针
- `shared_ptr` 会引入引用计数的运行时开销
- lambda 的生命周期绑定到 LVGL 对象——对象销毁时，LVGL 会调用事件清理，此时释放 lambda

**`std::decay<F>::type` 的作用：**
- 去除引用：`F&&` → `F`
- 去除 const：`const F` → `F`
- 去除 volatile
- 确保我们 `new` 的是 lambda 的实际类型，不是引用类型

### 2.5 样式系统的封装原理

LVGL 的样式系统是**值语义**的——`lv_style_t` 是独立于对象存在的样式描述。LVGL++ 的 `LStyle` 保持了这种值语义，同时添加了 Builder 模式：

```cpp
// LVGL C 样式 — 命令式，必须配对 init/reset
lv_style_t style;
lv_style_init(&style);                                    // 1. 初始化
lv_style_set_bg_color(&style, lv_color_hex(0x2196F3));   // 2. 设置属性
lv_style_set_radius(&style, 8);
lv_style_set_pad_all(&style, 10);
lv_obj_add_style(btn, &style, 0);                         // 3. 应用
lv_style_reset(&style);                                   // 4. 清理（经常忘记！）

// LVGL++ — RAII + Builder，自动管理
LStyle style;
style.set_bg_color(0x2196F3)    // 链式调用
     .set_radius(8)             // 不用每次都传 &style
     .set_pad_all(10)           // 直接用 hex，不用 lv_color_hex()
     .apply_to(btn.native());   // 应用到对象
// 离开作用域，自动 lv_style_reset()
```

**LStyle 的 RAII 实现：**

```cpp
class LStyle {
    lv_style_t m_style;
    bool m_initialized;    // 防御性标志：移动后不再 reset

    LStyle() : m_initialized(true) {
        lv_style_init(&m_style);           // 构造时初始化
    }
    ~LStyle() {
        if (m_initialized) {
            lv_style_reset(&m_style);      // 析构时清理
        }
    }

    // 移动后源对象不再负责清理
    LStyle(LStyle&& other) noexcept
        : m_style(other.m_style), m_initialized(other.m_initialized) {
        other.m_initialized = false;       // 标记已转移
    }
};
```

### 2.6 组件封装的统一模式

所有 35 个 Widget 组件遵循统一的封装模式，确保一致性和可预测性：

```cpp
// ── 组件封装模板 ──────────────────────────────────
class L{WidgetName} : public LWidgetCRTP<L{WidgetName}> {
    // [1] 私有子对象（如 LButton 内嵌的 label）
    lv_obj_t* m_label;

public:
    // [2] 构造函数 — 默认父对象为 lv_scr_act()
    explicit L{WidgetName}(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_{widget}_create(parent ? parent : lv_scr_act())) {
        // 初始化子对象
    }

    // [3] 属性设置方法 — 每个都返回 Derived&，支持链式
    L{WidgetName}& set_{prop}({type} val) {
        lv_{widget}_set_{prop}(m_obj, val);
        return *this;
    }

    // [4] 预设主题样式（可选）
    L{WidgetName}& primary() { /* 一键样式 */ return *this; }

    // [5] 事件绑定 — 模板方法，接受 lambda
    template<typename F>
    L{WidgetName}& on_{event}(F&& callback) {
        connect_event(m_obj, LV_EVENT_{EVENT}, std::forward<F>(callback));
        return *this;
    }
};
```

**以 LButton 为例的完整分析：**

```cpp
class LButton : public LWidgetCRTP<LButton> {   // CRTP：链式返回 LButton&
private:
    lv_obj_t* m_label;      // 按钮内嵌一个 label 显示文字
                            // 这是 LVGL 的设计：按钮本身不直接设置文字

public:
    explicit LButton(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_button_create(parent ? parent : lv_scr_act())) {
        m_label = lv_label_create(m_obj);   // 在按钮内部创建 label
        lv_obj_center(m_label);              // 居中显示
    }
    // 析构时 m_label 会随 m_obj 一起被 lv_obj_delete 递归删除
    // 不需要单独管理 m_label 的生命周期
    // 这是 LVGL 父子对象树的特性：删除父对象自动删除所有子对象

    LButton& set_text(const char* txt) {
        lv_label_set_text(m_label, txt);     // 操作的是内嵌 label
        return *this;
    }

    LButton& set_size(int w, int h) {        // 覆盖 CRTP 基类方法
        lv_obj_set_size(m_obj, w, h);        // 操作的是按钮本身
        return *this;
    }

    template<typename F>
    LButton& on_click(F&& callback) {
        connect_event(m_obj, LV_EVENT_CLICKED, std::forward<F>(callback));
        return *this;
    }

    LButton& primary() {
        LStyle style;
        style.set_bg_color(0x2196F3)
             .set_radius(8)
             .set_pad_all(10)
             .apply_to(m_obj);
        return *this;
    }
};
```

**父子对象树的内存模型：**

```
LButton btn;  // 创建了 lv_button_create() + lv_label_create()
              //
              //  LVGL 对象树:
              //    lv_scr_act()
              //      └── lv_button (m_obj)
              //            └── lv_label (m_label)
              //
              //  LButton::~LButton() 调用 lv_obj_delete(m_obj)
              //  → LVGL 递归删除 m_label
              //  → 不需要手动 delete m_label
```

### 2.7 布局容器的封装原理

LVGL 的 Flex 布局通过设置 `lv_obj_t` 的 flex_flow 属性实现。LVGL++ 将其封装为语义明确的布局组件：

```cpp
// C API — 需要理解 LVGL flex 系统的细节
lv_obj_t* vbox = lv_obj_create(parent);
lv_obj_set_flex_flow(vbox, LV_FLEX_FLOW_COLUMN);
lv_obj_set_flex_align(vbox, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
lv_obj_set_style_pad_row(vbox, 10, 0);    // 间距
lv_obj_set_style_pad_all(vbox, 15, 0);    // 内边距

// LVGL++ — 语义化，一目了然
LVBoxLayout vbox;
vbox.set_spacing(10)
    .set_padding(15)
    .set_size(400, 300);
```

```cpp
// LVBoxLayout — 垂直布局容器 (Qt: QVBoxLayout)
class LVBoxLayout : public LWidgetCRTP<LVBoxLayout> {
public:
    explicit LVBoxLayout(lv_obj_t* parent = nullptr)
        : LWidgetCRTP(lv_obj_create(parent ? parent : lv_scr_act())) {
        lv_obj_set_flex_flow(m_obj, LV_FLEX_FLOW_COLUMN);     // 核心设置
        lv_obj_set_flex_align(m_obj,
            LV_FLEX_ALIGN_START,     // 主轴：从顶部开始
            LV_FLEX_ALIGN_CENTER,    // 交叉轴：居中
            LV_FLEX_ALIGN_START      // 轨道：从顶部开始
        );
    }

    LVBoxLayout& set_spacing(int gap) {
        lv_obj_set_style_pad_row(m_obj, gap, 0);  // 间距 = 行内边距
        return *this;
    }

    LVBoxLayout& set_padding(int pad) {
        lv_obj_set_style_pad_all(m_obj, pad, 0);  // 整体内边距
        return *this;
    }
};

// LHBoxLayout — 水平布局容器（同理，FLEX_FLOW_ROW, Qt: QHBoxLayout）
class LHBoxLayout : public LWidgetCRTP<LHBoxLayout> { ... };

// LGridLayout — 网格布局 (Qt: QGridLayout)
class LGridLayout : public LWidgetCRTP<LGridLayout> { ... };

// LStackedLayout — 堆叠布局：同一时间只显示一个子组件 (Qt: QStackedLayout)
class LStackedLayout : public LWidgetCRTP<LStackedLayout> { ... };
```

**布局总览：**

| 布局类 | Qt 对应 | LVGL 实现 | 说明 |
|--------|---------|-----------|------|
| `LVBoxLayout` | QVBoxLayout | Flex COLUMN | 垂直排列子组件 |
| `LHBoxLayout` | QHBoxLayout | Flex ROW | 水平排列子组件 |
| `LGridLayout` | QGridLayout | Grid | 网格行列布局 |
| `LStackedLayout` | QStackedLayout | Flex + hidden | 堆叠切换显示 |

---

## 3. 系统架构

### 3.1 分层架构总览

```
┌─────────────────────────────────────────────────────────┐
│                     应用层 (Application)                  │
│  用户代码继承生成的 _UI 类，覆写事件处理，实现业务逻辑       │
├─────────────────────────────────────────────────────────┤
│                   工具链层 (Toolchain)                     │
│  ┌──────────────┐  ┌──────────────┐  ┌────────────────┐ │
│  │ Qt Designer  │  │ VSCode Ext   │  │  luic Compiler │ │
│  │ (桌面设计器)  │  │ (轻量编辑器)  │  │  JSON → C++   │ │
│  └──────┬───────┘  └──────┬───────┘  └───────┬────────┘ │
│         │                 │                   │          │
│         ▼                 ▼                   ▼          │
│              ┌─────────────────────┐                      │
│              │   .lui (JSON 格式)   │  ← 中间表示          │
│              └─────────────────────┘                      │
├─────────────────────────────────────────────────────────┤
│                  运行时层 (Runtime)                        │
│  ┌──────────────────────────────────────────────────────┐ │
│  │               LVGL++ C++ Wrapper                      │ │
│  │  ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐ ┌───────────┐ │ │
│  │  │Widget│ │Style │ │Event │ │Signal│ │ Object    │ │ │
│  │  │基类   │ │样式   │ │事件   │ │处理器│ │ Config    │ │ │
│  │  └──────┘ └──────┘ └──────┘ └──────┘ └───────────┘ │ │
│  │  ┌──────────────────────────────────────────────────┐ │ │
│  │  │  39 组件 (35 Widgets + 4 Layouts)                │ │ │
│  │  └──────────────────────────────────────────────────┘ │ │
│  └──────────────────────────────────────────────────────┘ │
├─────────────────────────────────────────────────────────┤
│                  平台层 (Platform)                        │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐                  │
│  │  SDL2   │  │  DRM    │  │  FBDEV  │                  │
│  └─────────┘  └─────────┘  └─────────┘                  │
├─────────────────────────────────────────────────────────┤
│              LVGL 9.2 C Library                         │
└─────────────────────────────────────────────────────────┘
```

### 3.2 数据流

```
1. 设计阶段
   Qt Designer / VSCode Extension
       │
       ▼ 拖拽组件 → 设置属性 → 绑定事件
   .lui 文件 (JSON)
       {
         "version": "1.0", "name": "MainScreen", "size": [800, 480],
         "children": [
           { "type": "LButton", "id": "btn_login",
             "props": { "text": "Login", "size": [120, 50] },
             "events": ["clicked"] }
         ]
       }

2. 编译阶段 (CMake 自动触发)
   .lui → Parser → IR → Validator → CodeGenerator → .hpp

3. 运行阶段
   用户继承 _UI 类 → setup() → bind_events() → 覆写 virtual 事件槽
```

---

## 4. luic 编译器设计

### 4.1 编译流水线

```
.lui 文件
    │
    ▼
┌──────────┐     ┌───────────┐     ┌──────────────┐     ┌──────────┐
│  Parser   │────▶│    IR     │────▶│  Validator   │────▶│ CodeGen  │────▶ .hpp
│ (JSON解析)│     │(中间表示) │     │  (校验)      │     │(代码生成)│
└──────────┘     └───────────┘     └──────────────┘     └──────────┘
```

### 4.2 IR — 中间表示

```cpp
struct property_t {
    std::string name;          // "text", "size", "range"
    std::string type;          // "string", "int", "hex", "int_pair", "int_triple", "bool"
    std::string raw;           // 原始 JSON
    int int_val = 0;           // int 值
    int int_val2 = 0;          // int_pair 的第二个值
    int int_val3 = 0;          // int_triple 的第三个值
    std::string str_val;       // string / hex 值
    bool bool_val = false;     // bool 值
};

struct widget_node_t {
    std::string type;                    // "LButton", "LLabel"
    std::string id;                      // "btn_login"
    std::vector<property_t> props;       // 属性列表
    std::vector<std::string> events;     // 事件列表
    std::vector<widget_node_t> children; // 子组件 (递归树)
};

struct lui_document_t {
    std::string version, name;
    int width = 800, height = 480;
    std::vector<widget_node_t> children;
    std::vector<const widget_node_t*> all_nodes() const;  // 扁平化遍历
};
```

### 4.3 ComponentRegistry — 组件元数据

编译器需要知道每个组件有哪些属性、类型、对应什么 C++ 方法。这就是 ComponentRegistry：

```cpp
struct prop_def_t {
    std::string name;        // JSON 中的属性名: "text"
    std::string type;        // 属性类型: "string"
    std::string method;      // C++ 方法名: "set_text"
    bool is_theme = false;   // 是否为主题快捷方式
};

struct event_def_t {
    std::string name;        // 事件名: "clicked"
    std::string method;      // C++ 方法名: "on_click"
};

// 注册示例
reg("LButton", {
    .props = {
        {"text",     "string",   "set_text"},
        {"size",     "int_pair", "set_size"},
        {"theme",    "string",   "", true /* is_theme */},
    },
    .events = {
        {"clicked",  "on_click"},
    }
});
```

**为什么需要 ComponentRegistry？**
- Parser 只知道 JSON 的语法，不知道 "text" 对应 "set_text"
- Validator 需要知道 "size" 应该是 int_pair 类型
- CodeGen 需要知道生成 `set_text("Login")` 而不是 `text("Login")`
- **集中的映射关系**避免了在三个地方重复维护

**当前注册的组件数量：** ComponentRegistry 中注册了 43 种组件（35 Widget + 4 Layout + 菜单/瓦片等 4 个子组件），覆盖了 LVGL 全部 32 个原生组件。

### 4.4 Validator — 校验规则

| 规则 | 说明 |
|------|------|
| name 必填 | `doc.name` 不能为空 |
| children 非空 | 至少一个组件 |
| 组件类型注册 | `type` 必须在 ComponentRegistry 中 |
| id 必填 | 每个节点的 `id` 不能为空 |
| 属性类型匹配 | 属性值类型必须与注册的类型一致 |
| theme 值域 | 只允许 "primary", "danger", "success", "warning" |
| 事件存在性 | events 中的事件名必须在组件定义中 |
| 容器约束 | 非容器组件不能有 children |
| base_props 通用样式 | 37 个 LVGL 通用样式属性跳过组件特定校验 |

### 4.5 CodeGenerator — 代码生成策略

**生成的 C++ 类结构：**

```cpp
class {Name}_UI {
protected:
    {Type}* m_{id} = nullptr;       // 成员指针

public:
    void setup(lv_obj_t* parent = nullptr);   // 前序遍历构建 UI 树
    void bind_events();                        // 绑定事件到虚函数槽

    virtual void on_{id}_{event}(lv_event_t* e) {}  // Template Method 模式

    virtual ~{Name}_UI();                      // 逆序析构
};
```

**为什么用 `new/delete` 而不是智能指针？**
- 生成的代码需要简单直接，便于理解
- `unique_ptr` 的析构顺序不如手动控制灵活
- 逆序析构确保子组件先于父组件删除

**为什么用 `virtual` 事件槽？**
- **Template Method 模式**: 基类定义框架（`bind_events`），子类覆写行为
- 符合 Qt 用户的习惯（`on_btn_clicked` 风格）
- 不需要理解 lambda 或函数对象

---

## 5. Qt 可视化设计器 ✅ IMPLEMENTED

### 5.1 设计理念

**核心原则：所见即所得，LVGL 原生渲染。**

设计器不使用 QPainter 模拟组件外观，而是嵌入帧缓冲区渲染，让 LVGL 直接渲染。用户看到的就是最终设备上运行的效果——字体、抗锯齿、动画、颜色完全一致。

> **实现状态：** ✅ 已完成。完整 Qt 实现位于 `tools/lvgl-designer/`。

### 5.2 架构

```
┌───────────────────────────────────────────────────────────────┐
│                        MainWindow (Qt)                         │
│                                                                │
│  ┌─────────────┐  ┌───────────────────────┐  ┌─────────────┐ │
│  │ComponentPanel│  │  SDL2Widget /         │  │PropertyPanel│ │
│  │ (组件面板)    │  │  DesignerView         │  │ (属性编辑)   │ │
│  │              │  │  ┌─────────────────┐ │  │              │ │
│  │ 拖拽 → .lui  │  │  │  LvglRenderer   │ │  │ 属性 → .lui  │ │
│  └─────────────┘  │  │  (帧缓冲区渲染)  │ │  └─────────────┘ │
│                   │  └─────────────────┘ │                   │
│                   │  DesignerScene +      │                   │
│                   │  WidgetItem 渲染      │                   │
│                   └───────────────────────┘                   │
│  ┌─────────────┐  ┌───────────────────────┐                   │
│  │  TreePanel   │  │     QUndoStack        │                   │
│  │ (组件树)     │  │   (撤销/重做)          │                   │
│  └─────────────┘  └───────────────────────┘                   │
└───────────────────────────────────────────────────────────────┘
```

### 5.3 帧缓冲区渲染方案

> **注意：** 设计器使用 LVGL 帧缓冲区渲染（framebuffer），而非独立的 SDL2 窗口嵌入。

```
Qt 主窗口
  └── DesignerView
        └── DesignerScene
              └── WidgetItem (QGraphicsItem)
                    └── LvglRenderer (LVGL 帧缓冲区渲染)
                          └── 真实 LVGL 组件树渲染
```

**事件桥接：**
- Qt 鼠标事件 → 转换 → LVGL 输入设备
- Qt 键盘事件 → 转换 → LVGL 键盘输入

**跨平台支持：**

| 平台 | 渲染后端 | 状态 |
|------|-----------|------|
| Linux | X11 / Wayland | ✅ 支持 |
| Windows | Win32 / D3D | ✅ 支持 |
| macOS | Cocoa / Metal | ✅ 支持 |

### 5.4 Model-Controller 分离

- **Model**: `LuiDocument` + `ComponentSchema`
- **View**: LVGL 原生渲染（帧缓冲区，非 Qt 绘制）
- **Controller**: `MainWindow` + `UndoCommands` + 事件桥接

### 5.5 撤销/重做

基于 `QUndoStack` 的 Command 模式：

| 命令 | 操作 |
|------|------|
| `AddWidgetCommand` | 添加组件 |
| `DeleteWidgetCommand` | 删除组件 (备份用于恢复) |
| `MoveResizeCommand` | 移动/调整大小 |
| `PropertyChangeCommand` | 属性变更 |

注意：每次 Command 执行后，需要重建 LVGL 组件树以反映变更。

---

## 6. VSCode 扩展 ⚠️ 部分实现

### 6.1 定位

VSCode 扩展是**轻量启动器 + 自动编译**，不做任何 UI 渲染。所有可视化设计工作交给 Qt 设计器完成。

**为什么不做 Webview 渲染？**
- VSCode Webview 本质是 Chromium，无法运行 LVGL
- HTML/CSS 模拟的组件外观与真实渲染必然存在偏差（字体、抗锯齿、动画）
- 维护两套渲染逻辑是不必要的负担

### 6.2 功能清单

| 功能 | 实现方式 | 状态 |
|------|----------|------|
| **打开设计器** | 双击 `.lui` 文件 → 调起 `bin/lvgl-designer` 进程 | ✅ 已实现 |
| **自动编译** | `fs.watch` 监听 `.lui` 变更 → 自动执行 `luic` | ⚠️ 缺少 fs.watch |
| **右键菜单** | `New .lui` / `Open in Designer` / `Compile` | ✅ 已实现 |
| **状态栏** | 显示 luic 编译结果（成功/失败） | ⚠️ 缺少状态栏 |

### 6.3 工作流

```
1. 用户在 VSCode 中双击 main.lui
   → 插件调起 bin/lvgl-designer main.lui

2. Qt 设计器打开，LVGL 原生渲染，用户可视化编辑
   → 保存 → main.lui 更新

3. VSCode 检测到文件变更
   → 自动执行 luic main.lui -o main_ui.hpp

4. 编译成功 → 状态栏显示 ✅
   编译失败 → 输出面板显示错误信息
```

### 6.4 代码量估算

```typescript
// 核心逻辑 ~50 行
vscode.commands.registerCommand('lvgl.openDesigner', (uri) => {
    cp.spawn('bin/lvgl-designer', [uri.fsPath]);
});

vscode.workspace.onDidChangeTextDocument((e) => {
    if (e.document.fileName.endsWith('.lui')) {
        cp.spawn('luic', [e.document.fileName]);
    }
});
```

---

## 7. 设计模式总结

| 模式 | 应用位置 | 解决的问题 |
|------|----------|-----------|
| **RAII** | LWidget, LStyle | 内存泄漏 |
| **CRTP** | LWidgetCRTP | 链式 API 类型截断 |
| **Builder** | LStyle, ObjectConfig | 复杂对象构建 |
| **Command** | UndoCommands | 撤销/重做 |
| **Template Method** | CodeGen 生成事件槽 | 用户覆写事件处理 |
| **Composite** | widget_node_t 树 | 组件树递归结构 |
| **Bridge** | native(), 帧缓冲区渲染 | C++ ↔ C 互操作, Qt ↔ LVGL 桥接 |

---

## 8. 端到端工作流

### 8.1 代码量对比

```cpp
// ═══ 原生 LVGL C (约 30 行) ═══
lv_obj_t* btn = lv_button_create(lv_screen_active());
lv_obj_set_size(btn, 120, 50);
lv_obj_set_pos(btn, 100, 200);
lv_obj_t* label = lv_label_create(btn);
lv_label_set_text(label, "Login");
lv_obj_center(label);
lv_style_t style;
lv_style_init(&style);
lv_style_set_bg_color(&style, lv_color_hex(0x2196F3));
lv_style_set_radius(&style, 8);
lv_style_set_pad_all(&style, 10);
lv_obj_add_style(btn, &style, 0);
lv_obj_add_event_cb(btn, on_login_click, LV_EVENT_CLICKED, NULL);
lv_style_reset(&style);
// 还需要定义: static void on_login_click(lv_event_t* e) { ... }

// ═══ LVGL++ 手写 (约 4 行) ═══
LButton btn;
btn.set_text("Login")
   .set_size(120, 50)
   .set_pos(100, 200)
   .primary()
   .on_click([](auto*) { printf("Login!\n"); });

// ═══ LVGL++ + luic (0 行 UI 代码) ═══
// JSON: { "type": "LButton", "id": "btn_login", "props": { "text": "Login", "size": [120, 50] } }
// 自动生成: class MainScreen_UI { ... };
// 用户只需: class MainScreen : public MainScreen_UI { void on_btn_login_clicked(...) override { ... } };
```

---

## 9. 项目目录结构

```
lv_port_linux/
├── easylvgl/                          # C++ 封装层 (Header-Only, 零构建成本)
│   ├── core/                        # Layer 1 核心基础设施
│   │   ├── widget.hpp               #   LWidget (RAII) + LWidgetCRTP (链式)
│   │   ├── style.hpp                #   LStyle (Builder + RAII)
│   │   ├── event.hpp                #   事件连接 (Lambda → C 回调桥接)
│   │   ├── config.hpp               #   ObjectConfig (通用配置器)
│   │   └── signal_handler.hpp       #   SignalHandler (优雅退出, 独立使用)
│   ├── widgets/                     # Layer 2: 35 个 Widget 组件
│   │   ├── button.hpp               #   LButton
│   │   ├── label.hpp                #   LLabel
│   │   ├── slider.hpp               #   LSlider
│   │   ├── checkbox.hpp             #   LCheckbox
│   │   ├── switch.hpp               #   LSwitch
│   │   ├── combobox.hpp             #   LComboBox (下拉选择框)
│   │   ├── lineedit.hpp             #   LLineEdit (单行文本输入)
│   │   ├── textedit.hpp             #   LTextEdit (多行文本编辑)
│   │   ├── groupbox.hpp             #   LGroupBox (分组框容器)
│   │   ├── radiobutton.hpp          #   LRadioButton (单选按钮)
│   │   ├── image.hpp, arc.hpp, bar.hpp, spinner.hpp, led.hpp, line.hpp
│   │   ├── roller.hpp, spinbox.hpp, buttonmatrix.hpp, keyboard.hpp
│   │   ├── imagebutton.hpp, table.hpp, chart.hpp, scale.hpp
│   │   ├── calendar.hpp, canvas.hpp, span.hpp, animimage.hpp, lottie.hpp
│   │   ├── list.hpp, tabview.hpp, menu.hpp, tileview.hpp, win.hpp
│   │   └── msgbox.hpp
│   ├── layouts/                     # Layer 2: 4 个布局容器
│   │   ├── layout.hpp               #   布局统一入口
│   │   ├── vbox_layout.hpp          #   LVBoxLayout (垂直布局)
│   │   ├── hbox_layout.hpp          #   LHBoxLayout (水平布局)
│   │   ├── grid_layout.hpp          #   LGridLayout (网格布局)
│   │   └── stacked_layout.hpp       #   LStackedLayout (堆叠布局)
│   ├── containers/                  # (空目录，布局已迁移至 layouts/)
│   ├── dialogs/                     # 对话框组件
│   │   └── dialog.hpp
│   ├── lvgl_core.hpp                # Layer 1 入口 (不含 signal_handler)
│   └── lvgl_widgets.hpp             # Layer 2 入口 (全组件 + 布局)
│
├── tools/
│   ├── luic/                        # Layer 3: JSON → C++ 编译器
│   │   ├── main.cpp                 #   CLI: luic input.lui -o output.hpp
│   │   ├── parser.hpp               #   JSON → IR 解析 (nlohmann/json)
│   │   ├── ir.hpp                   #   中间表示 (widget_node_t 树)
│   │   ├── validator.hpp            #   校验 (类型检查 + 约束检查)
│   │   ├── codegen.hpp              #   IR → C++ 代码生成
│   │   ├── component_registry.hpp   #   组件元数据 (43 种组件)
│   │   ├── CMakeLists.txt           #   构建配置
│   │   └── third_party/             #   第三方依赖 (nlohmann/json)
│   └── lvgl-designer/               # ✅ Qt 可视化设计器 (已实现)
│       ├── CMakeLists.txt           #   构建配置
│       └── src/
│           ├── main.cpp             #   入口
│           ├── app/
│           │   ├── MainWindow.hpp   #   主窗口 + 菜单 + 工具栏
│           │   └── MainWindow.cpp
│           ├── model/
│           │   ├── LuiDocument.hpp  #   文档模型
│           │   ├── LuiDocument.cpp
│           │   └── ComponentSchema.hpp  # 组件元数据模式
│           ├── canvas/
│           │   ├── DesignerScene.hpp/cpp    #   设计场景
│           │   ├── DesignerView.hpp/cpp     #   设计视图
│           │   ├── SDL2Widget.hpp/cpp       #   SDL2 嵌入组件
│           │   ├── LvglRenderer.hpp/cpp     #   LVGL 渲染器
│           │   ├── WidgetItem.hpp/cpp       #   组件图形项
│           │   └── items/                   #   各类组件项
│           ├── panels/
│           │   ├── ComponentPanel.hpp/cpp   #   组件面板
│           │   ├── PropertyPanel.hpp/cpp    #   属性面板
│           │   └── TreePanel.hpp/cpp        #   组件树面板
│           └── commands/
│               ├── UndoCommands.hpp         #   撤销/重做命令
│               └── UndoCommands.cpp
│
├── vscode-lvgl-designer/            # VSCode 扩展 (⚠️ 部分实现)
│   └── src/
│       ├── extension.ts             #   入口 + Commands
│       └── LuiEditorProvider.ts     #   Webview 编辑器
│
├── ui/                              # .lui 源文件 (JSON)
├── ui_generated/                    # luic 生成的 C++ 头文件
├── examples/                        # 8 个示例程序
├── lvgl/                            # LVGL 9.2 源码 (submodule)
└── CMakeLists.txt                   # 主构建文件 (含 luic 自动编译)
```

---

*文档结束*
