# LVGL++ - LVGL 9.2 的现代 C++ 封装

**版本：** 2.0.0  
**状态：** 已完成  
**日期：** 2026-04-06

---

## 项目简介

LVGL++ 是对 LVGL 9.2 的 Header-Only C++ 封装库，提供 RAII 内存管理、链式 API、类型安全的事件系统。同时包含完整的工具链：luic 编译器、Qt 可视化设计器、VSCode 插件。

- **39 个封装组件**：35 个 Widget + 4 个布局器（VBox/HBox/Grid/Stacked），覆盖 LVGL 全部 32 个原生组件
- **零成本抽象**：编译后与手写 C 代码性能一致
- **完整工具链**：luic 编译器、Qt 可视化设计器、VSCode 插件

---

## 快速开始

```cpp
#include "lvgl++/lvgl_widgets.hpp"
using namespace lvgl;

// 按钮与链式 API
LButton btn;
btn.set_text("Click Me")
   .set_size(120, 50)
   .primary()
   .on_click([](auto*) { printf("Clicked!\n"); });

// 进度条
LBar bar;
bar.set_range(0, 100)
   .set_value(75, LV_ANIM_ON)
   .set_colors(0xEEEEEE, 0x4CAF50);

// 垂直布局
LVBoxLayout vbox;
vbox.set_spacing(10)
    .set_padding(15);

// 样式构建器
LStyle btn_style;
btn_style.set_bg_color(0x2196F3)
         .set_radius(8)
         .set_pad_all(10)
         .apply_to(btn.native());
```

---

## 组件总览

### 核心组件（9 个）

| 组件 | 文件 | 说明 |
|------|------|------|
| LButton | `widgets/button.hpp` | 按钮，支持 primary/danger/success/warning 主题 |
| LLabel | `widgets/label.hpp` | 文本标签 |
| LSlider | `widgets/slider.hpp` | 滑块 |
| LCheckbox | `widgets/checkbox.hpp` | 复选框 |
| LSwitch | `widgets/switch.hpp` | 开关 |
| LComboBox | `widgets/combobox.hpp` | 下拉列表 |
| LLineEdit | `widgets/lineedit.hpp` | 单行文本输入 |
| LTextEdit | `widgets/textedit.hpp` | 多行文本输入 |
| LImage | `widgets/image.hpp` | 图片 |

### 高频组件（6 个）

| 组件 | 文件 | 说明 |
|------|------|------|
| LArc | `widgets/arc.hpp` | 圆弧进度 |
| LBar | `widgets/bar.hpp` | 进度条 |
| LSpinner | `widgets/spinner.hpp` | 加载动画 |
| LLed | `widgets/led.hpp` | LED 指示灯 |
| LLine | `widgets/line.hpp` | 线条 |
| LList | `widgets/list.hpp` | 列表 |

### 中频组件（12 个）

| 组件 | 文件 | 说明 |
|------|------|------|
| LTable | `widgets/table.hpp` | 表格 |
| LTabView | `widgets/tabview.hpp` | 标签页 |
| LMenu | `widgets/menu.hpp` | 菜单 |
| LRoller | `widgets/roller.hpp` | 滚轮选择器 |
| LScale | `widgets/scale.hpp` | 刻度尺 |
| LSpan | `widgets/span.hpp` | 富文本片段 |
| LSpinbox | `widgets/spinbox.hpp` | 数字输入框 |
| LButtonMatrix | `widgets/buttonmatrix.hpp` | 按钮矩阵 |
| LImageButton | `widgets/imagebutton.hpp` | 图片按钮 |
| LMsgBox | `widgets/msgbox.hpp` | 消息对话框 |
| LGroupBox | `widgets/groupbox.hpp` | 分组框 |
| LRadioButton | `widgets/radiobutton.hpp` | 单选按钮 |

### 低频组件（8 个）

