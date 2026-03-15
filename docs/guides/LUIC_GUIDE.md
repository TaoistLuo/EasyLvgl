# luic 编译器使用指南

luic 是 LVGL++ 的 UI 编译器，将 .lui 文件（JSON 格式）编译为 C++ 代码。

## 快速开始

### 1. 创建 .lui 文件

```json
{
  "version": "1.0",
  "name": "MyScreen",
  "size": [800, 480],
  "children": [
    {
      "type": "LLabel",
      "id": "title",
      "props": { "text": "Hello", "color": "0x2196F3" }
    },
    {
      "type": "LButton",
      "id": "btn_ok",
      "props": { "text": "OK", "size": [120, 50], "theme": "primary" },
      "events": ["clicked"]
    }
  ]
}
```

### 2. 编译

```bash
# 手动编译
bin/luic ui/my_screen.lui -o ui_generated/my_screen_ui.hpp

# 或者用 make 自动编译（推荐）
cd build && make ui_compile
```

### 3. 使用生成的代码

```cpp
#include "my_screen_ui.hpp"

class MyScreen : public lvgl::MyScreen_UI {
public:
    void on_btn_ok_clicked(lv_event_t* e) override {
        printf("OK clicked!\n");
    }
};

int main() {
    lv_init();
    // ...
    MyScreen screen;
    screen.setup();
    screen.bind_events();
    // ...
}
```

## 命令行参数

```
luic <input.lui> [-o output.hpp] [--verbose]

参数：
  input.lui       输入的 .lui 文件
  -o <file>       输出文件路径（默认：<input>_ui.hpp）
  --verbose       显示详细信息
  --help          显示帮助
```

## .lui 文件格式

### 根对象

| 字段 | 类型 | 必需 | 说明 |
|------|------|------|------|
| version | string | 是 | 格式版本，当前 "1.0" |
| name | string | 是 | 屏幕名称，用于生成类名 |
| size | [int, int] | 否 | 屏幕尺寸 [宽, 高] |
| children | array | 是 | 子组件列表 |

### 组件对象

| 字段 | 类型 | 必需 | 说明 |
|------|------|------|------|
| type | string | 是 | 组件类型（如 "LButton"） |
| id | string | 是 | 组件 ID，用于生成成员变量名 |
| props | object | 否 | 属性键值对 |
| events | array | 否 | 事件列表（如 ["clicked"]） |
| children | array | 否 | 子组件（仅容器组件） |

### 支持的组件

#### 核心组件
| 类型 | 属性 | 事件 |
|------|------|------|
| LButton | text, size, pos, theme | clicked |
| LLabel | text, color, font_size, pos | - |
| LSlider | range, value, size, pos | value_changed |
| LCheckbox | text, checked, pos | value_changed |
| LSwitch | checked, pos | value_changed |
| LDropdown | options, selected, size, pos | value_changed |
| LTextArea | text, placeholder, one_line, size, pos | value_changed |
| LImage | src, size, pos | - |

#### 高频组件
| 类型 | 属性 | 事件 |
|------|------|------|
| LArc | range, value, rotation, size, pos | value_changed |
| LBar | range, value, size, pos | - |
| LSpinner | size, pos | - |
| LLed | color, brightness, on, size, pos | - |
| LLine | pos | - |
| LList | size, pos | - |

#### 容器组件
| 类型 | 属性 | 事件 |
|------|------|------|
| LVStack | spacing, padding, size, pos | - |
| LHStack | spacing, padding, size, pos | - |

### 属性类型

| 类型 | 格式 | 示例 |
|------|------|------|
| string | "text" | "Hello World" |
| int | 42 | 50 |
| hex | "0xRRGGBB" | "0x2196F3" |
| int_pair | [a, b] | [120, 50] |
| bool | true/false | true |

### theme 属性

LButton 支持以下主题值：
- `"primary"` — 蓝色
- `"danger"` — 红色
- `"success"` — 绿色
- `"warning"` — 橙色

## 生成的代码结构

```cpp
class MyScreen_UI {
protected:
    LLabel*  m_title = nullptr;     // 成员变量（m_ 前缀）
    LButton* m_btn_ok = nullptr;

public:
    void setup(lv_obj_t* parent = nullptr);  // 创建所有组件
    void bind_events();                       // 绑定事件

    virtual void on_btn_ok_clicked(lv_event_t* e) {}  // 事件槽

    virtual ~MyScreen_UI();  // 析构（逆序删除）
};
```

用户继承这个类，override 事件函数即可：

```cpp
class MyScreen : public lvgl::MyScreen_UI {
    void on_btn_ok_clicked(lv_event_t* e) override {
        // 你的业务逻辑
    }
};
```

## CMake 集成

项目的 CMakeLists.txt 已经配置好自动编译：

```bash
cd build
make              # 自动检测 .lui 变化，调用 luic 重新生成
make ui_compile   # 只编译 .lui 文件
make luic         # 只编译 luic 工具本身
```

## 运行测试

```bash
bash tests/luic/run_tests.sh
```

测试用例：
- test_minimal.lui — 最小文件（1 个组件）
- test_core_widgets.lui — 核心组件（8 个）
- test_nested.lui — 嵌套容器
- test_invalid_type.lui — 错误类型（应该报错）
- test_empty.lui — 空文件（应该报错）
