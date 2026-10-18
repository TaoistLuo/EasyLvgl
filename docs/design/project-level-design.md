# LVGL++ 工程级设计（完整功能支持 + 工程化规划）

**版本：** 1.0
**状态：**
- **第一部分（§1–§10）：已实现**，内容与代码一致，每节末尾给出对应源文件
- **第二部分（§11–§20）：设计规划，尚未实现**（`kind` / `exports` / `@Name` / 工程清单等）

**关联文档：** [json-ui-format.md](json-ui-format.md)、[architecture-design.md](architecture-design.md)、[luic 编译器指南](../guides/luic_guild.md)、[DEVELOPMENT_GUIDELINES.md](../../DEVELOPMENT_GUIDELINES.md)

---

# 第一部分　现状：功能支持与特性

## 1. 定位与设计原则

LVGL++ 的目标是**让 LVGL 开发像 Qt 一样简单**：拖拽搭界面 → 生成纯 C++ → 手写业务逻辑。

三条硬性原则（来自 `DEVELOPMENT_GUIDELINES.md`，贯穿全部设计）：

| # | 原则 | 具体落地 |
|---|---|---|
| 1 | **只封装 LVGL 原生能力**，不做 LVGL 没有的功能 | 43 个组件中 41 个是 LVGL 原生控件的薄封装；`LRadioButton`/`LGroupBox` 由 `lv_obj` + 子对象组合而成（LVGL 无原生对应）；`LLottie` 在模块未启用时是占位 |
| 2 | 能直接映射成**稳定的 LVGL 原生代码**才做；需要模拟 Qt 才成立的不做 | 不做响应式绑定引擎、不做自绘特效、不做 moc/反射 |
| 3 | **不生效的属性不显示** | 属性面板按"该组件是否真的支持"过滤（见 §9） |

---

## 2. 架构总览

```
┌───────────────────────────────────────────────────────────────┐
│  lvgl-designer （Qt 桌面设计器）                               │
│  组件面板 / 画布 / 组件树 / 属性面板 / 撤销栈 / 资源管理        │
└───────────────────────┬───────────────────────────────────────┘
                        │ 读写 .lui（JSON）
                        ▼
┌───────────────────────────────────────────────────────────────┐
│  .lui 界面描述（纯声明式数据，无逻辑）                          │
│  version / name / size / resources / children(props, events)  │
└───────────────────────┬───────────────────────────────────────┘
                        │  luic 编译（校验 + 代码生成）
                        ▼
┌───────────────────────────────────────────────────────────────┐
│  *_ui.hpp （C++ 类，零运行时解析）                             │
│  class Foo_UI { setup(); bind_events(); virtual on_*_evt(); }  │
└───────────────────────┬───────────────────────────────────────┘
                        │
                        ▼
┌───────────────────────────────────────────────────────────────┐
│  lvgl++ （header-only C++ 封装，直接调用 LVGL 9.2 C API）      │
│  35 个 Widget + 4 个布局器 + LStyle(88 setter) + 事件/信号     │
└───────────────────────────────────────────────────────────────┘

辅助工具链：scripts/assets（图片转 C 数组）、scripts/designer（自动化测试）、
           scripts/headless（无头交互测试）、verify_matrix（全组件矩阵验证）
```

数据流只有一条：**设计器改 `.lui` → luic 生成 `.hpp` → 应用编译**。设计器与运行时
共享同一套渲染逻辑（设计器直接构建 LVGL 对象树），因此**预览即运行效果**。

**对应文件：** `tools/lvgl-designer/`、`tools/luic/`、`easylvgl/`

---

## 3. 组件支持（43 个）

### 3.1 组件清单

`属性数` = 属性面板里该组件可见的属性条目数（通用样式 + 组件专有，合计 **3112** 条）。

