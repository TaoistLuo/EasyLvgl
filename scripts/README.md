# scripts —— 设计器自动化测试工具

在 **WSLg** 环境下驱动 Qt 设计器（`tools/lvgl-designer`）做交互与逻辑测试。

---

## 1. 为什么需要这套东西

WSLg 里 `DISPLAY=:0` 是 **XWayland**，而会话是 Wayland（`WAYLAND_DISPLAY=wayland-0`）。
实测结论：

| | 机制 | 结果 |
|---|---|---|
| 截图 | X 核心协议 `GetImage`（读像素） | ✅ 一直可用 |
| XTEST 注入 | `XTestFakeInput`（写输入） | ⚠️ XTEST 扩展存在、**指针真的会移动**，但应用收不到点击 |

```
XTEST 扩展 : True
指针注入   : (157,100) → (700,500)      ← 服务器层生效
焦点       : focus window = 0           ← 无窗口持有焦点
点击后状态栏 md5 不变                    ← 应用层收不到
```

所以输入**改走 Windows 侧**：WSL2 能直接调 Windows 程序，用 Win32
`SetCursorPos` + `mouse_event` / `keybd_event` 在 Windows 桌面层注入，
WSLg 窗口（本质是 Windows 窗口）就能正常收到。实测这样点击/拖拽/滚轮全部生效。

---

## 2. 目录结构

```
scripts/
├── win/
│   ├── win_input.ps1        Win32 注入：click/drag/move/wheel/key/info
│   └── win_find.ps1         只定位窗口（win_input.ps1 -Action info 已含此功能）
├── designer/
│   ├── dtest.sh             统一入口（坐标用 X 窗口坐标，自动换算）
│   └── locate_canvas.py     从截图里定位画布/表格矩形
└── headless/
    ├── test_root_interaction.cpp    无头驱动 LVGL indev，测根选中/手柄语义
    ├── test_sdlwidget_resize.cpp    Qt offscreen 测根尺寸拖拽的坐标与数学
    └── run.sh                       一键构建并运行上面两个
```

---

## 2.5 图片资源转换（assets/）

`assets/convert_assets.py` 把 `.lui` 里 `resources` 声明的图片转成 LVGL 的
`lv_image_dsc_t` 常量数组（等价 Qt 的 qrc），并生成 `lvgl_assets.h`：

```bash
pip3 install pypng lz4
python3 scripts/assets/convert_assets.py examples/embedded_image/screen.lui
python3 scripts/assets/convert_assets.py foo.lui --cf ARGB8888 --compress RLE
```

- 控件里写 `"src": "#别名"`，luic 生成 `set_src(&别名)`，运行时零文件依赖
- 默认 `RGB565A8`（省空间）；`ARGB8888` 与设计器预览逐像素一致
- 生成的 `.c` 要用 C 编译器编译（CMake 对 `.c` 默认就是 C）

## 3. 坐标标定（关键）

Linux 窗口是 **1400×900**，但 WSLg 把它缩放成 Windows 窗口客户区 **1180×797**
（`sx=0.8429, sy=0.8856`，两轴不同，不能只算一个比例）。

```
Windows屏幕坐标 = (客户区原点X + X窗口坐标X * sx,
                   客户区原点Y + X窗口坐标Y * sy)
```

`dtest.sh` 会自动用 `xwininfo`（取 X 窗口尺寸）+ `win_input.ps1 -Action info`
（取 Windows 客户区原点与尺寸）算好，缓存到 `/tmp/dtest.env`。

> ⚠️ 窗口被窗口管理器移动后要 `dtest.sh env` 重新标定。
> 若窗口有部分在屏幕外，`import` 只能抓到可见部分，但窗口左上角仍可见，
> 所以窗口相对坐标依然正确。

---

## 4. 怎么定位画布

LVGL 默认主题的屏幕底色是 **纯 `#F5F5F5`**，表格单元格是 **纯 `#FFFFFF`**，
设计器控件区底色是 `#B8B8B8`、面板是深色 —— 颜色可精确区分，不需要 OCR。

`locate_canvas.py` 按行/列统计匹配像素数，取"每行/列匹配 >= 50"的极值得到矩形，
能过滤掉属性面板里的小色块。

> 早期我用"找白色区域"定位，结果把表格当成了画布 —— 因为画布底色是 F5F5F5 而非白色。

---

## 5. 用法

```bash
# 先启动设计器（坐标标定依赖窗口存在）
./tools/lvgl-designer/build/lvgl-designer examples/widgets/table.lui &

# 定位窗口 + 标定缩放
scripts/designer/dtest.sh env

# 定位画布/表格（X 窗口坐标）
scripts/designer/dtest.sh locate
# canvas=430,270,498,393
# table=452,292,235,195
# empty=898,633          ← 画布内、表格外的一点，用来"点空白选中根"

# 状态栏快照（md5 用于判断有没有变化）
scripts/designer/dtest.sh status before

# 点击 / 拖拽 / 滚轮 / 缩放（坐标为 X 窗口坐标）
scripts/designer/dtest.sh click 898 633          # 点画布空白 → 选中根
scripts/designer/dtest.sh click 569 389          # 点表格 → 选中表格
scripts/designer/dtest.sh drag 877 625 947 685   # 拖 BR 手柄改根尺寸
scripts/designer/dtest.sh zoom 600 450 120       # Ctrl+滚轮放大一格（-120 缩小）
scripts/designer/dtest.sh wheel 600 450 120      # 普通滚轮（透传给 LVGL）
scripts/designer/dtest.sh drag 600 450 700 510 2 # 中键拖拽平移（末位 2=中键）
scripts/designer/dtest.sh shot /tmp/x.png        # 截整窗
scripts/designer/dtest.sh key 27                 # 按 Esc

# 无头逻辑测试（不占鼠标，可随时跑）
scripts/headless/run.sh
```

`click` 末位是按键：`0`=左 `1`=右 `2`=中。

---

## 6. 已验证的能力（用本工具实测）

| 项目 | 结果 |
|---|---|
| 点画布空白 → `Selected: root (LScreen)` | ✅ |
| 拖 BR 手柄 → 画布 400×320 → 500×395 | ✅ |
| Ctrl+滚轮放大/缩小 | ✅ |
| 中键拖拽平移 | ✅ |
| 点表格 → `Selected: w (LTable)`（未破坏） | ✅ |
| 无头：根选中 / 控件选中 / 根无 LVGL 手柄 / 控件手柄仍可用 | ✅ 4/4 |
| 无头：BR→500×400（不被显示分辨率钳制）/ 中心不触发 / TL→460×360 / MR 只改宽度 | ✅ 4/4 |

---

## 7. 注意事项

- **注入会抢走你的真实鼠标和焦点**（`SetForegroundWindow`），跑的时候别自己动鼠标。
- 画布小于视口时 `clampPan` 会强制居中，所以**此时中键拖拽看不出效果**（没有可平移空间）——
  想验证平移请先放大到画布超过视口。
- 状态栏没有 OCR，`status` 只输出 md5 判断"变没变"；要读文字就打开那张 PNG 看，
  或在此环境补 `tesseract`。
- `win_input.ps1` 每次运行会从 `scripts/win/` 复制到 Windows `%TEMP%` 再执行
  （WSL 路径经 `\\wsl$` 调用 PowerShell 不稳妥）。
- 脚本里 `mouse_event` 的签名用 `int` 而非 `uint`，否则负的滚轮 delta（下滚）会转换失败。
