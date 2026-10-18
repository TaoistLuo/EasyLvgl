# Layer 2: 组件库详细设计

> **适用版本：** EasyLVGL 1.0.0（LVGL 9.2）　**最后更新：** 2026-10-18

**版本：** 2.0.0  
**状态：** ✅ 已完成（35 个 Widget + 4 个布局器，共 39 个组件）

## 概述

Layer 2在Layer 1的基础上，提供高层组件库，封装常用UI模式，大幅减少重复代码。

**设计目标：**
- 提供开箱即用的常用组件
- 预设主题样式
- 简化复杂布局
- 保持API一致性

## 文件结构

```
easylvgl/
├── widgets/                    # 35个组件
│   ├── button.hpp              # LButton
│   ├── imagebutton.hpp         # LImageButton
│   ├── checkbox.hpp            # LCheckbox
│   ├── radiobutton.hpp         # LRadioButton
│   ├── switch.hpp              # LSwitch
│   ├── buttonmatrix.hpp        # LButtonMatrix
│   ├── lineedit.hpp            # LLineEdit（单行输入）
│   ├── textedit.hpp            # LTextEdit（多行输入）
│   ├── combobox.hpp            # LComboBox（基于lv_dropdown）
│   ├── spinbox.hpp             # LSpinbox
│   ├── slider.hpp              # LSlider
│   ├── roller.hpp              # LRoller
│   ├── label.hpp               # LLabel
│   ├── image.hpp               # LImage
│   ├── bar.hpp                 # LBar
│   ├── arc.hpp                 # LArc
│   ├── spinner.hpp             # LSpinner
│   ├── led.hpp                 # LLed
│   ├── line.hpp                # LLine
│   ├── scale.hpp               # LScale
│   ├── calendar.hpp            # LCalendar
│   ├── chart.hpp               # LChart
│   ├── table.hpp               # LTable
│   ├── canvas.hpp              # LCanvas
│   ├── span.hpp                # LSpan
│   ├── keyboard.hpp            # LKeyboard
│   ├── animimage.hpp           # LAnimImage
│   ├── groupbox.hpp            # LGroupBox
│   ├── list.hpp                # LList
│   ├── tabview.hpp             # LTabView
│   ├── menu.hpp                # LMenu
│   ├── tileview.hpp            # LTileView
│   ├── win.hpp                 # LWin
│   ├── msgbox.hpp              # LMsgBox
│   └── lottie.hpp              # LLottie（可选，需要LV_USE_LOTTIE）
├── layouts/                    # 5个布局组件
│   ├── layout.hpp              # 统一入口
│   ├── vbox_layout.hpp         # LVBoxLayout
│   ├── hbox_layout.hpp         # LHBoxLayout
│   ├── grid_layout.hpp         # LGridLayout
│   └── stacked_layout.hpp      # LStackedLayout
├── dialogs/
│   └── dialog.hpp              # LDialog辅助类
├── containers/                 # （空，布局在layouts/中）
└── lvgl_widgets.hpp            # Layer 2统一入口
```

**注意：** 不存在`containers/`内容，所有布局组件在`layouts/`目录下。不存在`dialogs/`组件目录（Dialog在`dialogs/dialog.hpp`，MsgBox在`widgets/msgbox.hpp`）。

## 核心组件设计

### 1. LButton组件

**文件：** `easylvgl/widgets/button.hpp`

```cpp
#pragma once
#include "easylvgl/core/widget.hpp"
#include "easylvgl/core/event.hpp"
#include "lvgl/lvgl.h"

namespace lvgl {

class LButton : public LWidgetCRTP<LButton> {
private:
    lv_obj_t* m_label;  ///< 按钮文本标签（m_前缀）
    
public:
    explicit LButton(lv_obj_t* parent = nullptr) 
        : LWidgetCRTP(lv_button_create(parent ? parent : lv_scr_act())) {
        m_label = lv_label_create(m_obj);
        lv_obj_center(m_label);
    }
    
    LButton& set_text(const char* txt);      // 注意：set_前缀
    LButton& set_size(int w, int h);
    LButton& set_pos(int x, int y);
    LButton& set_align(lv_align_t a);
    
    template<typename F>
    LButton& on_click(F&& callback);         // 事件用on_前缀
    
    // 预设主题
    LButton& primary();                      // 蓝色 #2196F3
    LButton& danger();                       // 红色 #F44336
    LButton& success();                      // 绿色 #4CAF50
    LButton& warning();                      // 橙色 #FF9800
    
    LButton& set_disabled(bool dis = true);
};

} // namespace lvgl
```

