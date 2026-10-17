# LVGL++ 架构设计

**版本：** 2.0.0  
**状态：** ✅ 已完成

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

## Layer 1: 智能封装层（基础层） ✅ 已完成

**目标：** 解决内存管理和类型安全问题，不改变API风格。

### 核心组件

#### 1. LWidget基类（RAII + CRTP）
```cpp
class LWidget {
protected:
    lv_obj_t* m_obj;
public:
    explicit LWidget(lv_obj_t* obj);
    virtual ~LWidget();  // 调用lv_obj_delete()同步删除，Ctrl+C安全
    
    // 移动语义
    LWidget(LWidget&& other) noexcept;
    LWidget& operator=(LWidget&& other) noexcept;
    
    // 禁止拷贝
    LWidget(const LWidget&) = delete;
    LWidget& operator=(const LWidget&) = delete;
    
    lv_obj_t* native();
    const lv_obj_t* native() const;
};

template<typename Derived>
class LWidgetCRTP : public LWidget {
public:
    using LWidget::LWidget;
    using LWidget::operator=;

    Derived& set_size(int w, int h);
    Derived& set_pos(int x, int y);
    Derived& set_align(lv_align_t align_value);
};
```

**设计要点：**
- RAII自动管理对象生命周期
- CRTP模式（`LWidgetCRTP<Derived>`）实现编译时多态，避免虚函数开销
- 移动语义支持高效传递
- 禁止拷贝避免双重释放
- `native()`方法获取原生指针，保持与C API兼容
- 析构函数使用`lv_obj_delete()`同步删除，避免Ctrl+C崩溃
- `sizeof(LWidget) ≈ 16`（虚析构函数vtable指针 + m_obj指针）

#### 2. LStyle构建器（Builder模式）
```cpp
class LStyle {
    lv_style_t m_style;
public:
    LStyle();
    ~LStyle();
    
    LStyle& set_bg_color(uint32_t hex);
    LStyle& set_border(int width, uint32_t color);
    LStyle& set_radius(int r);
    LStyle& set_pad_all(int pad);
    LStyle& set_text_color(uint32_t hex);
    LStyle& set_text_font(const lv_font_t* font);
    LStyle& set_shadow_width(int width);
    LStyle& set_shadow_color(uint32_t hex);
    LStyle& set_pad_top/set_pad_bottom/set_pad_left/set_pad_right(int pad);
    LStyle& set_border_width(int width);
    LStyle& set_border_color(uint32_t hex);
    LStyle& set_bg_opa(lv_opa_t opa);
    LStyle& set_shadow_offset_x/set_shadow_offset_y(int offset);
    
    void apply_to(lv_obj_t* obj, lv_style_selector_t selector = 0);
    lv_style_t* native();
};
```

**设计要点：**
- 链式调用提高可读性
- 所有方法使用`set_`前缀（Qt风格）
- 自动管理lv_style_t生命周期
- 移动语义支持
- 支持所有LVGL样式属性

#### 3. 事件系统（类型安全 + 自动清理）
```cpp
template<typename F>
void connect_event(lv_obj_t* obj, lv_event_code_t code, F&& callback);

template<typename F>
void on_clicked(lv_obj_t* obj, F&& callback);

template<typename F>
void on_value_changed(lv_obj_t* obj, F&& callback);
```

**设计要点：**
- 使用模板自动推导回调类型
- 支持lambda表达式和函数对象
- 注册`LV_EVENT_DELETE`清理handler，自动释放堆上回调
- 类型安全，无需void*转换

#### 4. ObjectConfig（Fluent API）
```cpp
class ObjectConfig {
    lv_obj_t* m_obj;
public:
    explicit ObjectConfig(lv_obj_t* obj);
    
    ObjectConfig& size(int w, int h);
    ObjectConfig& pos(int x, int y);
    ObjectConfig& align(lv_align_t a);
    ObjectConfig& flex_flow(lv_flex_flow_t flow);
    ObjectConfig& flex_align(lv_flex_align_t main, lv_flex_align_t cross, lv_flex_align_t track);
    
    template<typename F>
    ObjectConfig& on_clicked(F&& callback);
    
    ObjectConfig& style(LStyle& style, lv_style_selector_t selector = 0);
    
    lv_obj_t* get();
};

inline ObjectConfig configure(lv_obj_t* obj);
```

**设计要点：**
- 链式配置对象属性
- `style()`方法延迟实现在`lvgl_core.hpp`中（避免循环依赖）
- 返回引用支持连续调用

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

LStyle().set_bg_color(0x2196F3).set_radius(8).set_pad_all(10).apply_to(btn);
```

## Layer 2: 组件库层（推荐层） ✅ 已完成

**目标：** 提供高层组件，消除重复代码模式。

### 核心组件（35个widgets + 4个layouts = 39个组件）

#### 1. LButton组件
```cpp
class LButton : public LWidgetCRTP<LButton> {
    lv_obj_t* m_label;
public:
    explicit LButton(lv_obj_t* parent = nullptr);
    