| 组件 | 中文名 | 属性数 | 组件专有属性 |
|---|---|---:|---|
| `LButton` | 按钮 | 75 | text、disabled、theme |
| `LLabel` | 标签 | 74 | text、color、font_size、long_mode |
| `LImage` | 图片 | 77 | src、rotation、scale、scale_x、scale_y、pivot、offset、antialias、opacity |
| `LArc` | 圆弧 | 76 | range、value、rotation、mode、bg_angles、angles、change_rate、knob_offset |
| `LBar` | 进度条 | 70 | range、value、start_value、mode、orientation |
| `LSlider` | 滑块 | 69 | range、value、left_value、mode |
| `LSwitch` | 开关 | 67 | checked、enabled |
| `LCheckbox` | 复选框 | 74 | text、checked |
| `LRadioButton` | 单选按钮 | 74 | text、checked |
| `LButtonMatrix` | 按钮矩阵 | 75 | map、selected、one_checked |
| `LSpinbox` | 数字输入框 | 79 | range、value、step、digit_format、rollover、cursor_pos、digit_step_direction |
| `LLineEdit` | 单行输入 | 81 | text、placeholder、password_mode、password_show_time、max_length、accepted_chars、cursor_click_pos、text_selection、enabled |
| `LTextEdit` | 多行输入 | 81 | 同上 |
| `LComboBox` | 下拉框 | 79 | options、selected、direction、symbol、text、selected_highlight、enabled |
| `LRoller` | 滚轮选择 | 74 | options、selected、visible_row_count |
| `LKeyboard` | 虚拟键盘 | 75 | textarea、mode、popovers |
| `LLed` | 指示灯 | 67 | color、brightness、on |
| `LSpinner` | 加载动画 | 65 | color |
| `LLine` | 线条 | 69 | points、width、color、y_invert、dash_width、dash_gap、rounded |
| `LTable` | 表格 | 76 | row_count、column_count、column_widths、data |
| `LChart` | 图表 | 70 | type、point_count、div_line_count、update_mode、series |
| `LScale` | 刻度尺 | 80 | mode、range、total_tick_count、major_tick_every、rotation、angle_range、label_show、draw_ticks_on_top |
| `LCalendar` | 日历 | 73 | today_date、showed_date |
| `LList` | 列表 | 73 | items |
| `LSpan` | 富文本片段 | 75 | spans、mode、overflow、indent |
| `LMenu` | 菜单 | 75 | mode_header、mode_root_back_button、active_page |
| `LMenuPage` | 菜单页 | 73 | title |
| `LMenuCont` | 菜单容器 | 69 | — |
| `LMenuSection` | 菜单分区 | 69 | — |
| `LMsgBox` | 消息框 | 74 | title、text |
| `LGroupBox` | 分组框 | 74 | title、padding |
| `LWin` | 窗口 | 73 | title |
| `LTabView` | 标签页 | 75 | tabs、tab_bar_position、tab_bar_size |
| `LTileView` | 平铺视图 | 72 | — |
| `LTile` | 平铺块 | 60 | col、row、dir |
| `LImageButton` | 图片按钮 | 72 | state、src_released、src_pressed、src_disabled |
| `LAnimImage` | 动画图片 | 60 | duration、repeat_count |
| `LCanvas` | 画布 | 55 | — |
| `LLottie` | Lottie 动画 | 68 | src、loop、speed（模块未启用，见 §9） |
| `LVBoxLayout` | 垂直布局 | 73 | spacing、padding、align_cross |
| `LHBoxLayout` | 水平布局 | 73 | spacing、padding、align_cross |
| `LGridLayout` | 网格布局 | 75 | columns、rows、col_spacing、row_spacing、padding |
| `LStackedLayout` | 堆叠布局 | 74 | current_index、padding |

说明：
- **原生封装**（41 个）：直接对应 `lv_xxx_create` 及其 API
- **复合控件**（2 个）：`LRadioButton`（obj + 圆环 + 圆点 + 标签）、`LGroupBox`（obj + 标题标签），由设计器/封装层用原生对象搭建
- **占位**（1 个）：`LLottie`（`LV_USE_LOTTIE=0` 时退化为普通矩形，见 §9.1）

**对应文件：** `tools/lvgl-designer/src/model/ComponentSchema.hpp`、`tools/luic/component_registry.hpp`

### 3.2 通用样式属性（11 组 / 72 项）

所有组件共享；按组件能力额外提供「圆弧样式」「图片样式」条件组。

| 分组 | 项数 | 属性 |
|---|---:|---|
| 对象 | 11 | x、y、width、height、align、hidden、clickable、scrollable、scroll_dir、scrollbar_mode、ext_click_area |
| 背景 | 11 | bg_color、bg_opa、bg_grad_color、bg_grad_dir、bg_main_stop、bg_grad_stop、bg_image_src、bg_image_opa、bg_image_recolor、bg_image_recolor_opa、bg_image_tiled |
| 边框 | 5 | border_color、border_width、border_opa、border_side、border_post |
| 轮廓 | 4 | outline_color、outline_width、outline_opa、outline_pad |
| 阴影 | 6 | shadow_color、shadow_width、shadow_ofs_x、shadow_ofs_y、shadow_spread、shadow_opa |
| 内边距 | 6 | pad_top、pad_bottom、pad_left、pad_right、pad_row、pad_column |
| 外边距 | 4 | margin_top、margin_bottom、margin_left、margin_right |
| 尺寸约束 | 4 | min_width、max_width、min_height、max_height |
| 外观 | 5 | radius、clip_corner、opa、blend_mode、base_dir |
| 文本样式 | 7 | text_color、text_opa、text_font_size、text_letter_space、text_line_space、text_align、text_decor |
| 变换 | 9 | transform_rotation、transform_scale_x、transform_scale_y、transform_pivot_x、transform_pivot_y、translate_x、translate_y、transform_skew_x、transform_skew_y |

三个层面的属性数量：

| 层 | 数量 | 说明 |
|---|---:|---|
| 设计器面板可见属性 | **3112** | 43 组件 × 各自可见属性条目 |
| luic 样式属性表 | **83** | 含 `line_*` / `arc_*` / `image_*` 条件项（`component_registry.hpp::style_props()`） |
| `LStyle` setter | **88** | 覆盖上述 83 项 + `font_by_size` 等辅助 |

### 3.3 属性类型与编辑控件

| 类型 | 面板控件 | 例子 |
|---|---|---|
| `int`（数值） | 单行数字输入 | x、width、pad_top、radius、opa |
| `int`（**枚举**） | **下拉框**（中文名 + LVGL 常量 tooltip） | align、scroll_dir、mode、state、dir、long_mode |
| `hex` | 色块（点击调色板）+ HEX 输入 | bg_color、text_color、border_color |
| `bool` | 复选框 | hidden、clickable、checked、password_mode |
| `string`（单行） | 单行文本，图片类带资源名补全 | text、placeholder、symbol、src |
| `string`（多行数据） | 多行文本框 | options、map、data、items、tabs、series、spans、points、column_widths |
| `int_pair` / `int_triple` | 逗号分隔输入，如 `0, 100` | range、pivot、offset、angles、digit_format、today_date |
| **对象引用** | **下拉框列出文档中的候选对象** | keyboard.textarea、menu.active_page |
| **字符串枚举** | 下拉框 | button.theme（默认/主色/危险/成功/警告） |

