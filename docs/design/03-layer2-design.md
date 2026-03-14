# Layer 2: 组件库详细设计

## 概述

Layer 2在Layer 1的基础上，提供高层组件库，封装常用UI模式，大幅减少重复代码。

**设计目标：**
- 提供开箱即用的常用组件
- 预设主题样式
- 简化复杂布局
- 保持API一致性

## 文件结构

```
lvgl++/
├── widgets/
│   ├── button.hpp          # 按钮组件
│   ├── label.hpp           # 标签组件
│   ├── slider.hpp          # 滑块组件
│   ├── textarea.hpp        # 文本输入组件
│   ├── checkbox.hpp        # 复选框组件
│   ├── dropdown.hpp        # 下拉框组件
│   └── image.hpp           # 图片组件
├── containers/
│   ├── vstack.hpp          # 垂直布局
│   ├── hstack.hpp          # 水平布局
│   ├── grid.hpp            # 网格布局
│   └── scroll.hpp          # 滚动容器
├── dialogs/
│   ├── dialog.hpp          # 对话框
│   └── msgbox.hpp          # 消息框
└── lvgl_widgets.hpp        # 统一入口
```

## 核心组件设计

### 1. Button组件

**文件：** `lvgl++/widgets/button.hpp`

```cpp
#pragma once
#include "../core/widget.hpp"
#include "../core/style.hpp"
#include "../core/event.hpp"

namespace lvgl {

/**
 * @brief 按钮组件
 * 
 * 封装lv_button，提供链式API和预设主题。
 */
class Button : public Widget {
private:
    lv_obj_t* label_;  ///< 按钮文本标签
    
public:
    /**
     * @brief 构造函数
     * @param parent 父对象（nullptr表示当前屏幕）
     */
    explicit Button(lv_obj_t* parent = nullptr) 
        : Widget(lv_button_create(parent ? parent : lv_scr_act())) {
        // 创建居中的标签
        label_ = lv_label_create(obj_);
        lv_obj_center(label_);
    }
    
    /**
     * @brief 设置按钮文本
     * @param txt 文本内容
     * @return Button& 当前对象引用
     */
    Button& text(const char* txt) {
        lv_label_set_text(label_, txt);
        return *this;
    }
    
    /**
     * @brief 设置按钮大小
     * @param w 宽度
     * @param h 高度
     * @return Button& 当前对象引用
     */
    Button& size(int w, int h) {
        lv_obj_set_size(obj_, w, h);
        return *this;
    }
    
    /**
     * @brief 设置按钮位置
     * @param x X坐标
     * @param y Y坐标
     * @return Button& 当前对象引用
     */
    Button& pos(int x, int y) {
        lv_obj_set_pos(obj_, x, y);
        return *this;
    }
    
    /**
     * @brief 设置对齐方式
     * @param a 对齐方式
     * @return Button& 当前对象引用
     */
    Button& align(lv_align_t a) {
        lv_obj_set_align(obj_, a);
        return *this;
    }
    
    /**
     * @brief 添加点击事件
     * @tparam F 回调函数类型
     * @param callback 回调函数
     * @return Button& 当前对象引用
     */
    template<typename F>
    Button& on_click(F&& callback) {
        connect_event(obj_, LV_EVENT_CLICKED, std::forward<F>(callback));
        return *this;
    }
    
    /**
     * @brief 应用主要按钮样式（蓝色）
     * @return Button& 当前对象引用
     */
    Button& primary() {
        Style style;
        style.bg_color(0x2196F3)
             .radius(8)
             .pad_all(10)
             .apply_to(obj_);
        return *this;
    }
    
    /**
     * @brief 应用危险按钮样式（红色）
     * @return Button& 当前对象引用
     */
    Button& danger() {
        Style style;
        style.bg_color(0xF44336)
             .radius(8)
             .pad_all(10)
             .apply_to(obj_);
        return *this;
    }
    
    /**
     * @brief 应用成功按钮样式（绿色）
     * @return Button& 当前对象引用
     */
    Button& success() {
        Style style;
        style.bg_color(0x4CAF50)
             .radius(8)
             .pad_all(10)
             .apply_to(obj_);
        return *this;
    }
    
    /**
     * @brief 应用警告按钮样式（橙色）
     * @return Button& 当前对象引用
     */
    Button& warning() {
        Style style;
        style.bg_color(0xFF9800)
             .radius(8)
             .pad_all(10)
             .apply_to(obj_);
        return *this;
    }
    
    /**
     * @brief 设置按钮为禁用状态
     * @param disabled true表示禁用
     * @return Button& 当前对象引用
     */
    Button& set_disabled(bool disabled = true) {
        if (disabled) {
            lv_obj_add_state(obj_, LV_STATE_DISABLED);
        } else {
            lv_obj_remove_state(obj_, LV_STATE_DISABLED);
        }
        return *this;
    }
};

} // namespace lvgl
```

