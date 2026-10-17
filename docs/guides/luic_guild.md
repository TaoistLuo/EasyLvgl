# luic 编译器使用指南

luic 是 LVGL++ 的 UI 编译器（C++ 实现），将 .lui 文件（JSON 格式）编译为 C++ 代码。

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
tools/luic/bin/luic ui/my_screen.lui -o ui_generated/my_screen_ui.hpp

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
| name | string | 是 | 屏幕名称，用于生成类名（如 MainScreen → MainScreen_UI） |
| size | [int, int] | 否 | 屏幕尺寸 [宽, 高] |
| children | array | 是 | 子组件列表 |

### 组件对象

| 字段 | 类型 | 必需 | 说明 |
|------|------|------|------|
| type | string | 是 | 组件类型（如 "LButton"，使用实际 C++ 类名） |
| id | string | 是 | 组件 ID，用于生成成员变量名（如 btn_ok → m_btn_ok） |
| props | object | 否 | 属性键值对 |
| events | array | 否 | 事件列表（如 ["clicked"]） |
| children | array | 否 | 子组件（仅容器组件） |

### 支持的组件

> **总计 34 个组件** 已在 ComponentRegistry 中注册。

#### 核心组件
| 类型 | 属性 | 事件 |
|------|------|------|
| LButton | text, size, pos, theme, disabled | clicked, pressed, released |
| LLabel | text, color, font_size, long_mode, pos | - |
| LSlider | range, value, left_value, mode, pos | value_changed |
| LCheckbox | text, checked, pos | value_changed |
| LSwitch | checked, enabled, pos | value_changed |
| LComboBox | options, selected, direction, text, pos | value_changed |
| LLineEdit | text, placeholder, password_mode, max_length, pos | value_changed, ready |
| LTextEdit | text, placeholder, password_mode, max_length, pos | value_changed, ready |
| LImage | src, rotation, scale, pivot, offset, opacity, pos | clicked |

#### 高频组件
| 类型 | 属性 | 事件 |
|------|------|------|
| LArc | range, value, rotation, mode, bg_angles, angles, pos | value_changed |
| LBar | range, value, start_value, mode, orientation, pos | - |
| LSpinner | color, size, pos | - |
| LLed | color, brightness, on, size, pos | - |
| LLine | width, color, y_invert, size, pos | - |

#### 输入组件
| 类型 | 属性 | 事件 |
|------|------|------|
| LRoller | options, selected, visible_row_count, pos | value_changed |
| LSpinbox | range, value, step, digit_format, rollover, pos | value_changed |
| LButtonMatrix | one_checked, pos | value_changed |
| LKeyboard | mode, popovers, pos | - |
| LRadioButton | text, checked, pos | value_changed |

#### 数据展示组件
| 类型 | 属性 | 事件 |
|------|------|------|
| LTable | row_count, column_count, pos | - |
| LChart | type, point_count, div_line_count, update_mode, pos | - |
| LScale | mode, range, total_tick_count, major_tick_every, rotation, pos | - |
| LCalendar | today_date, showed_date, pos | - |
| LCanvas | size, pos | - |

#### 布局容器
| 类型 | 属性 | 事件 |
|------|------|------|
| LVBoxLayout | spacing, padding, align_main, align_cross, pos | - |
| LHBoxLayout | spacing, padding, align_main, align_cross, pos | - |
| LGridLayout | columns, rows, col_spacing, row_spacing, pos | - |
| LStackedLayout | current_index, pos | - |

#### 其他容器
| 类型 | 属性 | 事件 |
|------|------|------|
| LList | size, pos | - |
| LTabView | tab_bar_position, tab_bar_size, pos | - |
| LMenu | mode_header, mode_root_back_button, pos | - |
| LTileView | size, pos | - |
| LWin | size, pos | - |
| LGroupBox | title, padding, size, pos | - |

#### 特殊组件
| 类型 | 属性 | 事件 |
|------|------|------|
| LMsgBox | title, text | - |
| LSpan | mode, overflow, indent, pos | - |
| LAnimImage | duration, repeat_count, pos | - |
| LImageButton | size, pos | clicked |
| LLottie | src, loop, speed, pos | ready |

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
