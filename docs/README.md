# EasyLVGL 文档中心

**版本：** 1.0.0　**日期：** 2026-10-18

EasyLVGL = **C++ 封装库**（`easylvgl/`）+ **编译器**（`luic`）+ **可视化设计器**（`tools/lvgl-designer`）。

---

## 从这里开始

- [英文主页](../README_EN.md) / [中文主页](../README.md) —— 项目定位、快速开始、特性
- [开发规范](../DEVELOPMENT_GUIDELINES.md) —— 封装原则（只封装 LVGL 原生能力）

## 设计文档（design/）

| 文档 | 内容 |
|---|---|
| [project-overview.md](design/project-overview.md) | 项目目标、三层架构、设计原则 |
| [architecture-design.md](design/architecture-design.md) | 整体架构、分层职责、数据流 |
| [layer1-design.md](design/layer1-design.md) | 核心层：LWidget / LStyle / 事件 / SignalHandler |
| [layer2-design.md](design/layer2-design.md) | 组件层：35 个 Widget + 4 个布局器 |
| [json-ui-format.md](design/json-ui-format.md) | `.lui` 格式规范（含 resources） |
| [project-level-design.md](design/project-level-design.md) | 工程级设计：组件化、导出参数、工程面板、实施状态 |

## 使用指南（guides/）

| 文档 | 内容 |
|---|---|
| [luic_guild.md](guides/luic_guild.md) | luic 用法、`.lui` 字段、工程模式、图片资源嵌入 |
| [coding_style.md](guides/coding_style.md) | 编码风格 |
| [signal_handling.md](guides/signal_handling.md) | 信号处理与优雅退出 |
| [windows_build.md](guides/windows_build.md) | Windows（MinGW-w64）构建与运行 |

## 工具链

| 工具 | 路径 | 说明 |
|---|---|---|
| `luic` | `tools/luic/` | `.lui` → C++ 编译器（支持 `--project`） |
| EasyLVGL Designer | `tools/lvgl-designer/` | Qt 设计器（工程/组件/属性面板、构建运行） |
| 资源转换 | `scripts/assets/convert_assets.py` | 图片 → `lv_image_dsc_t`（qrc 式） |
| 验证脚本 | `scripts/designer/`、`scripts/headless/` | 设计器自动化、无头交互测试 |

---

> EasyLVGL 与 LVGL Kft. 无隶属关系，LVGL 是 LVGL Kft. 的商标。
