# EasyLVGL Windows 便携包

整个 `dist` 目录可以直接拷贝到另一台 Windows x64 电脑，不需要安装 Qt、MinGW、CMake、Python。

```text
dist/
├── EasyLVGL.exe               # 启动器，也是 Enigma Virtual Box 主程序
├── Start-Designer.cmd         # 调用 EasyLVGL.exe
├── Start-Command-Prompt.cmd   # 带工具链环境的命令行
├── README-PORTABLE.md
├── EasyLVGL/                  # 设计器 + SDK
│   ├── tools/lvgl-designer/build-win/
│   ├── sdk/include/{easylvgl,lvgl,autoconf.h}
│   ├── sdk/lib/{libeasylvgl.a,liblvgl_linux.a}
│   ├── src/lib/               # driver_backends.h 等后端头文件
│   ├── scripts/assets/        # 图片转换脚本 + LVGLImage.py
│   ├── bin/luic.exe
│   └── CMakeLists.txt
├── toolchain/
│   ├── mingw64/
│   ├── cmake/
│   ├── clangd/bin/            # clangd + clang-format
│   └── python/                # 含 pypng、lz4
└── examples/excavator_dashboard/
```

不包含 LVGL C 源码、tests、docs、build 目录。

## 启动

双击 `EasyLVGL.exe`（或 `Start-Designer.cmd`）：

- 有上次工程时恢复上次工程
- 首次启动/没有有效历史时打开 `examples\excavator_dashboard\lvglpp.lpro`

## 源码编辑器

- 行号、当前行、括号匹配
- C/C++ 语法高亮、自动缩进、`Ctrl+/` 注释
- `Ctrl+F` / `Ctrl+H` 当前文件查找替换
- `F12` / `Ctrl+点击` 跳转定义
- `Ctrl+Space` 或输入 `.` / `>` 触发 clangd 补全
- `Ctrl+滚轮` 调整代码字号；设计画布同样支持 `Ctrl+滚轮` 缩放
- clangd 已包含在 `toolchain/clangd`，无需安装

## 构建示例

在 `Start-Command-Prompt.cmd` 中：

```bat
cd examples\excavator_dashboard
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build -j 8
build\excavator_dashboard.exe
```

示例 CMake 会自动从 `../EasyLVGL` 找到 SDK。设计器里也可以直接打开
`examples\excavator_dashboard\lvglpp.lpro` 后点“构建运行”。

工程清单里的 `target` 决定可执行文件名，`buildDir` 决定编译输出目录，默认都是工程名和
`build_lvgl`。这两项都可以在「工程设置」里修改。

## 用 Enigma Virtual Box 打成单文件

必须使用 **64 位 Enigma Virtual Box**。`EasyLVGL.exe` 和设计器都是 x86-64，32 位 EVB 打出的
程序会在 `evb*.tmp` 中以 `0xc000001d` 崩溃。

主程序选择 `dist\EasyLVGL.exe`，把 `EasyLVGL`、`toolchain`、`examples`、两个启动脚本和本说明
一起加入虚拟文件（不要重复加入主程序本身）。

输出例如：

```text
dist\EasyLVGL-Portable.exe
```

如果版本有「子进程虚拟化 / child process virtualization」选项，请开启。

注意：设计器编译时会启动 `cmake.exe`、`g++.exe`、`python.exe` 及其子进程。如果 EVB 不支持
子进程虚拟化，构建会找不到工具链或源码；这种情况请改用文件夹分发，或使用自解压 exe 先释放
到磁盘再运行。

## 说明

- 目标平台：Windows x64（Qt 5.12.2 / MinGW 7.3 / CMake 3.31 / Python 3.14）
- 整个 `dist` 约 856 MB；`toolchain`、`examples`、`EasyLVGL` 的相对位置不要拆散
- 首次运行可能被 SmartScreen 或杀毒软件询问，放行即可
- 第三方许可证见 `EasyLVGL/LICENSE`、`EasyLVGL/lvgl/LICENSE`
