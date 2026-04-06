# LVGL++ 文档中心

**版本：** 2.0.0  
**日期：** 2026-04-06

---

## 文档导航

### 核心设计文档

- [lvglcpp-arch-desiner.md](lvglcpp-arch-desiner.md) - 架构设计总纲

### 设计文档（design/）

- [project-overview.md](design/project-overview.md) - 项目概述
- [architecture-design.md](design/architecture-design.md) - 架构设计
- [layer1-design.md](design/layer1-design.md) - Layer 1 核心设计
- [layer2-design.md](design/layer2-design.md) - Layer 2 组件设计
- [json-ui-format.md](design/json-ui-format.md) - JSON UI 格式规范

### 使用指南（guides/）
- [coding_style.md](guides/coding_style.md) - 编码风格指南
- [signal_handling.md](guides/signal_handling.md) - 信号处理与优雅退出
- [luic_guild.md](guides/luic_guild.md) - luic 编译器使用指南
- [vscode_plugin_guide.md](guides/vscode_plugin_guide.md) - VSCode 插件使用指南

---

## 工具链

| 工具 | 路径 | 说明 |
|------|------|------|
| luic | `tools/luic/` | C++ UI 编译器，. `.lui` → C++ 代码 |
| LVGL Designer | `tools/lvgl-designer/` | Qt 桌面可视化设计器 |
| VSCode Extension | `vscode-lvgl-designer/` | VSCode `.lui` 编辑器扩展 |
