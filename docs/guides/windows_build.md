# Windows 构建指南（MinGW-w64）

> **适用版本：** EasyLVGL 2.0.0（LVGL 9.2）　**最后更新：** 2026-10-18

本项目在三个桌面平台上共用同一套源码与 CMake，只有“窗口/输入后端”按平台切换：

| 平台 | 窗口/输入后端 | 依赖 |
|------|----------------|------|
| Windows | LVGL 原生 **Win32** 后端（`LV_USE_WINDOWS=1`） | 无（只用系统库） |
| Linux / macOS | **SDL2**（`LV_USE_SDL=1`） | SDL2、SDL2_image |

平台判定集中在两处，业务代码不需要写平台分支：

- `lv_conf.h`：根据 `_WIN32` 自动设置 `LV_USE_OS` / `LV_USE_SDL` / `LV_USE_WINDOWS`
- `easylvgl/core/platform.hpp`：`easylvgl::init_desktop_display(w, h)` 统一创建窗口与输入设备

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
**并不直接使用 SDL**，因此在 Windows 上不需要 SDL2。

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

> 设计器“新建工程”生成的 CMake / `main.cpp` 也已改为跨平台：Windows 链接 Win32 系统库，
> 其它平台链接 SDL2，并且主程序统一调用 `easylvgl::init_desktop_display()`。
>
> 设计器里“构建工程”的生成器在 Windows 上默认使用 **MinGW Makefiles**（可在“设置”里修改），
> 并会自动把 Qt 安装目录下的 MinGW 工具链加入子进程 `PATH`；若旧配置里存的是 `Unix Makefiles`
> 而系统没有 `make`，会自动回退，避免 `execution of make failed / Make command was: -f Makefile -j`。

---

## 4. 平台切换原理

`lv_conf.h` 中：

```c
#if defined(_WIN32)
    #define LV_USE_OS        LV_OS_WINDOWS   /* Win32 后端要求 */
    #define LV_USE_SDL       0
    #define LV_USE_WINDOWS   1
    #define LV_USE_EVDEV     0               /* evdev 仅 Linux 可用 */
#else
    #define LV_USE_OS        LV_OS_NONE
    #define LV_USE_SDL       1
    #define LV_USE_WINDOWS   0
    #define LV_USE_EVDEV     1
#endif
```

`easylvgl/core/platform.hpp` 中：

```cpp
inline lv_display_t *init_desktop_display(int w, int h) {
#if LV_USE_SDL
    lv_display_t *disp = lv_sdl_window_create(w, h);
    lv_sdl_mouse_create();
    return disp;
#elif LV_USE_WINDOWS
    lv_display_t *disp = lv_windows_create_display(L"EasyLVGL", w, h, 100, false, true);
    lv_windows_acquire_pointer_indev(disp);
    lv_windows_acquire_keypad_indev(disp);
    return disp;
#endif
}
```

主程序与所有示例都只调用 `init_desktop_display(...)`，因此在 Linux 上走的仍是原来的
`lv_sdl_window_create` + `lv_sdl_mouse_create`，行为不变。

---

## 5. 已处理的 Windows 兼容点

- **CMake**：`bin/` 输出目录、多配置生成器 per-config 路径、`luic` 用 `$<TARGET_FILE:luic>` 引用、`run_*` 目标用生成表达式、MSVC 用 `lib.exe` / GNU 用 `ar -M` 合并静库。
- **静态库合并** `cmake/merge_libs.cmake`：改用相对路径 + `WORKING_DIRECTORY`，避免 Windows 盘符让 `ar` 解析失败。
- **`luic`**：`mkdir`/`stat` 在 Windows 走 `_mkdir`/`_stat`；路径判定兼容盘符与反斜杠；去掉 C++20 指定初始化器。
- **`lv_conf.h`**：MinGW 默认按 Server 2003 裁剪头文件，CMake 补 `_WIN32_WINNT=0x0601`、`WINVER=0x0601`，并为旧 MinGW 补 `WM_DPICHANGED`。

---

## 6. 常见问题

**Q：为什么 Windows 不用 SDL？**
A：使用 LVGL 原生 Win32 后端可省去 SDL2 依赖与 DLL 分发；Linux/macOS 仍用 SDL2。

**Q：MinGW 报 `TOUCHINPUT` / `WM_DPICHANGED` 未定义？**
A：这是旧 MinGW 头文件裁剪导致，CMake 已通过 `-D_WIN32_WINNT=0x0601` 等宏处理；若手动编译请一并加上。

**Q：想用 MSVC？**
A：本项目同时保留了 MSVC 路径（`CMakeLists.txt` 的 `MSVC` 分支、`/utf-8` 等），但目前只验证了 MinGW-w64 构建与运行。
