# LVGL++ JSON UI 格式设计

**版本：** 1.0.0  
**日期：** 2026-03-15

---

## 📋 设计目标

1. **简洁易读**：JSON 格式，易于人工编辑和版本控制
2. **完整性**：支持所有 LVGL++ 组件和属性
3. **可扩展**：易于添加新组件和属性
4. **类型安全**：运行时验证和错误提示

---

## 🎨 JSON 格式示例

### 基础示例

```json
{
  "version": "1.0",
  "screen": {
    "type": "screen",
    "id": "main_screen",
    "children": [
      {
        "type": "button",
        "id": "btn_login",
        "properties": {
          "text": "Login",
          "size": [120, 50],
          "pos": [100, 100],
          "style": "primary"
        },
        "events": {
          "clicked": "on_login_clicked"
        }
      },
      {
        "type": "label",
        "id": "lbl_title",
        "properties": {
          "text": "Welcome to LVGL++",
          "pos": [50, 20],
          "color": "#2196F3",
          "fontSize": 24
        }
      }
    ]
  }
}
```

### 复杂布局示例

```json
{
  "version": "1.0",
  "screen": {
    "type": "screen",
    "id": "dashboard",
    "children": [
      {
        "type": "vstack",
        "id": "main_container",
        "properties": {
          "spacing": 10,
          "padding": 20,
          "size": [800, 600]
        },
        "children": [
          {
            "type": "label",
            "properties": {
              "text": "Dashboard",
              "fontSize": 32,
              "color": "#333333"
            }
          },
          {
            "type": "hstack",
            "properties": {
              "spacing": 15
            },
            "children": [
              {
                "type": "card",
                "properties": {
                  "title": "Temperature",
                  "value": "25°C",
                  "icon": "thermometer"
                }
              },
              {
                "type": "card",
                "properties": {
                  "title": "Humidity",
                  "value": "60%",
                  "icon": "droplet"
                }
              }
            ]
          },
          {
            "type": "chart",
            "id": "temp_chart",
            "properties": {
              "size": [760, 300],
              "type": "line",
              "data": "temperature_data"
            }
          }
        ]
      }
    ]
  },
  "data": {
    "temperature_data": [20, 22, 25, 23, 26, 28, 25]
  },
  "handlers": {
    "on_login_clicked": "handleLogin",
    "on_refresh": "refreshData"
  }
}
```

---

## 🔧 格式规范

### 1. 根对象

```json
{
  "version": "1.0",           // 格式版本
  "screen": { ... },          // 主屏幕定义
  "data": { ... },            // 数据绑定
  "handlers": { ... },        // 事件处理器映射
  "styles": { ... },          // 自定义样式
  "theme": "light"            // 主题
}
```

### 2. 组件对象

```json
{
  "type": "button",           // 组件类型（必需）
  "id": "btn_submit",         // 组件 ID（可选，用于引用）
  "properties": { ... },      // 属性
  "events": { ... },          // 事件
  "children": [ ... ]         // 子组件（容器组件）
}
```

### 3. 属性对象

```json
{
  "text": "Click Me",         // 文本
  "size": [120, 50],          // 尺寸 [width, height]
  "pos": [10, 20],            // 位置 [x, y]
  "align": "center",          // 对齐方式
  "color": "#FF0000",         // 颜色（十六进制）
  "fontSize": 16,             // 字体大小
  "visible": true,            // 可见性
  "enabled": true             // 启用状态
}
```

### 4. 事件对象

```json
{
  "clicked": "on_button_click",      // 点击事件
  "value_changed": "on_value_change", // 值变化事件
  "pressed": "on_press",              // 按下事件
  "released": "on_release"            // 释放事件
}
```

---

## 📊 支持的组件类型

### 核心组件
- `button` - 按钮
- `label` - 标签
- `slider` - 滑块
- `checkbox` - 复选框
- `switch` - 开关
- `dropdown` - 下拉列表
- `textarea` - 文本输入
- `image` - 图片

### 容器组件
- `vstack` - 垂直堆栈
- `hstack` - 水平堆栈
- `grid` - 网格布局
- `scroll` - 滚动容器

### 高级组件
- `chart` - 图表
- `table` - 表格
- `tabview` - 标签页
- `calendar` - 日历
- `keyboard` - 虚拟键盘

---

## 🎯 数据绑定

### 静态数据

```json
{
  "type": "label",
  "properties": {
    "text": "Hello World"
  }
}
```

### 动态数据绑定

```json
{
  "type": "label",
  "properties": {
    "text": "${user.name}",
    "color": "${theme.primaryColor}"
  }
}
```

### 数据源定义

```json
{
  "data": {
    "user": {
      "name": "John Doe",
      "email": "john@example.com"
    },
    "theme": {
      "primaryColor": "#2196F3",
      "secondaryColor": "#FFC107"
    }
  }
}
```

---

## 🔄 运行时加载流程

```
1. 解析 JSON 文件
   ↓
2. 验证格式和类型
   ↓
3. 创建组件树
   ↓
4. 应用属性和样式
   ↓
5. 绑定事件处理器
   ↓
6. 渲染到屏幕
```

---

## 💡 优势

1. **可视化编辑**：设计器直接生成 JSON
2. **版本控制友好**：文本格式，易于 diff 和 merge
3. **团队协作**：设计师和开发者分离
4. **快速原型**：无需编译，直接加载
5. **热重载**：修改 JSON 立即生效
6. **跨平台**：同一份 JSON 可用于不同平台

---

## 🚀 下一步

1. 实现 JSON 解析器
2. 实现组件工厂
3. 实现数据绑定引擎
4. 实现事件处理器映射
5. 开发可视化设计器

---

**设计者：** LVGL++ Team  
**最后更新：** 2026-03-15
