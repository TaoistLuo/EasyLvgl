# LVGL++ JSON UI 格式设计（.lui）

**版本：** 2.0.0  
**日期：** 2026-03-15  
**状态：** ✅ 已实现（luic 编译器）

---

## 📋 设计目标

1. **简洁易读**：JSON 格式，易于人工编辑和版本控制
2. **编译时生成**：.lui 文件通过 luic 编译器生成纯 C++ 代码，零运行时开销
3. **可扩展**：通过 ComponentRegistry 注册新组件
4. **类型安全**：编译时验证组件类型和属性

---

## 🎨 .lui 格式示例

### 基础示例

```json
{
  "version": "1.0",
  "name": "MainScreen",
  "size": [800, 480],
  "children": [
    {
      "type": "LLabel",
      "id": "title",
      "props": {
        "text": "Welcome to LVGL++",
        "color": "0x2196F3",
        "font_size": 24
      }
    },
    {
      "type": "LButton",
      "id": "btn_login",
      "props": {
        "text": "Login",
        "size": [120, 50],
        "theme": "primary"
      },
      "events": ["clicked"]
    }
  ]
}
```

### 复杂布局示例

```json
{
  "version": "1.0",
  "name": "Dashboard",
  "size": [800, 480],
  "children": [
    {
      "type": "LLabel",
      "id": "dash_title",
      "props": {
        "text": "Dashboard",
        "font_size": 32,
        "color": "0x333333"
      }
    },
    {
      "type": "LVBoxLayout",
      "id": "main_container",
      "props": {
        "spacing": 10,
        "padding": 20,
        "size": [800, 600]
      },
      "children": [
        {
          "type": "LHBoxLayout",
          "id": "card_row",
          "props": {
            "spacing": 15
          },
          "children": [
            {
              "type": "LLabel",
              "id": "temp_label",
              "props": {
                "text": "Temperature: 25°C",
                "font_size": 18
              }
            },
            {
              "type": "LLabel",
              "id": "humidity_label",
              "props": {
                "text": "Humidity: 60%",
                "font_size": 18
              }
            }
          ]
        },
        {
          "type": "LChart",
          "id": "temp_chart",
          "props": {
            "size": [760, 300],
            "type": 1,
            "point_count": 7
          }
        }
      ]
    }
  ]
}
```

---

## 🔧 格式规范

### 1. 根对象

```json
{
  "version": "1.0",           // 格式版本（必需）
  "name": "MainScreen",       // 屏幕名称，用于生成类名（必需）
  "size": [800, 480],         // 屏幕尺寸 [宽, 高]（可选）
  "children": [ ... ]         // 子组件列表（必需）
}
```

> **注意：** 与早期设计不同，当前格式不包含 `data`、`handlers`、`styles`、`theme` 字段。
> 数据绑定功能尚未实现，事件绑定在组件对象的 `events` 字段中。

### 2. 组件对象

```json
{
  "type": "LButton",          // 组件类型（必需，使用实际 C++ 类名）
  "id": "btn_submit",         // 组件 ID（必需，用于生成成员变量 m_btn_submit）
  "props": { ... },           // 属性键值对（可选）
  "events": [ ... ],          // 事件列表（可选，如 ["clicked"]）
  "children": [ ... ]         // 子组件（仅容器组件，可选）
}
```

**字段名说明：**
- `props`（不是 `properties`）：组件属性
- `events`：事件列表（数组格式，如 `["clicked", "pressed"]`）
- `children`：子组件数组

### 3. 属性对象（props）

```json
{
  "text": "Click Me",         // 字符串属性
  "size": [120, 50],          // 尺寸 [width, height]（int_pair）
  "pos": [10, 20],            // 位置 [x, y]（int_pair）
  "color": "0x2196F3",        // 颜色（十六进制字符串）
  "font_size": 16,            // 整数属性
  "checked": true,            // 布尔属性
  "theme": "primary"          // 主题属性（仅 LButton）
}
```

### 4. 属性类型

| 类型 | 格式 | 示例 |
|------|------|------|
| string | "text" | "Hello World" |
| int | 42 | 50 |
| hex | "0xRRGGBB" | "0x2196F3" |
| int_pair | [a, b] | [120, 50] |
| bool | true/false | true |

### 5. 事件对象

```json
"events": ["clicked"]                     // 点击事件
"events": ["value_changed"]               // 值变化事件
"events": ["clicked", "pressed"]          // 多个事件
```

---

## 📊 支持的组件类型

