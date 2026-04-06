cmake_minimum_required(VERSION 3.14)
project(lvgl-designer LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

set(LVGL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../lvgl")
set(LVGL_INCLUDE_DIR "${LVGL_DIR}")
set(LV_CONF_INCLUDE_SIMPLE ON CACHE BOOL "Use simple include for lv_conf.h" FORCE)
set(LV_CONF_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../lv_conf.h" CACHE PATH "Path to lv_conf.h" FORCE)
set(LV_USE_THORVG_INTERNAL OFF CACHE BOOL "Disable ThorVG" FORCE)
add_subdirectory(${LVGL_DIR} ${CMAKE_CURRENT_BINARY_DIR}/lvgl_build)

set(LUIC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../luic")
set(LVGLPP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../lvgl++")

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
    src/panels/TreePanel.cpp
    src/commands/UndoCommands.hpp
    src/commands/UndoCommands.cpp
)

target_include_directories(lvgl-designer PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${LUIC_DIR}"
    "${LUIC_DIR}/.."
    "${LVGLPP_DIR}"
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
    ${SDL2_LIBRARIES}
    Qt5::Widgets
)

set_target_properties(lvgl-designer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
