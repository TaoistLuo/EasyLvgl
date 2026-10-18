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
| resources | object | 否 | 图片资源表：别名 → 路径（相对 .lui 所在目录），见下节 |
| children | array | 是 | 子组件列表 |

### 组件对象

| 字段 | 类型 | 必需 | 说明 |
|------|------|------|------|
| type | string | 是 | 组件类型（如 "LButton"，使用实际 C++ 类名） |
| id | string | 是 | 组件 ID，用于生成成员变量名（如 btn_ok → m_btn_ok） |
| props | object | 否 | 属性键值对 |
| events | array | 否 | 事件列表（如 ["clicked"]） |
| children | array | 否 | 子组件（仅容器组件） |

### 图片资源（qrc 式编译期嵌入）

等同于 Qt 的 `.qrc`：图片在**编译期**转成 `lv_image_dsc_t` 常量数组，运行时不再
读文件，也不需要 `lv_fs`。

```json
{
    "name": "EmbeddedImage",
    "size": [300, 160],
    "resources": {
        "caret": "assets/caret_down.png"
    },
    "children": [
        { "type": "LImage", "id": "logo",
          "props": { "pos": [40, 60], "size": [80, 60], "src": "#caret" } }
    ]
}
```

三步：

1. **声明并引用**：`resources` 里给图片起别名，控件属性写 `"#别名"`。
   别名必须是合法的 C 标识符（它同时是生成代码里的变量名）。
2. **转成 C 数组**（改图后执行一次）：

   ```bash
   pip3 install pypng lz4          # 官方转换器的依赖
   python3 scripts/assets/convert_assets.py path/to/screen.lui
   ```

   生成 `assets/<别名>.c`（`lv_image_dsc_t` 常量）和 `assets/lvgl_assets.h`
   （所有资源的声明）。颜色格式默认 `RGB565A8`（16 位色 + 透明，省空间）；
   需要和设计器预览逐像素一致时用 `--cf ARGB8888`。

3. **编译**：把 `assets/*.c` 加进目标，并让 luic 生成正确的 include：

   ```cmake
   add_executable(app main.cpp ${LUI_GENERATED} ${ASSET_SOURCES})
   target_include_directories(app PRIVATE path/to)   # 为了 #include "assets/lvgl_assets.h"
   add_custom_command(OUTPUT ${LUI_GENERATED}
       COMMAND luic screen.lui -o screen_ui.hpp --assets-include "assets/lvgl_assets.h"
       DEPENDS screen.lui)
   ```

生成的代码：

```cpp
#include "assets/lvgl_assets.h"
...
m_logo = new LImage(root);
m_logo->set_src(&caret);          // 指向固件里的常量数组
```

完整可运行示例见 `examples/embedded_image/`（`make embedded_image_demo`，
改图后 `make convert_assets`）。

> 注意：`assets/*.c` 是 C99 聚合初始化，必须按 C 编译（CMake 对 `.c` 默认如此），
> 不要塞进 C++ 源文件。
>
> `"#别名"` 与 `A:` 开头的 lv_fs 路径可以共存：前者编译期嵌入，后者运行时读文件。
> 引用了未在 `resources` 里声明的别名，luic 会直接报错。

### 支持的组件

> **总计 43 个组件** 已在 ComponentRegistry 中注册。

> **通用属性：** 所有组件均支持 `size`（[w,h]）和 `pos`（[x,y]）；下表仅列组件特有属性。

#### 核心组件
| 类型 | 属性 | 事件 |
|------|------|------|
| LButton | text, theme, disabled | clicked, pressed, released |
| LLabel | text, color, font_size, long_mode | - |
| LSlider | range, value, left_value, mode | value_changed |
| LCheckbox | text, checked | value_changed |
| LSwitch | checked, enabled | value_changed |
| LComboBox | options, selected, direction, symbol, text, selected_highlight, enabled | value_changed |
| LLineEdit | text, placeholder, password_mode, password_show_time, max_length, accepted_chars, cursor_click_pos, text_selection, enabled | value_changed, ready |
| LTextEdit | text, placeholder, password_mode, password_show_time, max_length, accepted_chars, cursor_click_pos, text_selection, enabled | value_changed, ready |
| LImage | src, rotation, scale, pivot, offset, opacity, antialias | clicked |

#### 高频组件
| 类型 | 属性 | 事件 |
|------|------|------|
| LArc | range, value, rotation, mode, bg_angles, angles, change_rate, knob_enabled | value_changed |
| LBar | range, value, start_value, mode, orientation | - |
| LSpinner | color | - |
| LLed | color, brightness, on | - |
| LLine | points, width, color, y_invert | - |

#### 输入组件
| 类型 | 属性 | 事件 |
|------|------|------|
| LRoller | options, selected, visible_row_count | value_changed |
| LSpinbox | range, value, step, digit_format, rollover | value_changed |
| LButtonMatrix | map, selected, one_checked | value_changed |
| LKeyboard | textarea（关联输入框的组件 id）, mode, popovers | - |
| LRadioButton | text, checked | value_changed |

#### 数据展示组件
| 类型 | 属性 | 事件 |
|------|------|------|
| LTable | row_count, column_count, column_widths, data | - |
| LChart | type, point_count, div_line_count, update_mode, series | - |
| LScale | mode, range, total_tick_count, major_tick_every, rotation, angle_range, label_show | - |
| LCalendar | today_date, showed_date | - |
| LCanvas | （仅尺寸/位置；绘制需代码） | - |

#### 布局容器
| 类型 | 属性 | 事件 |
|------|------|------|
| LVBoxLayout | spacing, padding, align_main, align_cross | - |
| LHBoxLayout | spacing, padding, align_main, align_cross | - |
| LGridLayout | columns, rows, col_spacing, row_spacing, padding | - |
| LStackedLayout | current_index, padding | - |

#### 其他容器
| 类型 | 属性 | 事件 |
|------|------|------|
| LList | items | - |
| LTabView | tabs, tab_bar_position, tab_bar_size | - |
| LMenu | mode_header, mode_root_back_button, active_page（页面 id） | - |
| LMenuPage | title | - |
| LMenuCont | - | - |
| LMenuSection | - | - |
| LTileView | - | - |
| LTile | col, row, dir | - |
| LWin | title | - |
| LGroupBox | title, padding | - |

#### 特殊组件
| 类型 | 属性 | 事件 |
|------|------|------|
| LMsgBox | title, text | - |
| LSpan | spans, mode, overflow, indent | - |
| LAnimImage | duration, repeat_count | - |
| LImageButton | state, src_released, src_pressed, src_disabled | clicked |
| LLottie | src, loop, speed | 需 LV_USE_LOTTIE=1（默认未启用） |

### 属性类型

| 类型 | 格式 | 示例 |
|------|------|------|
| string | "text" | "Hello World" |
| int | 42 | 50 |
| hex | "0xRRGGBB" | "0x2196F3" |
| int_pair | [a, b] | [120, 50] |
| int_triple | [a, b, c] | [2026, 1, 15] |
| bool | true/false | true |

> **列表类属性**（`map`/`data`/`series`/`tabs`/`items`/`spans`/`points` 等）以分隔符字符串表达：行用 `\n`、单元用 `|`、系列用 `;`、点对用 `x,y;x,y`。

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
