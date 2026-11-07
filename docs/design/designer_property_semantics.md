# 设计器属性语义核对（LVGL 9.6）

本文回答一个问题：**设计器矩阵里"改了但画面没变"的属性，是 LVGL 本来的行为，还是需要特定条件，还是设计器缺陷？**

核对方法：
1. 以 LVGL 9.6 的头文件/实现为唯一依据（`lvgl/include`、`lvgl/src`）。
2. 用 `tools/lvgl-designer/tests/matrix_main.cpp` 直接驱动真实 `LvglRenderer`，两两比较帧缓冲；
   满足条件后能产生差异的记为"条件生效"，仍无差异的归类为"设计如此"或"已知问题"。
3. 不以"换个值再试"作为结论，每条都给出 LVGL 侧理由。

---

## 一、已修复的真实缺陷

| 问题 | LVGL 依据 | 修复 |
|---|---|---|
| `align_main`（VBox/HBox）预览无效 | `lv_obj_set_flex_align` 的 main place 需显式设置 | 渲染器补 `align_main` |
| `knob_enabled`（Arc）预览无效 | 封装语义是切换 `LV_OBJ_FLAG_CLICKABLE` | 渲染器补该 flag 切换 |
| `start_value` / `left_value` 无效 | `lv_bar_set_start_value()` 在 `mode != LV_BAR_MODE_RANGE` 时**直接 return**；渲染器却在设置 mode 之前调用 | Bar/Slider 调整为**先 mode 后 start/left value** |
| `LLottie` 整体不可用 | 9.6 的 `lv_lottie.h` 没有 `set_loop/set_speed`；且必须由调用方 `lv_lottie_set_buffer` | 删除自创属性；封装管理缓冲；设计器用原生 API 创建 |
| `LTable.column_widths` 生成非法代码 | 列表属性需要逐项展开 | 生成逐列 `set_column_width(i, v)` |
| 顶层 `LTile` / `LMenuPage/Cont/Section` 生成非法代码 | 这些是父容器内部子元素 | luic 校验拒绝并给出父级要求 |
| `textarea` / `active_page` 引用不存在 id | 需要存在对应节点才能取 `native()` | luic 校验拒绝 |
| 新增 LArclabel / LBarcode / LQrcode | 9.6 原生 widget（`LV_USE_*` 均已启用） | 封装 + 注册表 + 设计器 + 代码生成 |

验证方式：`scripts/designer/codegen_all.py` 258/258 用例（每个组件每个属性 → luic → g++）全部通过。

---

## 二、设计如此：像素不会变（LVGL 语义决定）

| 属性 | LVGL 行为 | 说明 |
|---|---|---|
| `max_length`、`accepted_chars` | 输入策略，只影响键入 | 不在画面上产生差异 |
| `cursor_click_pos`、`text_selection` | 输入/选中行为 | 只有真正选中文本时才出现高亮 |
| `password_show_time` | 密码短暂明文的时间参数 | 需要输入 + 计时过程 |
| `update_mode`（QR/Barcode/Chart） | 立即/延迟刷新策略 | 最终像素相同 |
| `change_rate`（Arc） | 输入时旋转速率 | 静态帧无差异 |
| `knob_enabled`（Arc） | 封装映射到 `LV_OBJ_FLAG_CLICKABLE` | 行为属性，不改变外观（渲染器已应用） |
| `one_checked`（ButtonMatrix） | 多选/单选行为开关 | 需要交互或与 checkable 组合 |
| `textarea`（Keyboard） | 仅绑定输入目标 | 不绘制任何内容 |
| `duration` / `repeat_count`（AnimImage） | 动画时长/次数 | 静态帧相同 |
| `state`（ImageButton） | 只切换当前状态枚举 | 需要每个状态配不同图片才可见 |

---

## 三、需要特定条件（已逐条验证"条件满足后确实生效"）

