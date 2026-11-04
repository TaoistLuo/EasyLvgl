# Windows 构建指南（MinGW-w64）

> **适用版本：** EasyLVGL 2.0.0（LVGL 9.6）　**最后更新：** 2026-10-18

本项目在三个桌面平台上共用同一套源码与 CMake，只有“窗口/输入后端”按平台切换：

| 平台 | 窗口/输入后端 | 依赖 |
|------|----------------|------|
| Windows | LVGL 原生 **Win32** 后端（`CONFIG_LV_USE_WINDOWS`） | 无（只用系统库） |
| Linux / macOS | **SDL2**（`CONFIG_LV_USE_SDL`） | SDL2 |

平台判定集中在两处，业务代码不需要写平台分支：

- Kconfig：`configs/linux-sdl.defconfig` 选择 SDL2 后端，Windows 配置选择 Win32 后端
- `easylvgl/core/platform.hpp`：`easylvgl::init_desktop_display(w, h)` 按可用顺序选择后端（实现仍在 `src/lib/display_backends/`）

---

## 1. 环境要求

- **MinGW-w64**（`gcc` / `g++` / `mingw32-make`）
  - 已验证：Qt 5.12.2 自带的 `Tools\mingw730_64`（GCC 7.3.0）可完整构建。
  - 其它较新的 MinGW-w64（GCC 8+）同样可用。
- **CMake ≥ 3.12**（本项目使用 `mingw32-make` 生成器）。
- 无需安装 SDL2。

> `tools/luic` 已从 C++20 指定初始化器降级为 C++17，因此老版本 GCC 7.3 也能编译。

---

## 2. 快速构建

在仓库根目录执行：

```powershell
# 方式一：脚本（自动检测 g++ / cmake）
powershell -ExecutionPolicy Bypass -File scripts/win/build.ps1

# 方式二：手动
cmake -S . -B build-win -G "MinGW Makefiles" `
      -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
cmake --build build-win -j 8
```

常用参数：

```powershell
# 清理后重建
powershell -ExecutionPolicy Bypass -File scripts/win/build.ps1 -Clean

# 只构建某个目标
powershell -ExecutionPolicy Bypass -File scripts/win/build.ps1 -Target main -Target luic
```

> 如果 `cmake` 或 `g++` 不在 `PATH` 中，请先把对应目录加入 `PATH`，例如：
> ```powershell
> $env:Path = "E:\QT\QT5.12.2\Tools\mingw730_64\bin;" + $env:Path
> ```

---

## 3. 运行

所有可执行文件（以及 `luic.exe`）都输出到源码树下的 `bin\`：

```powershell
.\bin\main.exe            # lv_demo_widgets，Win32 窗口
.\bin\cpp_tests.exe       # C++ 封装单元测试
.\bin\layer1_demo.exe
.\bin\widgets_demo.exe
```

窗口尺寸可通过环境变量覆盖：`EASYLVGL_WINDOW_WIDTH` / `EASYLVGL_WINDOW_HEIGHT`。

### 可视化设计器（Qt）

`tools/lvgl-designer` 是一个 Qt Widgets 程序，它把 LVGL 渲染到自己的 framebuffer 后交给 Qt 绘制，
**并不直接使用 SDL**。设计器只链接构建出的 LVGL 静态库与 Qt，不再包含 `lvgl` 子目录或 SDL2 依赖。

```powershell
# 方式一：脚本（需要指定与 MinGW 匹配的 Qt）
powershell -ExecutionPolicy Bypass -File scripts/win/build-designer.ps1 `
    -QtDir "E:/QT/QT5.12.2/5.12.2/mingw73_64"

# 方式二：手动
$env:Path = "E:\QT\QT5.12.2\Tools\mingw730_64\bin;" + $env:Path
cmake -S tools/lvgl-designer -B tools/lvgl-designer/build-win -G "MinGW Makefiles" `
      -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ `
      -DCMAKE_PREFIX_PATH=E:/QT/QT5.12.2/5.12.2/mingw73_64
cmake --build tools/lvgl-designer/build-win -j 8
```