枚举属性共 **402 个（组件×属性）条目**，默认值全部落在选项中（脚本校验）。

### 3.4 事件

| 事件名 | 生成的方法 | 说明 |
|---|---|---|
| `clicked` | `on_<id>_clicked(lv_event_t*)` | 点击 |
| `pressed` | `on_<id>_pressed(lv_event_t*)` | 按下 |
| `released` | `on_<id>_released(lv_event_t*)` | 松开 |
| `value_changed` | `on_<id>_value_changed(lv_event_t*)` | 值变化 |
| `ready` | `on_<id>_ready(lv_event_t*)` | LVGL READY（布局完成） |

生成的是**虚函数**，用户在子类里 `override` 即可，无需注册回调、无需字符串查找。

### 3.5 布局能力

| 方式 | 能力 |
|---|---|
| 专用布局组件 | `LVBoxLayout`、`LHBoxLayout`（flex，spacing/padding/align_cross）、`LGridLayout`（行列/间距/padding）、`LStackedLayout`（单页显示 + 设计器内置翻页导航） |
| 工具栏 | 选中多个组件后一键「垂直布局/水平布局/网格布局/堆叠布局」，以及「解除布局」 |
| 普通容器 | 支持 `pad_row/pad_column`、`grid_col/grid_row`（子项）等落到底层布局 |

### 3.6 字体与主题

- **字体**：只编入 7 档 Montserrat（12/14/16/18/20/24/28），面板以下拉框呈现；其它字号按最近档位渲染
- **主题**：设计器画布与运行时共用同一主题；`LButton.theme` 提供 primary/danger/success/warning 四种外观
- **颜色**：设计器默认深色 UI + 画布浅色底（#F5F5F5），新建控件的默认前景色按可见性选择（如标签默认黑字、线条默认黑色）

---

## 4. `.lui` 格式

### 4.1 根对象

| 字段 | 类型 | 必需 | 说明 |
|---|---|---|---|
| `version` | string | 是 | 格式版本，当前 `"1.0"` |
| `name` | string | 是 | 名称，决定生成类名（`MainScreen` → `MainScreen_UI`） |
| `size` | `[int, int]` | 否 | 画布尺寸，默认 `[800, 480]`；生成 `kScreenWidth/kScreenHeight` |
| `resources` | object | 否 | 图片资源表：别名 → 路径（相对 `.lui` 所在目录），见 §4.5 |
| `children` | array | 是 | 顶层组件列表 |

### 4.2 组件对象

| 字段 | 类型 | 必需 | 说明 |
|---|---|---|---|
| `type` | string | 是 | 组件类型，如 `"LButton"`（与 C++ 封装类同名） |
| `id` | string | 是 | 唯一标识，决定成员名 `m_<id>` 与事件函数名 |
| `props` | object | 否 | 属性键值对（见 §4.3） |
| `events` | array | 否 | 事件名列表，如 `["clicked"]` |
| `children` | array | 否 | 子组件（仅容器类组件） |

位置与尺寸用 `pos` / `size` 两个数组属性表达（相对父对象）：

```json
"props": { "pos": [20, 16], "size": [100, 30] }
```

### 4.3 属性类型

| 类型 | JSON 写法 | 生成代码 |
|---|---|---|
| `int` | `75` | `set_value(75)` |
| `bool` | `true` | `set_checked(true)` |
| `hex` | `"0xFF5722"` | `set_color(0xFF5722)` |
| `string` | `"确定"` | `set_text("确定")` |
| `int_pair` | `[0, 100]` | `set_range(0, 100)` |
| `int_triple` | `[2026, 1, 1]` | `set_today_date(2026, 1, 1)` |

### 4.4 事件

```json
{ "type": "LButton", "id": "btn_ok", "events": ["clicked"] }
```

### 4.5 图片资源（qrc 式编译期嵌入）

```json
{
  "name": "EmbeddedImage",
  "size": [300, 160],
  "resources": { "caret": "assets/caret_down.png" },
  "children": [
    { "type": "LImage", "id": "logo",
      "props": { "pos": [40, 60], "size": [80, 60], "src": "#caret" } }
  ]
}
```

| 来源写法 | 含义 | 运行时依赖 |
|---|---|---|
| `"#别名"` | **编译期嵌入**的 `lv_image_dsc_t` 常量 | 无（不读文件、不需要 `lv_fs`） |
| `"A:path/logo.png"` | lv_fs 文件系统路径 | 需要文件系统与文件 |
| `LV_SYMBOL_*`（如 `"\\uF00C"`） | 字体字形 | 无 |

三步落地：

```bash
# 1) 转成 C 数组（改图后执行一次）
pip3 install pypng lz4
python3 scripts/assets/convert_assets.py path/to/screen.lui   # 默认 RGB565A8
#    可选：--cf ARGB8888 / --cf RGB565 / --compress RLE / --compress LZ4

# 2) luic 生成时指定资源头（默认 assets/lvgl_assets.h）
luic screen.lui -o screen_ui.hpp --assets-include "assets/lvgl_assets.h"

# 3) 把 assets/*.c 加入编译目标（CMake 对 .c 默认按 C 编译）
```

生成结果：

```cpp
#include "assets/lvgl_assets.h"
...
m_logo->set_src(&caret);          // 指向固件中的常量数组
```

颜色格式取舍：