### 2. Label组件

**文件：** `lvgl++/widgets/label.hpp`

```cpp
#pragma once
#include "../core/widget.hpp"
#include "../core/style.hpp"

namespace lvgl {

/**
 * @brief 标签组件
 */
class Label : public Widget {
public:
    explicit Label(lv_obj_t* parent = nullptr)
        : Widget(lv_label_create(parent ? parent : lv_scr_act())) {}
    
    /**
     * @brief 设置文本
     * @param txt 文本内容
     * @return Label& 当前对象引用
     */
    Label& text(const char* txt) {
        lv_label_set_text(obj_, txt);
        return *this;
    }
    
    /**
     * @brief 设置格式化文本
     * @param fmt 格式字符串
     * @param ... 可变参数
     * @return Label& 当前对象引用
     */
    Label& text_fmt(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        lv_label_set_text_fmt(obj_, fmt, args);
        va_end(args);
        return *this;
    }
    
    /**
     * @brief 设置对齐方式
     * @param a 对齐方式
     * @return Label& 当前对象引用
     */
    Label& align(lv_align_t a) {
        lv_obj_set_align(obj_, a);
        return *this;
    }
    
    /**
     * @brief 设置文本颜色
     * @param hex 十六进制颜色值
     * @return Label& 当前对象引用
     */
    Label& color(uint32_t hex) {
        Style style;
        style.text_color(hex).apply_to(obj_);
        return *this;
    }
    
    /**
     * @brief 设置字体大小（使用预设字体）
     * @param size 字体大小（12, 14, 16, 18, 20, 24等）
     * @return Label& 当前对象引用
     */
    Label& font_size(int size) {
        const lv_font_t* font = nullptr;
        switch(size) {
            case 12: font = &lv_font_montserrat_12; break;
            case 14: font = &lv_font_montserrat_14; break;
            case 16: font = &lv_font_montserrat_16; break;
            case 18: font = &lv_font_montserrat_18; break;
            case 20: font = &lv_font_montserrat_20; break;
            case 24: font = &lv_font_montserrat_24; break;
            default: font = LV_FONT_DEFAULT; break;
        }
        Style style;
        style.text_font(font).apply_to(obj_);
        return *this;
    }
};

} // namespace lvgl
```

### 3. VStack/HStack布局组件

**文件：** `lvgl++/containers/vstack.hpp`