    LButton& set_text(const char* txt);
    LButton& set_size(int w, int h);
    LButton& set_pos(int x, int y);
    LButton& set_align(lv_align_t a);
    
    template<typename F>
    LButton& on_click(F&& callback);
    
    // 预设主题
    LButton& primary();
    LButton& danger();
    LButton& success();
    LButton& warning();
    LButton& set_disabled(bool dis = true);
};
```

#### 2. 布局组件
```cpp
// 垂直布局（类似QVBoxLayout）
class LVBoxLayout : public LWidgetCRTP<LVBoxLayout> {
public:
    explicit LVBoxLayout(lv_obj_t* parent = nullptr);
    LVBoxLayout& set_spacing(int gap);
    LVBoxLayout& set_padding(int pad);
    LVBoxLayout& set_align_main(lv_flex_align_t align);
    LVBoxLayout& set_align_cross(lv_flex_align_t align);
    LVBoxLayout& set_growth();
};

// 水平布局（类似QHBoxLayout）
class LHBoxLayout : public LWidgetCRTP<LHBoxLayout> {
public:
    explicit LHBoxLayout(lv_obj_t* parent = nullptr);
    LHBoxLayout& set_spacing(int gap);
    LHBoxLayout& set_padding(int pad);
    LHBoxLayout& set_align_main(lv_flex_align_t align);
    LHBoxLayout& set_align_cross(lv_flex_align_t align);
    LHBoxLayout& set_growth();
};

// 网格布局（类似QGridLayout）
class LGridLayout : public LWidgetCRTP<LGridLayout> {
public:
    LGridLayout& set_columns(const std::vector<int32_t>& cols);
    LGridLayout& set_rows(const std::vector<int32_t>& rows);
    LGridLayout& set_grid(const std::vector<int32_t>& cols, const std::vector<int32_t>& rows);
    static void place(lv_obj_t* child, int32_t col, int32_t row, ...);
};

// 堆叠布局（类似QStackedLayout）
class LStackedLayout : public LWidgetCRTP<LStackedLayout> {
public:
    int add(lv_obj_t* child);
    LStackedLayout& set_current(int index);
};
```

#### 3. 消息框组件
```cpp
class LMsgBox : public LWidgetCRTP<LMsgBox> {
public:
    explicit LMsgBox(lv_obj_t* parent = nullptr);
    LMsgBox& set_title(const char* txt);
    LMsgBox& set_text(const char* txt);
    LMsgBox& add_close_button();
    lv_obj_t* add_footer_button(const char* txt);
    void close();
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
LVBoxLayout layout;
layout.set_spacing(20).set_padding(30);

LLabel title(layout.native());
title.set_text("Welcome").set_color(0x2196F3);

LHBoxLayout buttons(layout.native());
buttons.set_spacing(10);

LButton cancel(buttons.native());
cancel.set_text("Cancel").on_click([](auto*) { /* ... */ });

LButton submit(buttons.native());
submit.set_text("OK").primary().on_click([](auto*) { /* ... */ });
```

## Layer 3: 响应式属性层 ⚠️ 未实现

**状态：** 未实现，暂无计划。

~~**目标：** 提供数据绑定，实现自动UI更新。~~

Layer 1和Layer 2已提供足够的功能覆盖，Layer 3的响应式/数据绑定系统暂不实现。

## 性能考虑

### 内存开销

| 层次 | 额外内存 | 说明 |
|------|---------|------|
| Layer 1 | ~16字节 | LWidget: vtable指针 + m_obj指针 |
| Layer 2 | 16-32字节/对象 | 基类 + 成员变量（如m_label） |
| Layer 3 | N/A | 未实现 |

### 运行时开销

- **Layer 1**: 零额外开销，内联后与C代码相同
- **Layer 2**: 无虚函数调用开销（CRTP编译时多态）
- **LWidget sizeof ≈ 16**：虚析构函数vtable指针(8) + m_obj指针(8)

### 编译时优化

- CRTP消除虚函数开销
- 使用`inline`和`constexpr`
- 模板在编译期展开
- 移动语义避免拷贝
- RVO/NRVO优化返回值

## 兼容性策略

### 与原生C API混用

所有层次都提供`native()`方法获取原生`lv_obj_t*`指针：

```cpp
LButton btn;
lv_obj_t* native_ptr = btn.native();
lv_obj_set_user_data(native_ptr, &my_data);  // 使用原生API
```

### 渐进式迁移

可以逐步迁移现有代码：

```cpp
// 旧代码
lv_obj_t* old_btn = lv_button_create(parent);

// 新代码与旧代码混用
LButton new_btn(parent);
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

## 线程安全

LVGL本身不是线程安全的，LVGL++遵循相同原则：

- 所有UI操作必须在LVGL线程执行
- 使用`SignalHandler`配合`lv_timer_handler()`实现优雅退出
- Ctrl+C信号处理确保`lv_obj_delete()`同步安全执行

## 相关文档

- [layer1-design.md](./layer1-design.md) - Layer 1 详细设计
- [layer2-design.md](./layer2-design.md) - Layer 2 详细设计
- [json-ui-format.md](./json-ui-format.md) - JSON UI 格式规范