| `--cf` | 每像素 | 与设计器预览 | 适用 |
|---|---|---|---|
| `RGB565A8`（默认） | 3 字节 | 有 16 位量化色差 | 嵌入式屏（多数为 RGB565） |
| `ARGB8888` | 4 字节 | **逐像素一致** | 32 位色屏、需要精确预览 |
| `RGB565` | 2 字节 | 有量化色差、无透明 | 不透明图片、极致省空间 |

**对应文件：** `docs/design/json-ui-format.md`、`scripts/assets/convert_assets.py`、`examples/embedded_image/`

---

## 5. luic 编译器

### 5.1 命令与选项

```
luic <输入.lui> [-o 输出.hpp] [--assets-include <路径>] [--verbose] [--help]
```

| 选项 | 说明 |
|---|---|
| `-o <文件>` | 输出头文件；默认 `<输入>_ui.hpp` |
| `--assets-include <路径>` | 嵌入图片资源头文件的 include 路径，默认 `assets/lvgl_assets.h` |
| `--verbose` | 打印解析/生成细节 |
| `--help` | 帮助 |

### 5.2 校验规则

| 规则 | 报错示例 |
|---|---|
| 必须有 `name` | `document 'name' is required` |
| 必须有子组件 | `document has no children` |
| 组件类型必须已注册 | `unknown component type: LFoo` |
| 组件必须有 `id` | `LButton: 'id' is required` |
| 属性必须属于该组件 | `LButton.btn.title: unknown property` |
| 属性类型必须匹配 | `LBar.value: expected int` |
| 主题值必须合法 | `invalid theme value` |
| **资源引用必须已声明** | `logo: 图片资源 '#caret' 未在 resources 中声明` |

校验失败时**不生成任何文件**（一次列出全部错误）。

### 5.3 生成代码结构

```cpp
// Auto-generated by luic from MainScreen.lui
// DO NOT EDIT — changes will be overwritten
#pragma once
#include "easylvgl/lvgl_widgets.hpp"

namespace lvgl {

class MainScreen_UI {
protected:
    LButton* m_btn_ok = nullptr;
    LLabel*  m_title  = nullptr;

public:
    static constexpr int kScreenWidth = 400;
    static constexpr int kScreenHeight = 200;

    void setup(lv_obj_t* parent = nullptr);      // 构建整棵树
    void bind_events();                          // 绑定 events 里声明的事件

    virtual void on_btn_ok_clicked(lv_event_t* e) {}   // 事件槽（可 override）
    virtual ~MainScreen_UI();                    // 逆序 delete，删除顺序安全
};

} // namespace lvgl
```

特性：

- **零运行时开销**：生成的是普通 C++ 直接调用，没有解析器/字符串查找/绑定引擎
- **样式块**：样式属性生成 `LStyle st; st.set_xxx(...); st.apply_to(obj);`，与组件属性分开
- **交叉引用**：`keyboard.textarea`、`menu.active_page` 在整棵树建完后解析（`m_kb->set_textarea(...)`、`menu->set_page(...)`）
- **嵌入资源**：只有真正用到 `"#别名"` 时才 `#include` 资源头
- **空值保护**：空的 `src` 不生成 `set_src("")`（该调用会让 LVGL 段错误）

### 5.4 属性 → 方法映射

| 属性类别 | 生成形式 |
|---|---|
| 组件专有属性 | `<Wrapper>` 的链式 setter，如 `set_value(50)`、`set_text("OK")` |
| 通用样式属性 | `LStyle` 块（`set_bg_color`、`set_radius`…） |
| `pos` / `size` / `theme` | 构造时连续调用（`set_pos`、`set_size`、`primary()`） |
| 图片 `src` | `&别名`（嵌入）或 `"A:..."` 字符串（文件） |

**对应文件：** `tools/luic/{main,parser,validator,codegen,ir,component_registry}.hpp`

---

## 6. 可视化设计器

### 6.1 界面与操作

| 区域 | 能力 |
|---|---|
| **组件面板** | 43 个组件按「布局 / 按钮 / 容器 / 输入 / 显示 / 视图·数据」分组；拖到画布即创建 |
| **画布** | 拖拽移动、8 个手柄缩放、根（画布）四角手柄改画布尺寸；点击选中、`Ctrl+点击` 多选；右键菜单（删除/复制/粘贴/置于顶层/置于底层） |
| **组件树** | 层级浏览、选中联动 |
| **属性面板** | 按分组展示属性（见 §6.2） |
| **状态栏** | 当前选中组件、位置、尺寸、画布尺寸 |

菜单 / 工具栏：

| 菜单 | 项目 |
|---|---|
| 文件 | 新建、打开、保存、另存为、关闭、退出 |
| 编辑 | 撤销、重做、剪切、复制、粘贴、删除、全选、**添加图片资源…** |
| 视图 | 组件面板 / 组件树 / 属性面板 显隐、状态栏、画布尺寸 |
| 构建 | **编译 .lui（F5）** |
| 工具栏 | 新建、打开、保存、编译、剪切、复制、粘贴、删除、撤销、重做、垂直布局、水平布局、网格布局、堆叠布局、解除布局 |

### 6.2 属性面板特性