运行前把 Qt 的 `bin` 加入 `PATH`（或发行时用 `windeployqt`）：

```powershell
$env:Path = "E:\QT\QT5.12.2\5.12.2\mingw73_64\bin;" + $env:Path
.\tools\lvgl-designer\build-win\lvgl-designer.exe
```

> 设计器“新建工程”生成的 CMake / `main.cpp` 与宿主机平台一致；主程序不直接调用任何
> 窗口 API，统一由 `src/lib/driver_backends` 按可用后端顺序选择（Windows 为 `WIN32`）。
>
> 设计器里“构建工程”的生成器在 Windows 上默认使用 **MinGW Makefiles**（可在“设置”里修改），
> 并会自动把 Qt 安装目录下的 MinGW 工具链加入子进程 `PATH`。

---

## 4. 平台切换原理

构建配置由 Kconfig 生成：Linux 使用 `configs/linux-sdl.defconfig`，Windows 使用
`configs/windows-win32.defconfig`（CMake 按平台自动选择）。配置符号写入构建目录下的
`autoconf.h`：

| 符号 | 作用 |
|------|------|
| `CONFIG_LV_USE_SDL` | SDL2 窗口与输入 |
| `CONFIG_LV_USE_WINDOWS` | Win32 窗口与输入 |
| `CONFIG_LV_USE_EVDEV` | Linux evdev 输入 |

后端选择集中在 `src/lib/driver_backends.c`，各后端实现在 `src/lib/display_backends/`：

| 文件 | 后端 | 平台 |
|------|------|------|
| `sdl.c` | SDL2 | Linux / macOS |
| `win32.c` | LVGL 原生 Win32 | Windows |
| `fbdev.c` / `drm.c` / `x11.c` / `wayland.c` / `glfw3.c` | 其它 | Linux |

主程序只调用 `driver_backends_init_backend()`，不出现平台分支。
```

主程序与所有示例都只调用 `init_desktop_display(...)`，平台差异全部集中在后端实现里，Linux 上走的仍是原来的
`lv_sdl_window_create` + `lv_sdl_mouse_create`，行为不变。

---

## 5. 已处理的 Windows 兼容点

- **CMake**：`bin/` 输出目录、多配置生成器 per-config 路径、`luic` 用 `$<TARGET_FILE:luic>` 引用、`run_*` 目标用生成表达式、MSVC 用 `lib.exe` / GNU 用 `ar -M` 合并静库。
- **静态库合并** `cmake/merge_libs.cmake`：改用相对路径 + `WORKING_DIRECTORY`，避免 Windows 盘符让 `ar` 解析失败。
- **`luic`**：`mkdir`/`stat` 在 Windows 走 `_mkdir`/`_stat`；路径判定兼容盘符与反斜杠；去掉 C++20 指定初始化器。
- **Kconfig**：构建配置由 defconfig 生成；MinGW 默认按 Server 2003 裁剪头文件，CMake 补 `_WIN32_WINNT=0x0601`、`WINVER=0x0601`，并为旧 MinGW 补 `WM_DPICHANGED`。

---

## 6. 常见问题

**Q：为什么 Windows 不用 SDL？**
A：使用 LVGL 原生 Win32 后端可省去 SDL2 依赖与 DLL 分发；Linux/macOS 仍用 SDL2。

**Q：MinGW 报 `TOUCHINPUT` / `WM_DPICHANGED` 未定义？**
A：这是旧 MinGW 头文件裁剪导致，CMake 已通过 `-D_WIN32_WINNT=0x0601` 等宏处理；若手动编译请一并加上。

**Q：想用 MSVC？**
A：本项目同时保留了 MSVC 路径（`CMakeLists.txt` 的 `MSVC` 分支、`/utf-8` 等），但目前只验证了 MinGW-w64 构建与运行。