### 2. LLabel组件

**文件：** `easylvgl/widgets/label.hpp`

```cpp
class LLabel : public LWidgetCRTP<LLabel> {
public:
    explicit LLabel(lv_obj_t* parent = nullptr);

    LLabel& set_text(const char* txt);
    
    template<typename... Args>
    LLabel& set_text_fmt(const char* fmt, Args&&... args);
    
    LLabel& set_color(uint32_t hex);
    LLabel& set_font_size(int size);
    LLabel& set_long_mode(lv_label_long_mode_t mode);
    LLabel& set_size(int w, int h);
    LLabel& set_pos(int x, int y);
};
```

### 3. LSlider组件

**文件：** `easylvgl/widgets/slider.hpp`

```cpp
class LSlider : public LWidgetCRTP<LSlider> {
public:
    explicit LSlider(lv_obj_t* parent = nullptr);

    LSlider& set_range(int min, int max);    // 注意：set_range，不是range
    LSlider& set_value(int val, lv_anim_enable_t anim = LV_ANIM_OFF);
    LSlider& set_left_value(int val, lv_anim_enable_t anim = LV_ANIM_OFF);
    LSlider& set_mode(lv_slider_mode_t mode);
    
    int value() const;                       // getter不用set_前缀
    
    LSlider& set_size(int w, int h);
    LSlider& set_pos(int x, int y);
    
    template<typename F>
    LSlider& on_change(F&& callback);
};
```

### 4. 布局组件（layouts/目录）

#### LVBoxLayout - 垂直布局

**文件：** `easylvgl/layouts/vbox_layout.hpp`（注意：不是`containers/vstack.hpp`）

```cpp
class LVBoxLayout : public LWidgetCRTP<LVBoxLayout> {
public:
    explicit LVBoxLayout(lv_obj_t* parent = nullptr);
    
    LVBoxLayout& set_spacing(int gap);
    LVBoxLayout& set_padding(int pad);
    LVBoxLayout& set_padding(int top, int bottom, int left, int right);
    LVBoxLayout& set_align_main(lv_flex_align_t align);
    LVBoxLayout& set_align_cross(lv_flex_align_t align);
    LVBoxLayout& set_align_track(lv_flex_align_t align);
    LVBoxLayout& set_align(lv_flex_align_t main, lv_flex_align_t cross, 
                           lv_flex_align_t track = LV_FLEX_ALIGN_START);
    LVBoxLayout& set_growth();
    LVBoxLayout& set_size(int w, int h);
    LVBoxLayout& set_pos(int x, int y);
    LVBoxLayout& set_align(lv_align_t align_value);
};
```

#### LHBoxLayout - 水平布局

**文件：** `easylvgl/layouts/hbox_layout.hpp`

```cpp
class LHBoxLayout : public LWidgetCRTP<LHBoxLayout> {
public:
    explicit LHBoxLayout(lv_obj_t* parent = nullptr);
    
    LHBoxLayout& set_spacing(int gap);
    LHBoxLayout& set_padding(int pad);
    LHBoxLayout& set_padding(int top, int bottom, int left, int right);
    LHBoxLayout& set_align_main(lv_flex_align_t align);
    LHBoxLayout& set_align_cross(lv_flex_align_t align);
    LHBoxLayout& set_growth();
    // ... 与LVBoxLayout对称的API
};
```

#### LGridLayout - 网格布局

**文件：** `easylvgl/layouts/grid_layout.hpp`

```cpp
class LGridLayout : public LWidgetCRTP<LGridLayout> {
public:
    explicit LGridLayout(lv_obj_t* parent = nullptr);
    
    LGridLayout& set_columns(const std::vector<int32_t>& cols);
    LGridLayout& set_rows(const std::vector<int32_t>& rows);
    LGridLayout& set_grid(const std::vector<int32_t>& cols, 
                          const std::vector<int32_t>& rows);
    LGridLayout& set_column_align(lv_grid_align_t align);
    LGridLayout& set_row_align(lv_grid_align_t align);
    LGridLayout& set_grid_align(lv_grid_align_t col, lv_grid_align_t row);
    
    // 静态方法：放置子元素到网格
    static void place(lv_obj_t* child, int32_t col, int32_t row,
                      int32_t col_span = 1, int32_t row_span = 1);
    
    LGridLayout& set_row_spacing(int gap);
    LGridLayout& set_column_spacing(int gap);
    LGridLayout& set_spacing(int row_gap, int col_gap);
    LGridLayout& set_padding(int pad);
};
```

#### LStackedLayout - 堆叠布局