| 特性 | 说明 |
|---|---|
| **显示真实生效值** | 未显式设置的样式属性显示 LVGL 里**实际生效**的值（`lv_obj_get_style_prop`），而不是 schema 里的写死默认值 |
| **枚举下拉** | 402 个枚举条目全部下拉选择，显示中文名，tooltip 给出 LVGL 常量 |
| **对象引用下拉** | `keyboard.textarea`、`menu.active_page` 列出文档中的候选对象，避免手输 ID |
| **不显示无效属性** | 面板按组件能力过滤（见 §9），不显示"改了也没反应"的行 |
| **实时刷新** | 重建（新建/撤销/重做）后只更新数值、不重建控件，正在编辑的字段不被打断 |
| **撤销集成** | 每次修改都是一个可撤销命令（含位置/尺寸/样式） |
| **颜色行** | 色块 + HEX 双输入，色块点击打开调色板 |
| **多行数据** | `options`/`map`/`data`/`items`/`tabs`/`series`/`spans`/`points` 用多行文本框，按行编辑 |

### 6.3 资源管理

- `编辑 → 添加图片资源…`：选图后自动复制到 `<lui 所在目录>/assets/`，并在 `resources` 里登记别名（别名取自文件名，非法字符转 `_`）
- 图片类属性（`src`、`src_released/pressed/disabled`、`bg_image_src`）输入框对已登记资源做 `#别名` 补全
- 画布预览按 `#别名` → 绝对路径加载，与运行时同一张图

### 6.4 编译

`构建 → 编译 .lui`（F5）先保存，再调用 `bin/luic` 生成 `.hpp`，错误直接弹出。

**对应文件：** `tools/lvgl-designer/src/{app,canvas,panels,model,commands}/`

---

## 7. lvgl++ 封装层

| 层 | 内容 | 规模 |
|---|---|---|
| 组件 | 35 个 Widget 封装（`easylvgl/widgets/`） | 35 个头文件 |
| 布局 | `LVBoxLayout`/`LHBoxLayout`/`LGridLayout`/`LStackedLayout` + `layout.hpp` | 5 个 |
| 样式 | `LStyle` 链式构建器 | **88 个 setter** |
| 基础 | `LWidget`/CRTP 基类、事件系统、`SignalHandler`（优雅退出） | `easylvgl/core/` |

设计要点：

- **Header-only**，零链接依赖
- **RAII**：析构自动 `lv_obj_delete`
- **链式 API**：`btn.set_text("OK").set_size(120, 50).primary()`
- **不复制 LVGL 的能力**：所有 setter 直接调用对应 `lv_*` API

---

## 8. 工具链与验证

| 工具 | 路径 | 用途 |
|---|---|---|
| 资源转换 | `scripts/assets/convert_assets.py` | PNG/JPG → `lv_image_dsc_t`（qrc 式嵌入） |
| 设计器自动化 | `scripts/designer/dtest.sh` | WSLg 下用 Win32 注入鼠标/键盘，驱动设计器做端到端测试 |
| 画布定位 | `scripts/designer/locate_canvas.py` | 从截图精确定位画布/表格/控件区/选中框 |
| 组件矩阵验证 | `scripts/designer/verify_matrix.py` | 43 组件 × 全属性 → luic 生成 + 编译 |
| 无头交互测试 | `scripts/headless/run.sh` | 不启动 GUI，直接驱动渲染器做点击/拖动/缩放回归 |

当前验证数据：

| 验证项 | 结果 |
|---|---|
| luic 单元/回归测试 | **14/14 通过**（5 个正例 + 9 个反例：类型错误、未知组件、非法主题等） |
| 组件矩阵（生成+编译） | **42/43**（`LLottie` 因 `LV_USE_LOTTIE=0` 跳过） |
| 属性功能探测 | **3112 条**：画面或状态发生变化 2944 条，崩溃/超时 **0** |
| 无头交互测试 | **2/2 通过** |
| C++ 封装单测 | 32 项断言通过 |
| 预览/运行时一致性 | 嵌入图片（ARGB8888）时设计器预览与运行时**逐像素一致**；`widgets_demo` 主题修正后 RMSE=0 |

---

## 9. 已知限制（明确不支持 / 受限）

### 9.1 组件级

| 组件 | 限制 | 原因 |
|---|---|---|
| `LLottie` | 所有组件属性无效，渲染为普通矩形 | 编译开关 `LV_USE_LOTTIE=0`（面板保留，暂不实现） |
| `LCanvas` | 只能当普通矩形容器 | 面板没有画布缓冲区与绘制接口；要做就得模拟 Qt 绘图 API，违反原则 2 |
| `LAnimImage` | 无图片序列入口，画面为空 | `lv_animimg_set_src` 需要图片描述符数组，属性模型暂未提供（可基于 §4.5 扩展到多图） |
| `LRoller` | `height` 不生效 | 高度由可见行数与字体决定（LVGL 行为） |
| `LImageButton` | 有图片时 `height` 由图片决定 | `refr_image()` 用图片高度覆盖（只保留用户设置的宽度） |
| `LScale` | `rotation`、`draw_ticks_on_top` 只在**圆盘模式**可见 | LVGL 语义 |
| `LTile` | `dir` 只影响滑动方向，画面无变化；`x/y/width/height/align` 不可用 | 位置由 tileview 的 `col/row` 决定 |
| `LMenuPage` | `title` 只在它成为当前页时显示 | LVGL 语义 |

### 9.2 属性级

