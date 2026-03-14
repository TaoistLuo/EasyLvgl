# LVGL++ 架构设计

## 整体架构

LVGL++采用分层架构，每层提供不同级别的抽象，用户可根据需求选择合适的层次。

## Layer 0: LVGL C API（原生层）

这是LVGL原生C API，保持不变，作为所有上层的基础。

**特点：**
- 完整的LVGL功能
- 最大灵活性
- 最低学习成本（对C开发者）
- 需要手动管理内存
- 类型不安全

## Layer 1: 智能封装层（基础层）

**目标：** 解决内存管理和类型安全问题，不改变API风格。

### 核心组件

#### 1. Widget基类（RAII）
```cpp
class Widget {
protected:
    lv_obj_t* obj_;
public:
    explicit Widget(lv_obj_t* obj);
    ~Widget();  // 自动调用lv_obj_delete_async
    
    // 移动语义
    Widget(Widget&& other) noexcept;
    Widget& operator=(Widget&& other) noexcept;
    
    // 禁止拷贝
    Widget(const Widget&) = delete;
    Widget& operator=(const Widget&) = delete;
    
    lv_obj_t* native();
};
```

**设计要点：**
- RAII自动管理对象生命周期
- 移动语义支持高效传递
- 禁止拷贝避免双重释放
- `native()`方法获取原生指针，保持与C API兼容

#### 2. Style构建器（Builder模式）
```cpp
class Style {
    lv_style_t style_;
public:
    Style();
    ~Style();
    
    Style& bg_color(uint32_t hex);
    Style& border(int width, uint32_t color);
    Style& radius(int r);
    Style& pad_all(int pad);
    
    void apply_to(lv_obj_t* obj, lv_style_selector_t selector = 0);
};
```

**设计要点：**
- 链式调用提高可读性
- 自动管理lv_style_t生命周期
- 支持所有LVGL样式属性

#### 3. 事件系统（类型安全）
```cpp
template<typename F>
void connect_event(lv_obj_t* obj, lv_event_code_t code, F&& callback);
```

**设计要点：**
- 使用模板自动推导回调类型
- 支持lambda表达式和函数对象
- 自动管理回调生命周期
- 类型安全，无需void*转换

#### 4. ObjectConfig（Fluent API）
```cpp
class ObjectConfig {
    lv_obj_t* obj_;
public:
    ObjectConfig& size(int w, int h);
    ObjectConfig& pos(int x, int y);
    ObjectConfig& align(lv_align_t a);
    ObjectConfig& flex_flow(lv_flex_flow_t flow);
    
    template<typename F>
    ObjectConfig& on_clicked(F&& callback);
    
    lv_obj_t* get();
};
```

**设计要点：**
- 链式配置对象属性
- 返回引用支持连续调用
- 最后调用get()获取原生指针

### Layer 1 效果对比

**原生C代码（20行）：**
```c
lv_obj_t* btn = lv_button_create(parent);
lv_obj_set_size(btn, 120, 50);
lv_obj_set_align(btn, LV_ALIGN_CENTER);

lv_style_t style;
lv_style_init(&style);
lv_style_set_bg_color(&style, lv_color_hex(0x2196F3));
lv_style_set_radius(&style, 8);
lv_style_set_pad_all(&style, 10);
lv_obj_add_style(btn, &style, LV_STATE_DEFAULT);

void callback(lv_event_t* e) {
    printf("Clicked\n");
}
lv_obj_add_event_cb(btn, callback, LV_EVENT_CLICKED, NULL);
```

**Layer 1代码（5行）：**
```cpp
auto btn = configure(lv_button_create(parent))
    .size(120, 50)
    .align(LV_ALIGN_CENTER)
    .on_clicked([](auto*) { printf("Clicked\n"); })
    .get();

Style().bg_color(0x2196F3).radius(8).pad_all(10).apply_to(btn);
```

## Layer 2: 组件库层（推荐层）

**目标：** 提供高层组件，消除重复代码模式。

### 核心组件

#### 1. Button组件
```cpp
class Button : public Widget {
    lv_obj_t* label_;
public:
    explicit Button(lv_obj_t* parent = nullptr);
    
    Button& text(const char* txt);
    Button& size(int w, int h);
    Button& align(lv_align_t a);
    
    template<typename F>
    Button& on_click(F&& callback);
    
    // 预设主题
    Button& primary();
    Button& danger();
    Button& success();
};
```

#### 2. 布局组件
```cpp
class VStack : public Widget {
public:
    explicit VStack(lv_obj_t* parent = nullptr);
    
    VStack& spacing(int gap);
    VStack& padding(int pad);
    VStack& align_items(lv_flex_align_t align);
};

class HStack : public Widget {
public:
    explicit HStack(lv_obj_t* parent = nullptr);
    
    HStack& spacing(int gap);
    HStack& padding(int pad);
};
```

#### 3. 对话框组件
```cpp
class Dialog {
public:
    static Dialog info(const char* title, const char* msg);
    static Dialog confirm(const char* title, const char* msg);
    
    Dialog& add_button(const char* text, std::function<void()> callback);
    void show();
};
```