```cpp
#pragma once
#include "../core/widget.hpp"

namespace lvgl {

/**
 * @brief 垂直堆叠布局容器
 * 
 * 使用Flex布局，子元素垂直排列。
 */
class VStack : public Widget {
public:
    explicit VStack(lv_obj_t* parent = nullptr)
        : Widget(lv_obj_create(parent ? parent : lv_scr_act())) {
        // 设置为垂直Flex布局
        lv_obj_set_flex_flow(obj_, LV_FLEX_FLOW_COLUMN);
        // 默认对齐：顶部对齐，水平居中
        lv_obj_set_flex_align(obj_, 
            LV_FLEX_ALIGN_START,      // 主轴（垂直）
            LV_FLEX_ALIGN_CENTER,     // 交叉轴（水平）
            LV_FLEX_ALIGN_START       // 轨道
        );
    }
    
    /**
     * @brief 设置子元素间距
     * @param gap 间距值
     * @return VStack& 当前对象引用
     */
    VStack& spacing(int gap) {
        lv_obj_set_style_pad_row(obj_, gap, 0);
        return *this;
    }
    
    /**
     * @brief 设置容器内边距
     * @param pad 内边距值
     * @return VStack& 当前对象引用
     */
    VStack& padding(int pad) {
        lv_obj_set_style_pad_all(obj_, pad, 0);
        return *this;
    }
    
    /**
     * @brief 设置子元素水平对齐方式
     * @param align 对齐方式
     * @return VStack& 当前对象引用
     */
    VStack& align_items(lv_flex_align_t align) {
        lv_obj_set_flex_align(obj_, 
            LV_FLEX_ALIGN_START,  // 保持主轴对齐
            align,                // 修改交叉轴对齐
            LV_FLEX_ALIGN_START
        );
        return *this;
    }
    
    /**
     * @brief 设置大小
     * @param w 宽度
     * @param h 高度
     * @return VStack& 当前对象引用
     */
    VStack& size(int w, int h) {
        lv_obj_set_size(obj_, w, h);
        return *this;
    }
};

/**
 * @brief 水平堆叠布局容器
 */
class HStack : public Widget {
public:
    explicit HStack(lv_obj_t* parent = nullptr)
        : Widget(lv_obj_create(parent ? parent : lv_scr_act())) {
        // 设置为水平Flex布局
        lv_obj_set_flex_flow(obj_, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(obj_, 
            LV_FLEX_ALIGN_START,      // 主轴（水平）
            LV_FLEX_ALIGN_CENTER,     // 交叉轴（垂直）
            LV_FLEX_ALIGN_START
        );
    }
    
    /**
     * @brief 设置子元素间距
     * @param gap 间距值
     * @return HStack& 当前对象引用
     */
    HStack& spacing(int gap) {
        lv_obj_set_style_pad_column(obj_, gap, 0);
        return *this;
    }
    
    /**
     * @brief 设置容器内边距
     * @param pad 内边距值
     * @return HStack& 当前对象引用
     */
    HStack& padding(int pad) {
        lv_obj_set_style_pad_all(obj_, pad, 0);
        return *this;
    }
    
    /**
     * @brief 设置大小
     * @param w 宽度
     * @param h 高度
     * @return HStack& 当前对象引用
     */
    HStack& size(int w, int h) {
        lv_obj_set_size(obj_, w, h);
        return *this;
    }
};

} // namespace lvgl
```

### 4. Dialog组件

**文件：** `lvgl++/dialogs/dialog.hpp`

```cpp
#pragma once
#include "lvgl/lvgl.h"
#include <functional>

namespace lvgl {

/**
 * @brief 对话框组件
 * 
 * 提供便捷的对话框创建方法。
 */
class Dialog {
private:
    lv_obj_t* msgbox_;
    
    Dialog() : msgbox_(nullptr) {}
    
public:
    /**
     * @brief 创建信息对话框
     * @param title 标题
     * @param msg 消息内容
     * @return Dialog 对话框对象
     */
    static Dialog info(const char* title, const char* msg) {
        Dialog d;
        d.msgbox_ = lv_msgbox_create(nullptr);
        lv_msgbox_set_title(d.msgbox_, title);
        lv_msgbox_set_text(d.msgbox_, msg);
        return d;
    }
    
    /**
     * @brief 创建确认对话框
     * @param title 标题
     * @param msg 消息内容
     * @return Dialog 对话框对象
     */
    static Dialog confirm(const char* title, const char* msg) {
        return info(title, msg);
    }
    
    /**
     * @brief 添加按钮
     * @param text 按钮文本
     * @param callback 点击回调
     * @return Dialog& 当前对象引用
     */
    Dialog& add_button(const char* text, std::function<void()> callback) {
        lv_msgbox_add_close_button(msgbox_);
        // TODO: 实现自定义按钮添加
        return *this;
    }
    
    /**
     * @brief 显示对话框
     */
    void show() {
        if (msgbox_) {
            lv_obj_move_foreground(msgbox_);
        }
    }
    
    /**
     * @brief 关闭对话框
     */
    void close() {
        if (msgbox_) {
            lv_msgbox_close(msgbox_);
            msgbox_ = nullptr;
        }
    }
};

} // namespace lvgl
```