| 情况 | 处理 |
|---|---|
| 无内容对象（空 image / 空 canvas / 空 animimg / tile / spangroup）的 `transform_*` 与非 0 `blend_mode` | **跳过不应用**——LVGL 9.2 软件渲染在该组合下死循环，会卡死设计器 |
| 文本样式组 | `LChart` 不绘制任何文字，整组隐藏；`LCalendar`/`LRoller` 的 `text_align` 隐藏 |
| 标签 `text_color`/`text_font_size` | 隐藏，统一用 `color`/`font_size`（避免两行重复且其中一行永远被覆盖） |
| 正方形控件（`LArc`/`LLed`/`LSpinner`）的 `height` | 隐藏，宽高始终一致 |
| 布局的 `pad_row`/`pad_column` | 隐藏，由 `spacing`/`row_spacing`/`col_spacing` 控制 |
| 菜单容器/分区、图块的坐标与 `align` | 隐藏，由父级布局决定 |
| 通用 `layout` 属性 | 隐藏（只切 flex 不设方向，实测无效果；请用布局组件） |
| 阴影偏移 | 需要 `shadow_width > 0` 才可见（LVGL 语义） |
| `clip_corner`/`border_post` | 需要子对象超出边界才可见（LVGL 语义） |
| `ext_click_area`、`accepted_chars`、`password_show_time`、`change_rate`、`update_mode`、`digit_step_direction` | 不影响画面，属于交互/行为属性（已用命中测试等单独验证） |

### 9.3 工程级（见第二部分）

当前**没有**工程/组件复用概念：一个 `.lui` = 一个屏幕，不支持 include/引用/导出参数。方案见 §11–§20。

---

## 10. 与 LVGL 原生能力的边界

| LVGL 能力 | 我们的呈现 |
|---|---|
| 43 个原生控件 | 43 个同名 `L*` 封装 + 设计器组件 |
| 样式系统（含状态/部件） | `LStyle`（88 setter）+ 面板 72 项通用属性 |
| 布局（flex/grid） | 4 个布局组件 + 工具栏布局操作 |
| 事件系统 | `on_<id>_<event>` 虚函数 |
| 图片（C 数组 / lv_fs / 符号） | 三种来源都支持（§4.5） |
| 动画 | 只暴露 LVGL 自带的（spinner/animimg/lottie 占位），不做自绘动画 |
| 主题 | 直接使用 LVGL 主题；`theme` 属性是薄封装 |

**明确不做**：响应式数据绑定、QML 式运行时引擎、moc/反射、自绘特效、Qt 布局系统模拟。

---

# 第二部分　组件化与工程管理

> **实施状态（2026-10-18）**
>
> | 阶段 | 状态 | 说明 |
> |---|---|---|
> | P1 组件化地基 | ✅ 已实现 | `kind` / `exports` / `@Name` / 组件透明根容器 / 导出 setter；单文件模式引用 `@组件` 会报错并提示用 `--project` |
> | P2 工程清单 | ✅ 已实现 | `lvglpp.project.json` + `luic --project`（依赖图、拓扑排序、循环引用报错、`--emit-cmake`） |
> | P3 设计器工程化 | 🚧 部分实现 | 已支持：打开工程（`lvglpp.project.json`，也可命令行传清单）、组件面板"项目组件"分组、实例拖入与递归预览、实例参数编辑（组件参数组）、组件文档独立预览、"设为导出"行内按钮。未做：新建组件/屏幕向导、工程树视图、工程级 CMake 构建配置 |
> | P4 组件间事件 | ⬜ 未开始 | — |
>
> **实现时对原方案的调整**
> - 组件内部子控件用**相对组件根的坐标**（与屏幕一致），不使用 flex 自动排布
> - luic 生成文件**扁平输出**到 `ui_generated/`（要求文件名不重复），不镜像源码目录
> - 导出路径规范为 `<id>.<属性>`（如 `title.text`），同时兼容 `m_<id>.<属性>`
> - 实例内部视为黑盒：点击只选中整个实例，内部控件不可单独编辑
> - 导出可指向组件属性，也可指向样式属性（`bar.bg_color`），后者通过 `LStyle` 应用

## 11. 背景与缺口

§1–§10 的工具链已覆盖**单文件**的完整流程（设计 → 生成 → 编译 → 运行），但**工程级**存在三个缺口：

| 缺口 | 现状 | 证据 |
|---|---|---|
| 没有"工程"概念 | 一次只处理一个 `.lui` | `tools/luic/main.cpp` 只接受单输入 |
| 没有自定义组件 | 同样的卡片/按钮组只能复制粘贴 | `.lui` 无 include/import/引用字段 |
| 文件 = 屏幕 | 生成类带 `kScreenWidth/Height`、`setup()` 默认挂 `lv_scr_act()`、设计器直接建在 `lv_scr_act()` 上 | `LvglRenderer::buildTree` |

**根因**：一个文件不是一个**部件**，而是一个**应用**。

## 12. 核心概念

| 概念 | 定义 | 类比 |
|---|---|---|
| **部件** Component | 可复用 UI，无屏幕语义（不带尺寸、不碰 `lv_scr_act()`） | QML `.qml` / QWidget 子类 |
| **屏幕** Screen | 特殊部件：带尺寸、决定主题档位、是运行时入口 | QML `Window` |
| **实例** Instance | 部件在父文件里的一次使用 | `MyButton { text: "确定" }` |
| **导出参数** Export | 部件对外暴露的可调项，映射到内部元素的属性 | QML `property` |
| **工程** Project | 一组 `.lui` + 入口屏幕 + 构建信息 | `.pro` / `.pri` |

原则：**文件 = 部件，屏幕是特例**（已确认）。

## 13. `.lui` 格式扩展

### 13.1 根节点 `kind`

```json
{ "name": "MainScreen", "kind": "screen", "size": [400, 320], "children": [...] }
{ "name": "MyCard",     "kind": "component", "children": [...] }
```