### Layer 2 效果对比

**原生C代码（50+行）：**
```c
lv_obj_t* container = lv_obj_create(parent);
lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, 
                      LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
lv_obj_set_style_pad_row(container, 20, 0);
lv_obj_set_style_pad_all(container, 30, 0);

lv_obj_t* title = lv_label_create(container);
lv_label_set_text(title, "Welcome");
lv_style_t title_style;
lv_style_init(&title_style);
lv_style_set_text_color(&title_style, lv_color_hex(0x2196F3));
lv_obj_add_style(title, &title_style, 0);

lv_obj_t* btn_container = lv_obj_create(container);
lv_obj_set_flex_flow(btn_container, LV_FLEX_FLOW_ROW);
lv_obj_set_style_pad_column(btn_container, 10, 0);

lv_obj_t* btn1 = lv_button_create(btn_container);
lv_obj_t* btn1_label = lv_label_create(btn1);
lv_label_set_text(btn1_label, "Cancel");
// ... 更多代码
```

**Layer 2代码（15行）：**
```cpp
VStack stack;
stack.spacing(20).padding(30);

Label title(stack.native());
title.text("Welcome").color(0x2196F3);

HStack buttons(stack.native());
buttons.spacing(10);

Button btn1(buttons.native());
btn1.text("Cancel").on_click([](auto*) { /* ... */ });

Button btn2(buttons.native());
btn2.text("OK").primary().on_click([](auto*) { /* ... */ });
```

## Layer 3: 响应式属性层（可选层）

**目标：** 提供数据绑定，实现自动UI更新。

### 核心组件

#### 1. Property类
```cpp
template<typename T>
class Property {
    T value_;
    std::vector<std::function<void(const T&)>> observers_;
public:
    explicit Property(T initial = T{});
    
    const T& get() const;
    void set(T new_value);
    
    Property& operator=(T new_value);
    operator const T&() const;
    
    void on_change(std::function<void(const T&)> observer);
};
```

#### 2. 绑定辅助函数
```cpp
template<typename T>
void bind_to_label(Property<T>& prop, lv_obj_t* label);

template<typename T>
void bind_to_slider(Property<T>& prop, lv_obj_t* slider);
```

### Layer 3 效果对比

**原生C代码（需要手动同步）：**
```c
int counter = 0;
lv_obj_t* label = lv_label_create(parent);

void increment_callback(lv_event_t* e) {
    counter++;
    // 手动更新UI
    lv_label_set_text_fmt(label, "Count: %d", counter);
}
```

**Layer 3代码（自动同步）：**
```cpp
Property<int> counter{0};
auto* label = lv_label_create(parent);

bind_to_label(counter, label);

Button btn;
btn.on_click([&](auto*) {
    counter = counter.get() + 1;  // 自动更新UI
});
```

## 性能考虑

### 内存开销

| 层次 | 额外内存 | 说明 |
|------|---------|------|
| Layer 1 | 0字节 | 仅在栈上，编译后优化掉 |
| Layer 2 | 8-16字节/对象 | 虚函数表指针 + 成员变量 |
| Layer 3 | 24-32字节/属性 | std::vector存储观察者 |

### 运行时开销

- **Layer 1**: 零开销，内联后与C代码相同
- **Layer 2**: 一次虚函数调用（~5ns）
- **Layer 3**: 遍历观察者列表（O(n)，n为观察者数量）

### 编译时优化

- 使用`inline`和`constexpr`
- 模板在编译期展开
- 移动语义避免拷贝
- RVO/NRVO优化返回值

## 兼容性策略

### 与原生C API混用

所有层次都提供`native()`方法获取原生`lv_obj_t*`指针：

```cpp
Button btn;
lv_obj_t* native_ptr = btn.native();
lv_obj_set_user_data(native_ptr, &my_data);  // 使用原生API
```

### 渐进式迁移

可以逐步迁移现有代码：

```cpp
// 旧代码
lv_obj_t* old_btn = lv_button_create(parent);

// 新代码与旧代码混用
Button new_btn(parent);
lv_obj_align_to(new_btn.native(), old_btn, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
```

## 错误处理

### Layer 1-2: 返回值检查

```cpp
// 创建失败返回nullptr
lv_obj_t* obj = lv_button_create(parent);
if (!obj) {
    // 处理错误
}
```

### Layer 3: 可选的异常支持

```cpp
#ifdef LVGL_USE_EXCEPTIONS
    throw std::runtime_error("Widget creation failed");
#else
    return nullptr;  // 嵌入式环境
#endif
```

## 线程安全

LVGL本身不是线程安全的，LVGL++遵循相同原则：

- 所有UI操作必须在LVGL线程执行
- 使用`lv_async_call()`从其他线程调用
- Property的观察者在设置线程执行

## 下一步

- [02-layer1-design.md](./02-layer1-design.md) - Layer 1详细设计
- [03-layer2-design.md](./03-layer2-design.md) - Layer 2详细设计
- [04-implementation-plan.md](./04-implementation-plan.md) - 实施计划