### 核心组件
| 类型 | 关键属性 | 事件 |
|------|---------|------|
| `LButton` | text, size, pos, theme | clicked, pressed, released |
| `LLabel` | text, color, font_size, pos | - |
| `LSlider` | range, value, size, pos | value_changed |
| `LCheckbox` | text, checked, pos | value_changed |
| `LSwitch` | checked, pos | value_changed |
| `LImage` | src, rotation, scale, pos | clicked |
| `LComboBox` | options, selected, text, pos | value_changed |
| `LLineEdit` | text, placeholder, password_mode, pos | value_changed, ready |

### 高频组件
| 类型 | 关键属性 | 事件 |
|------|---------|------|
| `LArc` | range, value, rotation, size, pos | value_changed |
| `LBar` | range, value, size, pos | - |
| `LSpinner` | color, size, pos | - |
| `LLed` | color, brightness, on, size, pos | - |
| `LLine` | width, color, size, pos | - |

### 输入组件
| 类型 | 关键属性 | 事件 |
|------|---------|------|
| `LRoller` | options, selected, pos | value_changed |
| `LSpinbox` | range, value, step, pos | value_changed |
| `LButtonMatrix` | one_checked, pos | value_changed |
| `LKeyboard` | mode, popovers, pos | - |
| `LTextEdit` | text, placeholder, pos | value_changed, ready |
| `LRadioButton` | text, checked, pos | value_changed |

### 数据展示组件
| 类型 | 关键属性 | 事件 |
|------|---------|------|
| `LTable` | row_count, column_count, pos | - |
| `LChart` | type, point_count, pos | - |
| `LScale` | mode, range, rotation, pos | - |
| `LCalendar` | today_date, showed_date, pos | - |
| `LCanvas` | size, pos | - |

### 布局容器
| 类型 | 关键属性 | 事件 |
|------|---------|------|
| `LVBoxLayout` | spacing, padding, align_main, size, pos | - |
| `LHBoxLayout` | spacing, padding, align_main, size, pos | - |
| `LGridLayout` | columns, rows, col_spacing, row_spacing, pos | - |
| `LFormLayout` | label_width, row_spacing, col_spacing, pos | - |
| `LStackedLayout` | current_index, size, pos | - |

### 其他容器
| 类型 | 关键属性 | 事件 |
|------|---------|------|
| `LList` | size, pos | - |
| `LTabView` | tab_bar_position, tab_bar_size, pos | - |
| `LMenu` | mode_header, mode_root_back_button, pos | - |
| `LTileView` | size, pos | - |
| `LWin` | size, pos | - |
| `LGroupBox` | title, padding, size, pos | - |

### 特殊组件
| 类型 | 关键属性 | 事件 |
|------|---------|------|
| `LMsgBox` | title, text | - |
| `LSpan` | mode, overflow, indent, pos | - |
| `LAnimImage` | duration, repeat_count, pos | - |
| `LImageButton` | size, pos | clicked |
| `LLottie` | src, loop, speed, pos | ready |

> **总计：34 个组件** 已在 ComponentRegistry 中注册。

---

## 🔄 编译流程

```
1. 编写 .lui 文件（JSON 格式）
   ↓
2. luic 编译器解析 JSON（C++ 实现，位于 tools/luic/）
   ↓
3. ComponentRegistry 验证组件类型和属性
   ↓
4. CodeGenerator 生成 C++ 头文件
   ↓
5. 得到 xxx_ui.hpp（纯 C++ 代码，零运行时开销）
```

---

## 💡 优势

1. **零运行时开销**：编译时生成 C++ 代码，不需要 JSON 解析器
2. **可视化编辑**：设计器直接生成/编辑 .lui 文件
3. **版本控制友好**：JSON 文本格式，易于 diff 和 merge
4. **团队协作**：设计师使用设计器，开发者编写业务代码
5. **类型安全**：luic 编译时检查组件类型和属性
6. **跨平台**：同一份 .lui 可用于不同平台

---

## ⚠️ 未实现功能

### 数据绑定（FUTURE / NOT IMPLEMENTED）

早期设计包含的数据绑定功能（`${user.name}` 语法、data 字段、双向绑定）尚未实现。

当前方案是编译时代码生成，数据绑定需要运行时支持，与当前架构（零运行时开销）存在冲突。
如需动态更新 UI，请在业务代码中直接调用 `set_text()` 等方法。

### 热重载（NOT IMPLEMENTED）

修改 .lui 文件后需要重新运行 luic 编译器并重新编译 C++ 代码。

---

## 🚀 相关工具

| 工具 | 路径 | 说明 |
|------|------|------|
| luic 编译器 | `tools/luic/` | C++ 实现，将 .lui 编译为 C++ 代码 |
| Qt 设计器 | `tools/lvgl-designer/` | 桌面可视化设计器（Qt5） |
| VSCode 插件 | `vscode-lvgl-designer/` | VSCode 内嵌编辑器 |

---

**设计者：** LVGL++ Team  
**最后更新：** 2026-03-15
