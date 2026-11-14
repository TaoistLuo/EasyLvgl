# 挖掘机智能仪表盘（工程级示例）

一个完整的 EasyLVGL **工程化**示例，而不是单文件 demo：工程清单、多屏幕、可复用组件、
图片资源、业务逻辑与独立 CMake 构建一应俱全，也可以直接用设计器打开编辑。

- 分辨率：800 x 480，深色 HMI 风格
- 场景：履带式挖掘机（SY215C）的车载仪表
- 数据来源：程序内模拟 CAN 采集（500 ms 刷新）

## 目标平台

当前只面向桌面 Linux（SDL2 后端），不为 MCU 做裁剪；完整 TTF 字体与运行时渲染都按
“内存/存储充足”的前提实现。

## 界面

| 屏幕 | 内容 |
|------|------|
| `Dashboard`（入口） | 挖机 logo、发动机转速/负载双仪表、整机状态灯、燃油/水温/电压速览、进入查询与设置的大按钮 |
| `InfoCenter` | 左侧 8 个实时参数卡（转速、液压压力、水温、燃油、电压、工时、液压油温、负载率）；右侧“提示语 + 值”列表，值分别用 **状态徽章组件**（ON/OFF）、**图标 + 文本**、**数值** 三种形态 |
| `Settings` | 四个 switch、三个单选（工作模式）、复选框、三个数值输入（Spinbox）、语言下拉框（简体中文 / English / Русский）、操作员文本输入（点击弹出屏幕键盘） |

配置保存后 ECO 状态会立刻反馈到仪表盘的模式指示（`模式: 经济 / 标准`）。
操作员输入框被点击时会弹出屏幕键盘，按键盘 OK 后收起并提示待保存。

## 目录结构

```
excavator_dashboard/
├── lvglpp.lpro        # 工程清单：入口屏幕、文件列表、图片资源、输出目录
├── main.cpp                   # 业务逻辑：页面切换、模拟数据、交互处理
├── i18n.hpp                   # LVGL 原生 lv_translation 封装（注册/切换/控件绑定）
├── i18n/strings.xlsx          # 文案源（key / zh / en），Excel 是唯一来源
├── tools/xlsx2i18n.py         # 构建时把 xlsx 生成 i18n_generated.hpp（仅标准库）
├── CMakeLists.txt             # 独立工程构建（自动调用 luic + convert_assets + i18n）
├── ui/
│   ├── screens/               # 屏幕（kind: screen）
│   │   ├── Dashboard.lui
│   │   ├── InfoCenter.lui
│   │   └── Settings.lui
│   └── components/            # 可复用组件（kind: component + exports）
│       ├── GaugeCard.lui      # 弧线仪表卡：caption / reading / unit / value / range
│       ├── ParamRow.lui       # 参数行：name / value / unit / level / accent
│       └── StatusBadge.lui    # 状态徽章：text（背景色由逻辑层设置）
├── assets/
│   ├── excavator.png          # 挖机 logo（编译期嵌入，等价 qrc）
│   └── gear.png               # 设置图标
└── fonts/
    ├── NotoSansSC-Regular.ttf # 完整思源黑体（OFL，约 10.6 MB）
    └── OFL.txt
```

`ui_generated/`、`assets/*.c`、`assets/lvgl_assets.h` 都是构建产物，**不入库**，由 CMake 生成：

- `luic --project lvglpp.lpro --out-dir <build>/ui_generated`
- `scripts/assets/convert_assets.py lvglpp.lpro -o <build>/assets`

## 构建

依赖与主仓库一致；此外资源转换需要 Python 包 `pypng`、`lz4`：

```bash
pip install pypng lz4
```

在仓库根目录下构建（默认从当前目录上溯两级定位 easylvgl）：

```bash
cmake -S examples/excavator_dashboard -B examples/excavator_dashboard/build -G "MinGW Makefiles"
cmake --build examples/excavator_dashboard/build -j 8
./examples/excavator_dashboard/build/excavator_dashboard
```