- 省略时默认 `"screen"` —— 现有示例无需修改
- `kind: "screen"` 才允许 `size`，才生成 `kScreenWidth/kScreenHeight`
- `kind: "component"` 出现 `size` 报错

### 13.2 `exports`

```json
{
  "name": "MyCard",
  "kind": "component",
  "exports": { "caption": "m_title.text", "accent": "m_bar.bg_color" },
  "children": [
    { "id": "m_title", "type": "LLabel", "props": { "text": "默认标题" } },
    { "id": "m_bar",   "type": "LBar",   "props": { "value": 50 } }
  ]
}
```

每个导出项生成一个普通 C++ setter（编译期检查，无 moc/反射/运行时查找）。

### 13.3 实例语法 `"type": "@Name"`

```json
{ "id": "card1", "type": "@MyCard",
  "props": { "pos": [10, 10], "size": [180, 120], "caption": "第一张", "accent": "0xFF5722" } }
```

- `pos`/`size` 作用于实例根容器
- 其余键按 `exports` 解析；未声明的键**报错**（防止拼写错误静默失效）

## 14. 代码生成规范

### 14.1 组件类必须有独立根容器

现状是子节点直接挂 `parent`，父级 flex/grid 会打乱组件内部布局，实例也无法整体移动。改为：

```cpp
class MyCard_UI {
protected:
    LObj*   m_root    = nullptr;   // 组件自己的根容器
    LLabel* m_m_title = nullptr;
    LBar*   m_m_bar   = nullptr;
public:
    void setup(lv_obj_t* parent = nullptr) {
        m_root = new LObj(parent);
        m_root->set_size(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        m_root->set_layout_vbox();          // 根独占布局
        m_m_title = new LLabel(m_root->native());
        ...
    }
    MyCard_UI& set_caption(const char* v) { m_m_title->set_text(v); return *this; }
    MyCard_UI& set_accent(uint32_t v)     { m_m_bar->set_bg_color(v); return *this; }
    lv_obj_t* root() const { return m_root->native(); }
    virtual ~MyCard_UI() { delete m_root; }   // 删根即删整棵子树
};
```

### 14.2 屏幕类保持现状

`kind: "screen"` 的生成结果与今天一致，保证向后兼容。

### 14.3 实例化与参数应用

```cpp
m_card1 = new MyCard_UI();
m_card1->setup(root);
m_card1->set_pos(10, 10)->set_size(180, 120);
m_card1->set_caption("第一张");
m_card1->set_accent(0xFF5722);
```

## 15. 工程管理与 luic 工程模式

### 15.1 工程清单（`.pri` 式薄清单）

```json
{
  "version": "1.0",
  "name": "MyProject",
  "entry": "MainScreen",
  "includeDirs": ["ui/components", "ui/screens"],
  "files": ["ui/components/MyCard.lui", "ui/screens/MainScreen.lui"]
}
```

只记录"包含哪些文件、入口是谁"；谁是组件由各文件的 `kind` 决定，不重复信息。

### 15.2 luic 工程模式

```
luic --project lvglpp.project.json
```

流程：加载全部 `.lui` → 建立 `@Name` 依赖图 → 拓扑排序生成 → 循环检测（A 嵌 B、B 嵌 A 报错并给出环路径）→ 校验 `kind/size`、`@Name` 存在性、`exports` 路径有效性 → （可选）`--emit-cmake` 输出源文件列表。

### 15.3 CMake 集成

```cmake
include(${CMAKE_SOURCE_DIR}/ui_generated/lvglpp_project.cmake)
add_executable(my_app main.cpp ${EASYLVGL_UI_SOURCES})
```

## 16. 设计器工程化

### 16.1 工程视图
打开工程（含清单的目录）；左侧"工程"视图按 `kind` 分「组件 / 屏幕」两组；同一时刻只编辑一个文件但知道整个工程。

### 16.2 新建组件 / 新建屏幕
输入名字（校验合法标识符、工程内不重名、不与内置组件重名）→ 选目录 → 创建 `.lui` 并加入清单。

### 16.3 组件面板新增"项目组件"分组
列出工程内所有 `kind: component`，拖入画布即插入 `type: "@MyCard"` 实例。

### 16.4 递归预览
实例预览 = 递归构建被引用文件的 children 到实例根容器下（不走代码生成）；需处理多实例独立建树、被引用文件变更后的重建。

### 16.5 选中实例时的属性面板
分两段：实例自身（位置/尺寸）、导出参数（按 `exports` 动态生成可编辑行）。

### 16.6 在属性上设置导出（作者方视角）

- 每个属性行提供「导出」入口（行内按钮或右键菜单），默认可改的对外名
- 已成为导出的属性行有明显标记，可改名、可取消
- 仅 `kind: "component"` 可用；对外名必须唯一且合法
- 内部 id / 属性重命名时**自动改写 `exports` 路径**；打开文件时校验路径有效性，失效则报错
- 本轮不做：导出子实例内部、导出的类型标注/分组/提示文案

### 16.7 必须守住的不变量

> **只认识一部分字段，但绝不丢弃不认识的字段。**

`.lui` 是 git 里的 JSON，用户可能手改；解析 → 序列化必须保留未知键，否则保存即损坏数据。

## 17. 兼容性与迁移

- 现有示例均为 `kind` 缺省 → 按 `screen` 处理，行为不变
- 设计器：工程视图为新增，单文件模式继续可用
- luic：不带 `--project` 时行为与今天一致
- 文档：`json-ui-format.md` 需补 `kind` / `exports` / `@Name`

## 18. 分阶段落地

