# EasyLVGL Designer 开发准则

## 核心原则

> **只封装 LVGL 原生能力，提升开发效率。不做 LVGL 没有的功能。**

设计器职责：
- 降低 LVGL 使用门槛
- 提供 Qt Creator 风格的组织和操作体验
- 自动生成合法的 LVGL C++ 代码

设计器不该做的事：
- 发明 LVGL 没有的布局语义
- 用设计器行为覆盖 LVGL 的原生控制逻辑
- 模拟 Qt 独有但 LVGL 无稳定对应的功能

---

## 一、组件面板（ComponentPanel）

### 保留的布局器
| 布局器 | LVGL 原生映射 | 状态 |
|--------|--------------|------|
| VBox Layout | `lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN)` | ✅ 保留 |
| HBox Layout | `lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW)` | ✅ 保留 |
| Grid Layout | `lv_obj_set_grid_dsc_array` + `lv_obj_set_grid_cell` | ✅ 保留 |
| Stacked Layout | child hidden / current_index 控制 | ✅ 保留（简化） |

### 已裁剪的布局器
| 布局器 | 原因 | 状态 |
|--------|------|------|
| Form Layout | LVGL 无原生 FormLayout；表单需求用 GridLayout（2 列）实现 | ❌ 已移除 |

### 保留的 Widget
全部保留，均映射到 LVGL 原生 widget 创建 API。

---

## 二、交互模型

### 2.1 自由对象
**适用**：非布局容器子项、不受父布局控制的对象。

| 交互 | 允许 | 说明 |
|------|------|------|
| 自由拖动改位置 | ✅ | `lv_obj_set_pos` |
| Resize handle 改尺寸 | ✅ | `lv_obj_set_size` |
| 选中高亮 + outline | ✅ | 8 个 handle |
| 属性面板编辑 | ✅ | 所有原生 style/属性 |

### 2.2 VBox / HBox 子项
**适用**：父容器为 LVBoxLayout 或 LHBoxLayout 的子对象。

| 交互 | 允许 | 说明 |
|------|------|------|
| 拖动换位（顺序重排） | ✅ | 改变 parent 下 children 的顺序，LVGL Flex 自动重排 |
| 自由拖动改绝对坐标 | ❌ | Flex 子项位置由布局决定，不是 `lv_obj_set_pos` |
| Resize handle | ❌ | 子项尺寸由 flex 规则控制 |
| 属性面板编辑 | ✅ | flex_grow / min/max size 等 LVGL 原生属性 |

### 2.3 GridLayout 子项
**适用**：父容器为 LGridLayout 的子对象。

| 交互 | 允许 | 说明 |
|------|------|------|
| 设置 grid cell（col/row/span/align） | ✅ | `lv_obj_set_grid_cell`，唯一的位置来源 |
| 自由拖动改绝对坐标 | ❌ | Grid 子项位置由 cell 决定，不是 `lv_obj_set_pos` |
| Resize handle | ❌ | 子项尺寸由 grid cell stretch/align 决定 |
| 属性面板编辑 | ✅ | grid_col / grid_row / grid_col_span / grid_row_span / align |

**核心规则**：Grid 子项的位置必须唯一来源于 grid cell 属性，不能既用 `lv_obj_set_grid_cell` 又用 `lv_obj_set_pos`。

### 2.4 StackedLayout 子项
**适用**：父容器为 LStackedLayout 的子对象。

| 交互 | 允许 | 说明 |
|------|------|------|
| 切换 current_index | ✅ | 显示/隐藏 child |
| 自由拖动 | ❌ | 堆叠语义下位置无意义 |
| Resize handle | ❌ | 子项通常填满父容器 |
| 属性面板编辑 | ✅ | current_index 等 |

---

## 三、选择模型

### 3.1 选中高亮
所有对象选中时均显示 outline 高亮。

### 3.2 Resize Handles（8 个角/边方块）
仅对**自由对象**创建。布局子项不创建 handle，因为 handle 传达的是"可自由定位"的语义，与布局控制冲突。

### 3.3 多选
保留多选能力，用于：
- 多个自由对象批量移动
- 多个对象应用布局（包裹进 VBox/HBox/Grid）

---

## 四、文档模型（LuiDocument）

### 4.1 每个节点保存的属性
仅保存 LVGL 原生可表达的属性：
- type（组件类型）
- id（唯一标识）
- parentId（父容器）
- pos（自由对象位置，布局子项忽略）
- size（自由对象尺寸，布局子项部分忽略）
- LVGL 原生 style 属性（padding/spacing/color/radius/...）
- Grid 专属：grid_col / grid_row / grid_col_span / grid_row_span
- Flex 专属：flex_grow / min_size / max_size

### 4.2 不保存的属性
- Qt 风格但 LVGL 无法稳定表达的布局语义
- 设计器内部状态（如 handle 位置）

---

## 五、代码生成

导出的代码必须是合法的 LVGL C++ 代码：

### VBox / HBox
```cpp
auto container = lv_obj_create(parent);
lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN); // or ROW
lv_obj_set_flex_align(container, ...);
lv_obj_set_style_pad_row(container, spacing, 0);
lv_obj_set_style_pad_column(container, spacing, 0);
```

### Grid
```cpp
auto container = lv_obj_create(parent);
lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
lv_obj_set_grid_dsc_array(container, col_dsc, row_dsc);
// child:
lv_obj_set_grid_cell(child, LV_GRID_ALIGN_STRETCH, col, col_span,
                      LV_GRID_ALIGN_STRETCH, row, row_span);
```

### Stacked
```cpp
// 只显示 current_index 对应的 child
for (auto* child : children) {
    lv_obj_add_flag(child, LV_OBJ_FLAG_HIDDEN);
}
lv_obj_clear_flag(children[current_index], LV_OBJ_FLAG_HIDDEN);
```

---

## 六、Qt Creator 对标边界

### 对标的体验层
- ✅ 左侧组件按类别分组
- ✅ 树面板展示 parent-child
- ✅ 属性面板可视化编辑
- ✅ 工具栏布局入口（VBox/HBox/Grid/Stacked/Break Layout）
- ✅ 多选
- ✅ 撤销/重做
- ✅ 选中高亮
- ✅ 文档结构清晰

### 不对标的
- ❌ Qt 独有布局语义（FormLayout 等）
- ❌ Qt Designer 超出 LVGL 原生能力的交互方式
- ❌ 布局子项的自由坐标拖动

---

## 七、已知高风险点（已处理或待处理）

### 已处理
- ✅ SDL 窗口所有权冲突 → 移除 SDL_CreateWindowFrom，只用 Qt QPainter 渲染 LVGL framebuffer
- ✅ FormLayout 从组件面板下线
- ✅ GridLayout 事件回调已做 pending queue 异步化

### 待处理
- ⬜ Grid 子项的自由拖动需改为 cell 编辑
- ⬜ VBox/HBox 子项拖动需改为顺序重排
- ⬜ 布局子项的 resize handle 需按父容器类型禁用
- ⬜ Grid 子项属性面板增加 grid_col / grid_row / grid_col_span / grid_row_span 编辑

---

## 八、判断标准

以后评估任何新功能，只问：

> **这个能力，能不能直接映射成稳定的 LVGL 原生代码？**

- **能** → 保留，做封装
- **不能** → 暂时不做
- **需要模拟 Qt 才能成立** → 不做
