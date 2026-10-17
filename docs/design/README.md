# LVGL++ 设计文档

本目录包含 LVGL++ 项目的架构设计和组件设计文档。

## 📚 文档导航

### 1. [项目概述](./project-overview.md)
- 项目目标和核心问题
- 三层架构设计
- 设计原则和参考框架
- 项目结构

### 2. [架构设计](./architecture-design.md)
- 整体架构说明
- Layer 1/2/3 详细介绍
- 性能考虑和兼容性策略

### 3. [Layer 1 设计](./layer1-design.md)
- LWidget / LWidgetCRTP 基类
- LStyle 构建器
- 事件系统
- ObjectConfig（Fluent API）
- SignalHandler

### 4. [Layer 2 设计](./layer2-design.md)
- 35 个 Widget 组件设计
- 4 个布局器（VBox/HBox/Grid/Stacked）设计
- 统一封装模式
- 组件使用示例

### 5. [JSON UI 格式](./json-ui-format.md)
- .lui 文件格式规范
- 组件类型和属性定义

### 6. [工程级设计](./project-level-design.md) 📝 讨论稿
- 文件 = 部件、屏幕是特例
- 组件化：`kind` / `exports` / `@Name` 实例
- 工程清单与 luic 工程模式
- 设计器工程化（新建组件、递归预览、参数编辑）

---

**最后更新：** 2026-10-18