**文件：** `easylvgl/layouts/stacked_layout.hpp`

```cpp
class LStackedLayout : public LWidgetCRTP<LStackedLayout> {
public:
    explicit LStackedLayout(lv_obj_t* parent = nullptr);
    
    int add(lv_obj_t* child);
    LStackedLayout& set_current(int index);
    int current() const;
    int count() const;
};
```

### 5. LMsgBox组件（替代Dialog）

**文件：** `easylvgl/widgets/msgbox.hpp`

LMsgBox直接封装`lv_msgbox`，提供消息框功能：

```cpp
class LMsgBox : public LWidgetCRTP<LMsgBox> {
public:
    explicit LMsgBox(lv_obj_t* parent = nullptr);
    
    LMsgBox& set_title(const char* txt);
    LMsgBox& set_text(const char* txt);
    LMsgBox& add_close_button();
    lv_obj_t* add_footer_button(const char* txt);
    
    template<typename Func>
    LMsgBox& on_value_changed(Func&& callback);
    
    void close();
    
    ~LMsgBox() override;  // 重写析构，使用lv_msgbox_close
};
```

**注意：** 还有独立的`LDialog`辅助类在`dialogs/dialog.hpp`中，提供`create()`、`info()`、`confirm()`等静态工厂方法，但主要推荐使用`LMsgBox`。

### 6. 额外组件

以下组件为额外封装，不在原始计划中：

| 组件 | 文件 | 说明 |
|------|------|------|
| LGroupBox | `widgets/groupbox.hpp` | 分组容器（带标题边框） |
| LRadioButton | `widgets/radiobutton.hpp` | 单选按钮（基于lv_checkbox） |
| LComboBox | `widgets/combobox.hpp` | 下拉选择框（基于lv_dropdown） |
| LLineEdit | `widgets/lineedit.hpp` | 单行文本输入（lv_textarea + one_line） |
| LTextEdit | `widgets/textedit.hpp` | 多行文本输入（lv_textarea） |

## 统一入口头文件

**文件：** `easylvgl/lvgl_widgets.hpp`

```cpp
#pragma once

// Buttons
#include "easylvgl/widgets/button.hpp"
#include "easylvgl/widgets/imagebutton.hpp"
#include "easylvgl/widgets/checkbox.hpp"
#include "easylvgl/widgets/radiobutton.hpp"
#include "easylvgl/widgets/switch.hpp"
#include "easylvgl/widgets/buttonmatrix.hpp"

// Input
#include "easylvgl/widgets/lineedit.hpp"
#include "easylvgl/widgets/textedit.hpp"
#include "easylvgl/widgets/combobox.hpp"
#include "easylvgl/widgets/spinbox.hpp"
#include "easylvgl/widgets/slider.hpp"
#include "easylvgl/widgets/roller.hpp"

// Display
#include "easylvgl/widgets/label.hpp"
#include "easylvgl/widgets/image.hpp"
#include "easylvgl/widgets/bar.hpp"
#include "easylvgl/widgets/arc.hpp"
#include "easylvgl/widgets/spinner.hpp"
#include "easylvgl/widgets/led.hpp"
#include "easylvgl/widgets/line.hpp"
#include "easylvgl/widgets/scale.hpp"
#include "easylvgl/widgets/calendar.hpp"
#include "easylvgl/widgets/chart.hpp"
#include "easylvgl/widgets/table.hpp"
#include "easylvgl/widgets/canvas.hpp"
#include "easylvgl/widgets/span.hpp"
#include "easylvgl/widgets/keyboard.hpp"
#include "easylvgl/widgets/animimage.hpp"

// Containers
#include "easylvgl/widgets/groupbox.hpp"
#include "easylvgl/widgets/list.hpp"
#include "easylvgl/widgets/tabview.hpp"
#include "easylvgl/widgets/menu.hpp"
#include "easylvgl/widgets/tileview.hpp"
#include "easylvgl/widgets/win.hpp"

// Dialogs
#include "easylvgl/widgets/msgbox.hpp"

// Layouts
#include "easylvgl/layouts/layout.hpp"
```

## 使用示例

### 示例1：简单按钮

```cpp
#include "easylvgl/lvgl_widgets.hpp"
using namespace lvgl;

void create_button() {
    LButton btn;
    btn.set_text("Click Me")      // 注意：set_text，不是text
       .set_size(120, 50)         // 注意：set_size，不是size
       .set_align(LV_ALIGN_CENTER)
       .primary()
       .on_click([](auto*) {
           printf("Button clicked!\n");
       });
}
```

