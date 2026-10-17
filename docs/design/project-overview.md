# LVGL++ 项目概述

**版本：** 2.0.0  
**状态：** ✅ 已完成

## 项目目标

为LVGL 9.2创建现代C++抽象层，将开发效率提升到接近Qt的水平，同时保持嵌入式系统的性能和资源约束。

## 核心问题

当前LVGL C API存在以下痛点：

### 1. 样式设置冗长
```c
// 需要10+行代码设置一个简单样式
lv_style_t style;
lv_style_init(&style);
lv_style_set_bg_color(&style, lv_color_hex(0xFF0000));
lv_style_set_border_width(&style, 3);
lv_style_set_border_color(&style, lv_color_hex(0xFF0800));
lv_style_set_radius(&style, 10);
lv_style_set_pad_all(&style, 5);
lv_obj_add_style(btn, &style, LV_STATE_DEFAULT);
```

### 2. 事件处理不安全
```c
// 类型不安全，需要手动void*转换
void callback(lv_event_t* e) {
    void* data = lv_event_get_user_data(e);
    int* count = (int*)data;  // 危险
}
lv_obj_add_event_cb(btn, callback, LV_EVENT_CLICKED, &user_data);
```

### 3. 布局代码重复
```c
// Flex布局需要多行配置
lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, 
                      LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
lv_obj_set_style_pad_row(container, 10, 0);
```

### 4. 无自动内存管理
```c
// 容易忘记释放，导致内存泄漏
lv_obj_t* obj = lv_obj_create(parent);
// ... 中间可能return或throw
lv_obj_del(obj);  // 可能执行不到
```

## 解决方案：三层架构

```
┌─────────────────────────────────────────┐
│  Layer 3: 响应式属性 (未实现)            │  ← 数据绑定
│  Reactive Properties & Data Binding     │
├─────────────────────────────────────────┤
│  Layer 2: 组件库 (推荐) ✅              │  ← 高层抽象
│  High-level Components + Builder API    │
├─────────────────────────────────────────┤
│  Layer 1: 智能封装 (基础) ✅            │  ← 核心基础
│  Smart Wrappers - RAII + Type Safety    │
├─────────────────────────────────────────┤
│  Layer 0: LVGL C API                    │  ← 原生API
└─────────────────────────────────────────┘
```

## 设计原则

1. **零成本抽象** - 编译后无额外开销
2. **类型安全** - 编译期类型检查
3. **RAII** - 自动资源管理
4. **渐进式** - 可与原生C API混用
5. **嵌入式友好** - 无异常、最小内存占用
6. **现代C++** - 使用C++14/17特性

## 参考框架

本项目借鉴了以下框架的设计理念：

- **Qt** - 信号槽机制、属性系统、命名规范（L前缀）
- **Slint** - 嵌入式优化、声明式UI
- **Dear ImGui** - 简洁API设计
- **sigslot** - 轻量级信号槽实现

## 预期效果

| 指标 | Layer 1 | Layer 2 | Layer 3 |
|------|---------|---------|---------|
| 代码减少 | 40% | 60% | 70% |
| 学习成本 | 极低 | 低 | 中 |
| 工作量 | 1周 | 1-2月 | 未实现 |
| 维护成本 | 极低 | 低 | 中 |

## 项目结构

```
lvgl++/
├── core/
│   ├── widget.hpp              # LWidget基类 + LWidgetCRTP<Derived>模板
│   ├── style.hpp               # LStyle样式构建器
│   ├── event.hpp               # 事件系统（类型安全）
│   ├── config.hpp              # ObjectConfig（Fluent API）
│   └── signal_handler.hpp      # SignalHandler（Ctrl+C优雅退出）
├── widgets/                    # 35个组件封装
│   ├── button.hpp              # LButton
│   ├── label.hpp               # LLabel
│   ├── slider.hpp              # LSlider
│   ├── checkbox.hpp            # LCheckbox
│   ├── switch.hpp              # LSwitch
│   ├── image.hpp               # LImage
│   ├── bar.hpp                 # LBar
│   ├── arc.hpp                 # LArc
│   ├── spinner.hpp             # LSpinner
│   ├── led.hpp                 # LLed
│   ├── line.hpp                # LLine
│   ├── scale.hpp               # LScale
│   ├── list.hpp                # LList
│   ├── table.hpp               # LTable
│   ├── chart.hpp               # LChart
│   ├── calendar.hpp            # LCalendar
│   ├── canvas.hpp              # LCanvas
│   ├── keyboard.hpp            # LKeyboard
│   ├── roller.hpp              # LRoller
│   ├── spinbox.hpp             # LSpinbox
│   ├── buttonmatrix.hpp        # LButtonMatrix
│   ├── imagebutton.hpp         # LImageButton
│   ├── span.hpp                # LSpan
│   ├── animimage.hpp           # LAnimImage
│   ├── tabview.hpp             # LTabView
│   ├── menu.hpp                # LMenu
│   ├── tileview.hpp            # LTileView
│   ├── win.hpp                 # LWin
│   ├── msgbox.hpp              # LMsgBox
│   ├── groupbox.hpp            # LGroupBox
│   ├── radiobutton.hpp         # LRadioButton
│   ├── combobox.hpp            # LComboBox（基于lv_dropdown）
│   ├── lineedit.hpp            # LLineEdit（单行文本输入）
│   ├── textedit.hpp            # LTextEdit（多行文本输入）
│   └── lottie.hpp              # LLottie（可选）
├── layouts/                    # 5个布局组件
│   ├── layout.hpp              # 布局统一入口
│   ├── vbox_layout.hpp         # LVBoxLayout（垂直布局）
│   ├── hbox_layout.hpp         # LHBoxLayout（水平布局）
│   ├── grid_layout.hpp         # LGridLayout（网格布局）
│   └── stacked_layout.hpp      # LStackedLayout（堆叠布局）
├── dialogs/
│   └── dialog.hpp              # LDialog辅助类
├── containers/                 # （空目录，布局已迁移至layouts/）
├── lvgl_core.hpp               # Layer 1 统一入口
└── lvgl_widgets.hpp            # Layer 2 统一入口（40个组件）
```

## 兼容性

- **LVGL版本**: 9.2+
- **C++标准**: C++14 或更高
- **编译器**: GCC 7+, Clang 6+, MSVC 2017+
- **平台**: Linux, Windows, macOS, 嵌入式RTOS

## 相关文档

- [architecture-design.md](./architecture-design.md) - 架构设计
- [layer1-design.md](./layer1-design.md) - Layer 1 设计
- [layer2-design.md](./layer2-design.md) - Layer 2 设计
- [json-ui-format.md](./json-ui-format.md) - JSON UI 格式规范
