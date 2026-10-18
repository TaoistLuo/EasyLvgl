# EasyLVGL

**Build LVGL UIs like Qt — in pure C++, with zero runtime overhead.**

EasyLVGL turns LVGL development into **drag → generate → code**: a Qt visual designer produces
a declarative `.lui` (JSON) file, `luic` compiles it into plain C++ classes, and the resulting
code is indistinguishable from hand-written LVGL code — no runtime parsers, no binding engine,
no reflection.

```cpp
#include "easylvgl/lvgl_widgets.hpp"
using namespace easylvgl;

LButton btn;
btn.set_text("Click Me").set_size(120, 50).primary().on_click([](auto*) {
    printf("Clicked!\n");
});
```

## Highlights

| | |
|---|---|
| **43 components** | 35 widgets + 4 layouts, thin wrappers over LVGL 9.2 native widgets |
| **Visual designer** | Qt-based, WYSIWYG canvas, real LVGL rendering (preview == runtime) |
| **Code generator** | `luic` compiles `.lui` → C++ classes with `setup()` / `bind_events()` / virtual slots |
| **Project mode** | custom components (`kind: component`), exported parameters, `"@Component"` instances, dependency graph + cycle detection |
| **Compile-time assets** | images embedded as `lv_image_dsc_t` arrays (qrc-style), no filesystem at runtime |
| **Zero cost** | header-only wrapper, generated code is plain C++ calls into LVGL |
| **Linux + Windows** | builds with GCC / MinGW64 (MSVC untested), Qt 5.15 for the designer |

## Quick start

```bash
git clone --recursive https://github.com/TaoistLuo/EasyLVGL.git
cd EasyLVGL
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8          # library + luic + demos
```

Designer (requires Qt 5.15):

```bash
cd tools/lvgl-designer
cmake -S . -B build -DCMAKE_PREFIX_PATH=$HOME/Qt/5.15.2/gcc_64
cmake --build build -j8
./build/lvgl-designer example.lui        # or pass lvglpp.project.json
```

## A minimal project

```json
{
  "version": "1.0",
  "name": "MyScreen",
  "size": [400, 240],
  "children": [
    { "type": "LButton", "id": "ok",
      "props": { "pos": [20, 20], "size": [120, 50], "text": "OK" } }
  ]
}
```

```bash
bin/luic my_screen.lui -o my_screen_ui.hpp
```

```cpp
#include "my_screen_ui.hpp"
class App : public easylvgl::MyScreen_UI {
public:
    void on_ok_clicked(lv_event_t*) override { /* ... */ }
};
```

## Build

Same source and CMake on all three platforms; the display/input backend is selected
automatically: **native Win32 backend on Windows (no SDL2 required)**, SDL2 on Linux/macOS.

**Windows (MinGW-w64)**

```powershell
cmake -S . -B build-win -G "MinGW Makefiles"
cmake --build build-win -j 8
.\bin\main.exe
```

Or use `powershell -ExecutionPolicy Bypass -File scripts/win/build.ps1`.
See the [Windows build guide](docs/guides/windows_build.md).

**Linux / macOS (SDL2)**

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./bin/main
```

**Designer (Qt 5.15)**

```bash
cd tools/lvgl-designer
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/5.15.2/gcc_64 -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Documentation

- [luic guide](docs/guides/luic_guild.md) — `.lui` format, options, project mode
- [project-level design](docs/design/project-level-design.md) — components, exports, roadmap
- [json-ui-format](docs/design/json-ui-format.md) — format specification
- [Windows build guide](docs/guides/windows_build.md) — MinGW-w64 build & run

## License

MIT — see [LICENSE](LICENSE).

> EasyLVGL is not affiliated with or endorsed by LVGL Kft. "LVGL" is a trademark of LVGL Kft.
