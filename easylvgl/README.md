# easylvgl —— EasyLVGL 的 C++ 封装库

Header-only C++17 封装，直接调用 LVGL 9.2 原生 API：RAII 管理对象生命周期、链式 API、
类型安全事件。编译产物为 **`libeasylvgl.a`**（由 `cmake/merge_libs.cmake` 合并）。

## 目录

```
easylvgl/
├── lvgl_core.hpp        # 核心层入口（LWidget / LStyle / 事件 / SignalHandler）
├── lvgl_widgets.hpp     # 组件层入口（35 Widget + 4 布局）
├── core/                # LWidget / LWidgetCRTP / LStyle / Event / SignalHandler / Platform
├── widgets/             # 35 个组件封装（button / label / arc / chart / ...）
├── layouts/             # LVBoxLayout / LHBoxLayout / LGridLayout / LStackedLayout
└── dialogs/             # 对话框辅助
```

## 用法

```cpp
#include "easylvgl/lvgl_widgets.hpp"
using namespace easylvgl;

LButton btn;
btn.set_text("OK").set_size(120, 50).primary().on_click([](auto*) { /* ... */ });
```

```cpp
LStyle style;
style.set_bg_color(0x2196F3).set_radius(8).apply_to(btn.native());
```

## 约束

- 只封装 LVGL 原生能力，不引入 LVGL 之外的功能（见 `DEVELOPMENT_GUIDELINES.md`）
- 不复制 LVGL 的 API：所有 setter 直接调用对应的 `lv_*` 函数
- 组件属性与设计器面板一一对应；不生效的属性不暴露

## 相关文档

- [核心层设计](../docs/design/layer1-design.md)
- [组件层设计](../docs/design/layer2-design.md)
- [luic 指南](../docs/guides/luic_guild.md)
