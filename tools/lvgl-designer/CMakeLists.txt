cmake_minimum_required(VERSION 3.14)
project(lvgl-designer LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

# 桌面窗口后端：Windows 用 LVGL 原生 Win32 后端；其它平台沿用 SDL2。
# 说明：SDL2Widget 实际把 LVGL 渲染到自己的 framebuffer 后交给 Qt 绘制，
#       并不直接调用 SDL。Windows 上因此无需 SDL2；Linux 保留原 SDL2 链接。
if(WIN32)
    # LVGL Win32 后端所需：较新的 Windows 头声明 + 旧 MinGW 缺失的 WM_DPICHANGED
    add_definitions(-D_WIN32_WINNT=0x0601 -DWINVER=0x0601 -DWM_DPICHANGED=0x02E0)
    set(DESIGNER_DESKTOP_LIBS
        user32 gdi32 imm32 shell32 ole32 oleaut32 uuid comctl32)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    set(DESIGNER_DESKTOP_LIBS ${SDL2_LIBRARIES})
endif()

set(LVGL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../lvgl")
set(LVGL_INCLUDE_DIR "${LVGL_DIR}")
set(LV_CONF_INCLUDE_SIMPLE ON CACHE BOOL "Use simple include for lv_conf.h" FORCE)
set(LV_CONF_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../lv_conf.h" CACHE PATH "Path to lv_conf.h" FORCE)
set(LV_USE_THORVG_INTERNAL OFF CACHE BOOL "Disable ThorVG" FORCE)
add_subdirectory(${LVGL_DIR} ${CMAKE_CURRENT_BINARY_DIR}/lvgl_build)

set(LUIC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../luic")
set(EASYLVGL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../easylvgl")

add_executable(lvgl-designer
    src/main.cpp
    src/app/MainWindow.hpp
    src/app/MainWindow.cpp
    src/model/LuiDocument.hpp
    src/model/LuiDocument.cpp
    src/canvas/SDL2Widget.hpp
    src/canvas/SDL2Widget.cpp
    src/canvas/LvglRenderer.hpp
    src/canvas/LvglRenderer.cpp
    src/panels/ComponentPanel.hpp
    src/panels/ComponentPanel.cpp
    src/panels/PropertyPanel.hpp
    src/panels/PropertyPanel.cpp
    src/panels/TreePanel.hpp
    src/panels/ProjectPanel.hpp
    src/panels/OutputPanel.hpp
    resources/icons/icons.qrc
    src/panels/TreePanel.cpp
    src/commands/UndoCommands.hpp
    src/commands/UndoCommands.cpp
)

target_include_directories(lvgl-designer PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${LUIC_DIR}"
    "${LUIC_DIR}/.."
    "${EASYLVGL_DIR}"
    "${LVGL_INCLUDE_DIR}"
    "${LVGL_INCLUDE_DIR}/src"
    "${LVGL_INCLUDE_DIR}/.."
    "${SDL2_INCLUDE_DIRS}"
)

target_compile_definitions(lvgl-designer PRIVATE
    LV_CONF_INCLUDE_SIMPLE
)

target_link_libraries(lvgl-designer PRIVATE
    lvgl
    ${DESIGNER_DESKTOP_LIBS}
    Qt5::Widgets
)

set_target_properties(lvgl-designer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
