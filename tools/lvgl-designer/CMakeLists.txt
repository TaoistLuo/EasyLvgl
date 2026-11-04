cmake_minimum_required(VERSION 3.14)
project(lvgl-designer LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Threads REQUIRED)

set(LVGL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../lvgl")
set(LV_BUILD_USE_KCONFIG ON CACHE BOOL "use kconfig" FORCE)
set(LV_BUILD_KCONFIG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../Kconfig" CACHE PATH "path to root kconfig" FORCE)
set(LV_BUILD_DEFCONFIG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../configs/linux-sdl.defconfig" CACHE PATH "path to defconfig" FORCE)
set(LV_BUILD_INSTALL OFF CACHE BOOL "disable lvgl install rules" FORCE)
unset(LV_BUILD_DOTCONFIG_PATH CACHE)
unset(LV_BUILD_CONF_DIR CACHE)
unset(LV_BUILD_CONF_PATH CACHE)
unset(LV_CONF_PATH CACHE)
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
    "${LVGL_DIR}/.."
)

target_link_libraries(lvgl-designer PRIVATE
    lvgl::lvgl
    lvgl::thorvg
    Threads::Threads
    Qt5::Widgets
)

set_target_properties(lvgl-designer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