| 组件 | 文件 | 说明 |
|------|------|------|
| LCalendar | `widgets/calendar.hpp` | 日历 |
| LChart | `widgets/chart.hpp` | 图表 |
| LCanvas | `widgets/canvas.hpp` | 画布 |
| LKeyboard | `widgets/keyboard.hpp` | 虚拟键盘 |
| LAnimImage | `widgets/animimage.hpp` | 动画图片 |
| LTileView | `widgets/tileview.hpp` | 平铺视图 |
| LWin | `widgets/win.hpp` | 窗口 |
| LLottie | `widgets/lottie.hpp` | Lottie 动画 |

### 布局组件（5 个）

| 组件 | 文件 | 说明 |
|------|------|------|
| LVBoxLayout | `layouts/vbox_layout.hpp` | 垂直 Flex 布局 |
| LHBoxLayout | `layouts/hbox_layout.hpp` | 水平 Flex 布局 |
| LGridLayout | `layouts/grid_layout.hpp` | 网格布局 |
| LStackedLayout | `layouts/stacked_layout.hpp` | 堆叠布局 |

---

## 核心特性

### 1. RAII 内存管理

```cpp
{
    LButton btn;  // 自动创建 lv_button_create()
    // ...
}   // 离开作用域自动 lv_obj_delete()
```

### 2. 事件处理

```cpp
btn.on_click([](auto*) {
    printf("Clicked!\n");
});

// 通用事件连接
connect_event(obj, LV_EVENT_VALUE_CHANGED, [](lv_event_t* e) {
    // 处理值变化
});
```

### 3. 布局

```cpp
LVBoxLayout vbox;
vbox.set_spacing(10)
    .set_padding(15)
    .set_size(300, 400);

LHBoxLayout hbox;
hbox.set_spacing(5)
    .set_align(LV_ALIGN_CENTER);
```

---

## 工具链

### luic 编译器

将 `.lui`（JSON）文件编译为 C++ 代码，零运行时开销。详见 [luic 指南](docs/guides/luic_guild.md)。

### Qt 可视化设计器

桌面应用，拖拽设计 UI，自动保存为 `.lui` 文件。位于 `tools/lvgl-designer/`。

详见 [DEVELOPMENT_GUIDELINES.md](DEVELOPMENT_GUIDELINES.md) 了解设计原则。

### VSCode 插件

在 VSCode 中打开 `.lui` 文件时自动启动可视化编辑器。位于 `vscode-lvgl-designer/`。

详见 [VSCode 插件指南](docs/guides/vscode_plugin_guide.md)。

---

## 项目结构

```
lv_port_linux/
├── lvgl++/                     # C++ 封装库（Header-Only）
│   ├── core/                   # Layer 1：核心基础设施
│   ├── widgets/                # Layer 2：35 个 Widget
│   ├── layouts/                # Layer 2：5 个 Layout
│   ├── lvgl_core.hpp           # Layer 1 入口
│   └── lvgl_widgets.hpp        # Layer 2 入口
├── tools/
│   ├── luic/                   # luic 编译器（C++）
│   └── lvgl-designer/          # Qt 可视化设计器
├── vscode-lvgl-designer/       # VSCode 插件
├── ui/                         # .lui 界面描述文件
├── ui_generated/               # luic 自动生成的 C++ 代码
├── examples/                   # 示例程序
├── docs/                       # 文档
└── tests/                      # 测试
```

---

## 文档

### 设计文档

- [project-overview.md](docs/design/project-overview.md) - 项目概述
- [architecture-design.md](docs/design/architecture-design.md) - 架构设计
- [layer1-design.md](docs/design/layer1-design.md) - Layer 1 核心设计
- [layer2-design.md](docs/design/layer2-design.md) - Layer 2 组件设计
- [json-ui-format.md](docs/design/json-ui-format.md) - JSON UI 格式规范

### 使用指南

- [signal_handling.md](docs/guides/signal_handling.md) - 信号处理与优雅退出
- [coding_style.md](docs/guides/coding_style.md) - 编码规范
- [luic_guild.md](docs/guides/luic_guild.md) - luic 编译器指南
- [vscode_plugin_guide.md](docs/guides/vscode_plugin_guide.md) - VSCode 插件指南

---

## 许可证

MIT License