## 统一入口头文件

**文件：** `lvgl++/lvgl_widgets.hpp`

```cpp
#pragma once

/**
 * @file lvgl_widgets.hpp
 * @brief LVGL++ Layer 2统一入口
 * 
 * 包含所有Layer 2组件
 */

// Layer 1基础
#include "lvgl_core.hpp"

// 基础控件
#include "widgets/button.hpp"
#include "widgets/label.hpp"
#include "widgets/slider.hpp"
#include "widgets/textarea.hpp"
#include "widgets/checkbox.hpp"
#include "widgets/dropdown.hpp"
#include "widgets/image.hpp"

// 布局容器
#include "containers/vstack.hpp"
#include "containers/hstack.hpp"
#include "containers/grid.hpp"
#include "containers/scroll.hpp"

// 对话框
#include "dialogs/dialog.hpp"
#include "dialogs/msgbox.hpp"

namespace lvgl {

// Layer 2版本信息
constexpr int WIDGETS_VERSION_MAJOR = 1;
constexpr int WIDGETS_VERSION_MINOR = 0;
constexpr int WIDGETS_VERSION_PATCH = 0;

} // namespace lvgl
```

## 使用示例

### 示例1：简单按钮

```cpp
#include "lvgl++/lvgl_widgets.hpp"
using namespace lvgl;

void create_button() {
    Button btn;
    btn.text("Click Me")
       .size(120, 50)
       .align(LV_ALIGN_CENTER)
       .primary()
       .on_click([](auto*) {
           printf("Button clicked!\n");
       });
}
```

### 示例2：表单布局

```cpp
void create_form() {
    VStack form;
    form.spacing(20)
        .padding(30)
        .size(300, 400)
        .align(LV_ALIGN_CENTER);
    
    // 标题
    Label title(form.native());
    title.text("User Login")
         .color(0x2196F3)
         .font_size(24);
    
    // 用户名输入
    // TextArea username(form.native());
    // username.placeholder("Username");
    
    // 按钮组
    HStack buttons(form.native());
    buttons.spacing(10);
    
    Button cancel(buttons.native());
    cancel.text("Cancel")
          .on_click([](auto*) { /* ... */ });
    
    Button submit(buttons.native());
    submit.text("Submit")
          .primary()
          .on_click([](auto*) {
              Dialog::info("Success", "Login successful!")
                    .show();
          });
}
```

### 示例3：对话框

```cpp
void show_dialog() {
    Dialog::confirm("Confirm", "Are you sure?")
          .add_button("Yes", []() {
              printf("Confirmed\n");
          })
          .add_button("No", []() {
              printf("Cancelled\n");
          })
          .show();
}
```

## 待实现组件列表

### 基础控件
- [x] Button - 按钮
- [x] Label - 标签
- [ ] Slider - 滑块
- [ ] TextArea - 文本输入
- [ ] Checkbox - 复选框
- [ ] Switch - 开关
- [ ] Dropdown - 下拉框
- [ ] Image - 图片
- [ ] Spinner - 加载动画

### 布局容器
- [x] VStack - 垂直布局
- [x] HStack - 水平布局
- [ ] Grid - 网格布局
- [ ] ScrollView - 滚动视图
- [ ] TabView - 标签页

### 复杂组件
- [x] Dialog - 对话框
- [ ] List - 列表
- [ ] Table - 表格
- [ ] Chart - 图表
- [ ] Calendar - 日历
- [ ] Keyboard - 键盘

## 性能考虑

### 内存开销

每个组件额外开销：
- Button: 16字节（基类8字节 + label_指针8字节）
- Label: 8字节（仅基类）
- VStack/HStack: 8字节（仅基类）
- Dialog: 8字节（msgbox_指针）

### 优化策略

1. **延迟创建**：仅在需要时创建子对象
2. **内联小函数**：编译器优化为零开销
3. **避免虚函数**：除非必要，减少虚函数表开销
4. **移动语义**：避免不必要的拷贝

## 下一步

- [04-implementation-plan.md](./04-implementation-plan.md) - 实施计划
- [05-checklist.md](./05-checklist.md) - 自检清单