外部工程可用 `-DEASYLVGL_ROOT=/path/to/lv_port_linux` 指定 SDK 根目录。

Windows 下窗口由 LVGL 原生 Win32 后端创建；Linux/macOS 使用 SDL2（与主工程一致）。
无头 CI 冒烟可用环境变量自动退出：

```bash
EASYLVGL_AUTOCLOSE_MS=3000 ./examples/excavator_dashboard/build/excavator_dashboard
```

## 用设计器编辑

用 Qt 设计器打开本目录（工程清单已被识别）即可：

- 左侧工程树按 `kind` 分为“屏幕 / 组件”
- 双击屏幕编辑；修改后执行一次上面的 `cmake --build` 会自动重新生成 UI
- 组件实例通过 `exports` 暴露参数，在右侧属性面板中按实例填写
  （例如 `GaugeCard` 的 `caption` / `value` / `range`）

## 多语言（LVGL 原生 lv_translation）

- 文案源：`i18n/strings.xlsx`（sheet `strings`，列 `key / zh / en / ru`），Excel 是唯一来源。
- 构建时：`tools/xlsx2i18n.py`（只依赖 Python 标准库）把 xlsx 生成到
  `build/i18n/i18n_generated.hpp`；`i18n.hpp` 再调用原生
  `lv_translation_add_static()` 注册语言包。改完 Excel 重新构建即可生效。
- 运行时切换走 `lv_translation_set_language()`：
  - 静态 `LLabel` 用 `lv_label_set_translation_tag()` 绑定，语言变化时由 LVGL 自动重译；
  - 按钮、单选、复选、输入框占位、组件导出参数没有 tag API，封装层 `bind_text()`
    监听 `LV_EVENT_TRANSLATION_LANGUAGE_CHANGED` 后重新调用 setter；
  - 实时状态词、反馈等动态文本用 `on_language_change()` 挂刷新回调。
- 启动语言：默认中文；`EASYLVGL_LANG=en` 时以英文启动。
- 新增语言：Excel 增加一列（当前封装固定 `zh`、`en` 两列与 `i18n::Lang` 映射），
  当前封装固定 `zh`、`en`、`ru` 三列与 `i18n::Lang` 映射；新增语言需同时扩展 `i18n::Lang`、
  设置页下拉索引，并让生成器接受新的语言列（目前要求前三列依次为 zh/en/ru）。
  生成器会校验列顺序、重复 key 和空翻译。
- 当前语言保存在内存；如需掉电保持，可在保存设置时写入配置文件。

## 中文字体

界面中文使用完整的思源黑体（Noto Sans SC，OFL），由官方可变字体在 `wght=400` 实例化得到，
保留全部约 3.1 万个字形（不做子集裁剪）。运行时由 LVGL Tiny TTF 读取
`fonts/NotoSansSC-Regular.ttf`，按 16 px（正文）与 22 px（标题）创建两个字号；新增中文文案无需重新生成字体。

- 字体文件：`fonts/NotoSansSC-Regular.ttf`（约 10.6 MB）
- 许可：`fonts/OFL.txt`
- 替换字体：配置时传 `-DEXCAVATOR_FONT_FILE=/path/to/your.ttf`（需要 TrueType `glyf` 轮廓，Tiny TTF 不支持 CFF/OTF）

## 代码组织

`main.cpp` 继承 luic 生成的三类 UI 类并覆写事件钩子：

```cpp
class DashboardScreen : public Dashboard_UI {
    void on_btn_info_clicked(lv_event_t*) override { /* 切页 */ }
};
```

生成的成员（`m_gauge_rpm`、`m_param_fuel`、`m_sw_eco` 等）在子类中可直接访问，
组件实例则通过生成的 `set_*`（`set_value` / `set_reading` / `set_level` …）更新。