| 阶段 | 内容 | 交付判据 |
|---|---|---|
| **P1 组件化地基** | `kind` + 组件根容器 + `@Name` 实例 + `exports` + 代码生成 | 手写一组件一屏幕，生成可编译运行的 C++，复用组件并设不同参数 |
| **P2 工程清单** | 清单 + `luic --project`（依赖图/拓扑/循环检测/校验）+ CMake | 多组件多屏幕工程一条命令编译，改组件后引用处全部生效 |
| **P3 设计器工程化** | 工程视图 + 新建组件/屏幕 + 项目组件面板 + 递归预览 + 属性上设置导出 + 实例参数编辑 | 全程图形化搭出 P2 的工程 |
| **P4（预留）** | 组件间事件/回调 | 不在本轮 |

每阶段结束跑：`tests/luic/run_tests.sh`、`scripts/headless/run.sh`、全组件矩阵。

## 19. 预留：组件间事件/回调

- 组件可声明 `"emits": ["clicked"]`
- 父文件实例上声明处理：`"on": { "clicked": "onCardClicked" }`
- 待定：回调签名与生命周期归属（父持有 vs 组件自持）

## 20. 待确认决策

| # | 议题 | 推荐 | 理由 |
|---|---|---|---|
| 1 | 组件根容器默认尺寸 | `LV_SIZE_CONTENT`，实例可覆盖 | 父级 flex/grid 里最自然 |
| 2 | 工程清单权威形式 | JSON 清单 + luic 生成 cmake 片段 | JSON 便于工具读写，CMake 是产物 |
| 3 | `exports` 写法 | `"caption": "m_title.text"` | 可读性最好 |
| 4 | `int_pair` 导出的 setter | `set_size(int w, int h)` 直接透传 | 直观 |
| 5 | 实例上未声明参数 | **报错** | 防止拼写错误静默失效 |
| 6 | 导出名默认建议 | 按属性名建议，可改 | 减少输入 |
| 7 | 重命名联动 | 自动改写 `exports` 路径 | 否则静默产生失效路径 |

---

## 附录 A：当前可用的最小示例（已实现）

```
examples/embedded_image/
├── screen.lui
├── main.cpp
└── assets/
    ├── caret_down.png        # 原图
    ├── caret.c               # 转换产物（lv_image_dsc_t）
    └── lvgl_assets.h         # 资源声明
```

`screen.lui`

```json
{
  "version": "1.0",
  "name": "EmbeddedImage",
  "size": [300, 160],
  "resources": { "caret": "assets/caret_down.png" },
  "children": [
    { "type": "LLabel", "id": "title",
      "props": { "pos": [20, 16], "size": [260, 24],
                 "text": "Embedded image (like qrc)", "color": "0x000000" } },
    { "type": "LImage", "id": "logo",
      "props": { "pos": [40, 60], "size": [80, 60], "src": "#caret", "scale": 512 } }
  ]
}
```

`main.cpp`

```cpp
#include "easylvgl/lvgl_widgets.hpp"
#include "easylvgl/core/signal_handler.hpp"
#include "screen_ui.hpp"
using namespace lvgl;

int main() {
    lv_init();
    lv_sdl_window_create(EmbeddedImage_UI::kScreenWidth, EmbeddedImage_UI::kScreenHeight);
    SignalHandler signal_handler;
    EmbeddedImage_UI ui;
    ui.setup();
    ui.bind_events();
    while (signal_handler.is_running()) { lv_timer_handler(); usleep(5000); }
}
```

构建（CMake）：见根 `CMakeLists.txt` 的 `embedded_image_demo` 目标与 `convert_assets` 目标。

## 附录 B：规划中的组件化示例（未实现）

```
MyProject/
├── lvglpp.project.json
├── main.cpp
└── ui/
    ├── components/MyCard.lui      # kind: component + exports
    └── screens/MainScreen.lui     # kind: screen + "@MyCard" 实例
```

```json
{
  "version": "1.0",
  "name": "MyCard",
  "kind": "component",
  "exports": { "caption": "m_title.text", "accent": "m_bar.bg_color" },
  "children": [
    { "id": "m_title", "type": "LLabel", "props": { "text": "默认标题", "pos": [8, 8] } },
    { "id": "m_bar",   "type": "LBar",   "props": { "value": 50, "pos": [8, 40], "size": [160, 12] } }
  ]
}
```

```json
{
  "version": "1.0",
  "name": "MainScreen",
  "kind": "screen",
  "size": [400, 320],
  "children": [
    { "id": "card1", "type": "@MyCard",
      "props": { "pos": [10, 10], "size": [180, 120], "caption": "第一张", "accent": "0xFF5722" } },
    { "id": "card2", "type": "@MyCard",
      "props": { "pos": [200, 10], "size": [180, 120], "caption": "第二张" } }
  ]
}
```

## 附录 C：与 QML / Qt 的取舍

**借鉴**：文件即组件类型、实例化语法、参数（`property`）模型。

**不借鉴**：

| QML 的做法 | 我们的做法 | 原因 |
|---|---|---|
| `Q_PROPERTY` / moc / 元对象反射 | 生成普通 C++ setter | 编译期检查，无运行时字符串查找，无引擎 |
| 运行时绑定引擎 / 响应式 | 静态代码生成 | LVGL 是纯 C 直接调用；静态生成零运行时开销 |
| `.ui`/`.qml` 与逻辑语言分离 | `.lui` 是纯声明式**数据** | 避免再发明一门语言 |
| 逐实例响应式绑定 | 默认不做，必要时 opt-in | QML 最重、最易失控的部分 |
