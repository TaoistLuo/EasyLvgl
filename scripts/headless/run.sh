#!/usr/bin/env bash
# 构建并运行设计器的无头测试（不需要显示器，不需要 SDL/Qt Widgets 交互）。
#
#   test_root_interaction  : 直接驱动 LVGL indev，验证"点击画布空白选中根"、
#                            "点控件不选根"、"根不创建 LVGL 手柄"、"普通控件手柄仍可用"
#   test_sdlwidget_resize  : 用 Qt offscreen 平台验证根尺寸拖拽的坐标与数学
#                            （手柄命中、BR/TL/MR 各方向、不受显示分辨率钳制）
#
# 依赖：设计器已构建过一次（需要 build/lvgl 与 moc 产物）。
set -uo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BUILD="$ROOT/tools/lvgl-designer/build"
OUT="${TMPDIR:-/tmp}/lvglpp_headless"

[ -f "$BUILD/CMakeCache.txt" ] || { echo "请先构建一次设计器（tools/lvgl-designer/build）"; exit 1; }

QT="$(sed -n 's|^Qt5_DIR:PATH=\(.*\)/lib/cmake/Qt5$|\1|p' "$BUILD/CMakeCache.txt" | head -1)"
[ -n "$QT" ] || { echo "从 CMakeCache 里找不到 Qt5_DIR"; exit 1; }
MOC="$(find "$BUILD" -name moc_SDL2Widget.cpp | head -1)"
[ -n "$MOC" ] || { echo "找不到 moc_SDL2Widget.cpp"; exit 1; }

INCS=(-I "$ROOT/tools/lvgl-designer/src" -I "$ROOT" -I "$ROOT/build/lvgl"
      -isystem "$ROOT/lvgl" -isystem "$ROOT/lvgl/demos" -isystem "$ROOT/lvgl/src/libs/thorvg"
      -I "$QT/include" -I "$QT/include/QtCore" -I "$QT/include/QtGui" -I "$QT/include/QtWidgets")
LIBS=(-L"$ROOT/build/lib" -llvgl -llvgl_thorvg -L"$QT/lib" -Wl,-rpath,"$QT/lib" -lm -lpthread)
FLAGS=(-x c++ -g -std=gnu++17 -fPIC -O0)

mkdir -p "$OUT"
fails=0

echo "=== 1/2 test_root_interaction ==="
g++ "${FLAGS[@]}" "${INCS[@]}" \
    "$HERE/test_root_interaction.cpp" "$ROOT/tools/lvgl-designer/src/canvas/LvglRenderer.cpp" \
    -o "$OUT/test_root_interaction" "${LIBS[@]}" -lQt5Core || exit 1
( cd "$ROOT" && timeout 60 "$OUT/test_root_interaction" "$@" 2>/dev/null \
    | grep -vE '^\[Info\]|^\[Warn\]' ) || fails=1

echo
echo "=== 2/2 test_sdlwidget_resize ==="
g++ "${FLAGS[@]}" "${INCS[@]}" \
    "$HERE/test_sdlwidget_resize.cpp" "$ROOT/tools/lvgl-designer/src/canvas/SDL2Widget.cpp" "$MOC" \
    -o "$OUT/test_sdlwidget_resize" "${LIBS[@]}" -lQt5Widgets -lQt5Gui -lQt5Core || exit 1
QT_QPA_PLATFORM=offscreen timeout 60 "$OUT/test_sdlwidget_resize" 2>/dev/null \
    | grep -vE '^\[Info\]|^\[Warn\]' || fails=1

echo
[ "$fails" -eq 0 ] && echo "全部通过" || echo "存在失败"
exit "$fails"