| 属性 | 前置条件 | 验证结果 |
|---|---|---|
| `password_mode` / `password_show_time` | textarea 里必须有文本 | EFFECTIVE |
| `font_size`（Label） | 取值必须映射到不同字体（1/2 映射到同一默认字体） | 14 vs 28 EFFECTIVE |
| `long_mode`（Label） | 文本要长于固定宽度 | EFFECTIVE |
| `range`（Bar/Slider/Arc） | 需要同时有 `value` 才体现比例 | EFFECTIVE |
| `start_value` / `left_value` | **必须先设 mode=RANGE(2)**（已修渲染器顺序） | EFFECTIVE |
| `row_count` / `column_count`（Table） | 需要 `data` 支撑 | EFFECTIVE |
| `options`（Dropdown/Roller） | 需要 `selected` 或打开状态才改变可见文本 | EFFECTIVE |
| `symbol`（Dropdown） | 下拉箭头只在特定状态绘制；关闭态实测两符号无差异 | 待补充开态验证 |
| `mode` / `overflow`（Span） | 需要 spans 文本 + 受限宽度 | EFFECTIVE |
| `brightness`（LED） | 需要差距明显的取值（1 vs 2 几乎不可见） | 20 vs 220 EFFECTIVE |
| `digit_format`（Spinbox） | 类型是 `int_pair`（digit_count, separator），不是格式化字符串 | EFFECTIVE |
| `dash_width` / `dash_gap`（Line） | 需要 `width>0` 且 gap 非 0 | EFFECTIVE |
| `scale`（Image） | 需要有效资源（`src` 指向已声明资源） | EFFECTIVE |
| `rotation` / `angle_range` / `draw_ticks_on_top`（Scale） | **只在圆环模式**（`LV_SCALE_MODE_ROUND_INNER/OUTER`）生效；水平/垂直模式不使用这些参数 | EFFECTIVE（mode=16） |
| `size` / `pos` | 布局子项由父布局决定；自由对象才直接生效 | 视父容器而定 |

---

## 四、素材/结构限制（当前仓库没有对应素材，无法在预览里验证）

| 属性/组件 | 缺少的条件 |
|---|---|
| `src`（Lottie） | 需要真实 `.lottie` 资源 |
| `src_released` / `src_pressed` / `src_disabled` / `state`（ImageButton） | 需要每个状态一张不同图片 |
| `duration` / `repeat_count` / 动画（AnimImage） | 需要多帧图片序列 |
| `active_page` / `mode_header` / `title`（Menu 系列） | 需要完整 page 结构 |
| `size` / `pos`（Canvas、Tile、AnimImage） | Canvas 需要外部 buffer；Tile 由 TileView 定位；AnimImage 无帧时不可见 |

---

## 五、已知问题（未修，按要求暂缓）

| 项 | 现状 |
|---|---|
| LQrcode / LBarcode 内容不绘制 | 纯 LVGL 隔离测试复现：`update/render` 返回 OK、`is_render_valid=1`，但帧缓冲与空屏一致；`LV_USE_CANVAS/QRCODE/BARCODE/I1` 均已启用。需在 LVGL canvas/draw_buf 层定位，非设计器映射问题 |
| ButtonMatrix `selected` | `lv_buttonmatrix_set_selected_button()` 只记录索引；绘制侧要求对应按钮带 `LV_BUTTONMATRIX_CTRL_CHECKABLE`（源码 `button_is_checkable`），而设计器 map 字符串无法表达控制位 → 选中态不显示 |
| Dropdown `symbol` | 渲染器已调用 `lv_dropdown_set_symbol`；关闭态实测换符号无差异，需在展开态再验证 |

---

## 六、复现方式

```powershell
# 代码生成矩阵（全绿：258/258）
python scripts/designer/codegen_all.py

# 设计器预览矩阵
python scripts/designer/matrix_all.py --json out.json

# 单项条件验证（示例：bar start_value 需要 mode=RANGE）
tools/lvgl-designer/build-win/designer_matrix.exe <a.lui> w
tools/lvgl-designer/build-win/designer_matrix.exe <b.lui> w
```

> 结论：矩阵里剩余的"无可见变化"绝大多数是 **LVGL 设计如此** 或 **需要前置条件**；
> 真正由设计器造成的已修（align_main、knob_enabled、start/left value 顺序）；
> 剩下的是素材限制与两个已知问题（QR/Barcode 绘制、ButtonMatrix checkable 位）。