### 示例2：表单布局

```cpp
void create_form() {
    LVBoxLayout form;                  // 注意：LVBoxLayout，不是VStack
    form.set_spacing(20)
        .set_padding(30)
        .set_size(300, 400)
        .set_align(LV_ALIGN_CENTER);
    
    // 标题
    LLabel title(form.native());
    title.set_text("User Login")       // 注意：set_text
         .set_color(0x2196F3)          // 注意：set_color
         .set_font_size(24);
    
    // 用户名输入
    LLineEdit username(form.native());  // 注意：LLineEdit，不是TextArea
    username.set_placeholder("Username");
    
    // 密码输入
    LLineEdit password(form.native());
    password.set_placeholder("Password")
            .set_password_mode(true);
    
    // 按钮组
    LHBoxLayout buttons(form.native()); // 注意：LHBoxLayout，不是HStack
    buttons.set_spacing(10);
    
    LButton cancel(buttons.native());
    cancel.set_text("Cancel")
          .on_click([](auto*) { /* ... */ });
    
    LButton submit(buttons.native());
    submit.set_text("Submit")
          .primary()
          .on_click([](auto*) { /* ... */ });
}
```

### 示例3：消息框

```cpp
void show_message() {
    // 使用LMsgBox
    LMsgBox msgbox;
    msgbox.set_title("Info")
          .set_text("Operation completed!")
          .add_close_button();
    
    // 或使用LDialog辅助类
    // LDialog::info("Info", "Operation completed!").show();
}
```

### 示例4：网格布局

```cpp
void create_grid() {
    LGridLayout grid;
    grid.set_grid({100, 100, 100}, {50, 50})
        .set_spacing(10, 10);
    
    LButton btn1(grid.native());
    btn1.set_text("1");
    LGridLayout::place(btn1.native(), 0, 0);
    
    LButton btn2(grid.native());
    btn2.set_text("2");
    LGridLayout::place(btn2.native(), 1, 0);
}
```

## 全部组件列表（39个，全部已实现 ✅）

### 按钮类（6个）
- [x] LButton - 按钮
- [x] LImageButton - 图片按钮
- [x] LCheckbox - 复选框
- [x] LRadioButton - 单选按钮
- [x] LSwitch - 开关
- [x] LButtonMatrix - 按钮矩阵

### 输入类（6个）
- [x] LLineEdit - 单行文本输入
- [x] LTextEdit - 多行文本输入
- [x] LComboBox - 下拉选择框
- [x] LSpinbox - 数值输入
- [x] LSlider - 滑块
- [x] LRoller - 滚轮选择器

### 显示类（13个）
- [x] LLabel - 标签
- [x] LImage - 图片
- [x] LBar - 进度条
- [x] LArc - 圆弧
- [x] LSpinner - 加载动画
- [x] LLed - LED指示灯
- [x] LLine - 线条
- [x] LScale - 刻度尺
- [x] LCalendar - 日历
- [x] LChart - 图表
- [x] LTable - 表格
- [x] LCanvas - 画布
- [x] LSpan - 富文本片段

### 容器类（6个）
- [x] LGroupBox - 分组容器
- [x] LList - 列表
- [x] LTabView - 标签页
- [x] LMenu - 菜单
- [x] LTileView - 平铺视图
- [x] LWin - 窗口

### 特殊类（3个）
- [x] LKeyboard - 虚拟键盘
- [x] LAnimImage - 动画图片
- [x] LMsgBox - 消息框

### 可选类（1个）
- [x] LLottie - Lottie动画（需要LV_USE_LOTTIE）

### 布局类（5个）
- [x] LVBoxLayout - 垂直布局
- [x] LHBoxLayout - 水平布局
- [x] LGridLayout - 网格布局
- [x] LStackedLayout - 堆叠布局

## 性能考虑

### 内存开销

每个组件额外开销：
- LButton: ~24字节（基类16 + m_label指针8）
- LLabel: ~16字节（仅基类）
- LVBoxLayout/LHBoxLayout: ~16字节（仅基类）
- LGridLayout: ~48字节（基类 + vectors + align状态）
- LStackedLayout: ~24字节（基类 + m_current + m_count）

### 优化策略

1. **CRTP消除虚函数**：所有组件使用`LWidgetCRTP<Derived>`，编译时多态
2. **内联小函数**：编译器优化为零开销
3. **移动语义**：避免不必要的拷贝
4. **同步删除**：`lv_obj_delete()`避免异步问题

## 相关文档

- [json-ui-format.md](./json-ui-format.md) - JSON UI 格式规范
