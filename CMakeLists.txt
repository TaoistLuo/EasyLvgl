cmake_minimum_required(VERSION 3.11)
project(lvgl)

option(WERROR "Treat warnings as errors for LVGL targets" OFF)
option(ASAN "Enable Sanitizer Options" OFF)
option(LVGL_USE_INSTALLED "Use installed version of LVGL" OFF)
option(LVGL_USE_CONF_H "Use an lv_conf.h instead of kconfig" OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 所有可执行文件统一输出到源码树下的 bin/。
# 多配置生成器（MSVC）会忽略 EXECUTABLE_OUTPUT_PATH，需要 per-config 变量。
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
if(CMAKE_CONFIGURATION_TYPES)
    foreach(_cfg ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER "${_cfg}" _cfg_upper)
        set(EXECUTABLE_OUTPUT_PATH_${_cfg_upper} ${PROJECT_SOURCE_DIR}/bin)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_cfg_upper} ${PROJECT_SOURCE_DIR}/bin)
    endforeach()
endif()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")

# MinGW 下的编译期差异（对 LVGL 源码零修改，仅调整编译宏）：
#  - GCC 7.x 的 libstdc++ 在 C++ 中要显式开启 inttypes 格式宏，ThorVG 用到 PRIu32
#  - MinGW 默认按 Server 2003 裁剪 Win32 头，且旧版头文件没有 Win8.1 的 DPI 消息常量，
#    LVGL 的 Win32 后端会用到 WM_DPICHANGED
if(MINGW)
    add_definitions(-D__STDC_FORMAT_MACROS)
    add_definitions(-D_WIN32_WINNT=0x0601 -DWINVER=0x0601)
    add_definitions(-DWM_DPICHANGED=0x02E0)
endif()

if(ASAN)
    message(STATUS "Enabling Sanitizer")
    add_compile_options(-fsanitize=address,undefined,null)
    add_link_options(-fsanitize=address,undefined,null,leak)
endif()

if(LVGL_USE_INSTALLED)
    find_package(lvgl REQUIRED)
elseif(LVGL_USE_CONF_H)
    set(LV_BUILD_SET_CONFIG_OPTS ON CACHE BOOL "parse lv_conf.h" FORCE)
    add_subdirectory(lvgl)
else()
    set(LV_BUILD_USE_KCONFIG ON CACHE BOOL "use kconfig" FORCE)
    set(DOTCONFIG "${CMAKE_CURRENT_LIST_DIR}/.config")
    if(WIN32)
        set(DEFCONFIG "${CMAKE_CURRENT_LIST_DIR}/configs/windows-win32.defconfig")
    else()
        set(DEFCONFIG "${CMAKE_CURRENT_LIST_DIR}/configs/linux-sdl.defconfig")
    endif()
    set(LV_BUILD_KCONFIG_ROOT "${CMAKE_CURRENT_LIST_DIR}/Kconfig" CACHE PATH "path to root kconfig" FORCE)

    if(EXISTS "${DOTCONFIG}")
        message(STATUS "Using .config found to build LVGL")
        set(LV_BUILD_DOTCONFIG_PATH "${DOTCONFIG}" CACHE PATH "path to .config" FORCE)
        unset(LV_BUILD_DEFCONFIG_PATH)
    else()
        message(STATUS "No .config found, using ${DEFCONFIG} to build LVGL")
        set(LV_BUILD_DEFCONFIG_PATH "${DEFCONFIG}" CACHE PATH "path to defconfig" FORCE)
        unset(LV_BUILD_DOTCONFIG_PATH)
    endif()

    add_subdirectory(lvgl)
endif()

if(CONFIG_LV_USE_DEMO_TRUCK)
    add_subdirectory(demos/lv_demo_truck)
    set(EXTRA_LIBS lv_demo_truck_ui)
endif()

file(GLOB LV_LINUX_SRC src/lib/*.c)
set(LV_LINUX_INC src/lib)
set_property(GLOBAL PROPERTY LV_LINUX_BACKEND_SRC "")
set(DISPLAY_BACKENDS src/lib/display_backends)
set(INDEV_BACKENDS src/lib/indev_backends)

function(lv_linux_add_backend dir file)
    set_property(GLOBAL APPEND PROPERTY LV_LINUX_BACKEND_SRC ${dir}/${file})
endfunction()

function(lv_linux_add_backend_if)
    if(ARGC LESS 3)
        message(FATAL_ERROR "lv_linux_add_backend_if: expected <condition...> <folder> <file>, got: ${ARGV}")
    endif()
    set(condition ${ARGV})
    list(POP_BACK condition file)
    list(POP_BACK condition folder)
    if(${condition})
        lv_linux_add_backend(${folder} ${file})
    endif()
endfunction()

# Add every backend in case we're using an installed version of LVGL to avoid having to parse installed lv_conf.h
lv_linux_add_backend_if(LVGL_USE_INSTALLED OR CONFIG_LV_USE_WAYLAND     ${DISPLAY_BACKENDS} wayland.c)
lv_linux_add_backend_if(LVGL_USE_INSTALLED OR CONFIG_LV_USE_LINUX_DRM   ${DISPLAY_BACKENDS} drm.c)
lv_linux_add_backend_if(LVGL_USE_INSTALLED OR CONFIG_LV_USE_LINUX_FBDEV ${DISPLAY_BACKENDS} fbdev.c)
lv_linux_add_backend_if(LVGL_USE_INSTALLED OR CONFIG_LV_USE_SDL         ${DISPLAY_BACKENDS} sdl.c)
lv_linux_add_backend_if(LVGL_USE_INSTALLED OR CONFIG_LV_USE_WINDOWS     ${DISPLAY_BACKENDS} win32.c)
lv_linux_add_backend_if(LVGL_USE_INSTALLED OR CONFIG_LV_USE_X11         ${DISPLAY_BACKENDS} x11.c)
lv_linux_add_backend_if(LVGL_USE_INSTALLED OR CONFIG_LV_USE_GLFW        ${DISPLAY_BACKENDS} glfw3.c)
lv_linux_add_backend_if(LVGL_USE_INSTALLED OR CONFIG_LV_USE_EVDEV       ${INDEV_BACKENDS}   evdev.c)

get_property(BACKEND_SRC GLOBAL PROPERTY LV_LINUX_BACKEND_SRC)

# Library type is determined by BUILD_SHARED_LIBS (shared if ON, static if OFF)
add_library(lvgl_linux ${LV_LINUX_SRC} ${BACKEND_SRC})

set(LIBS lvgl::lvgl)

if(CONFIG_LV_BUILD_DEMOS)
list(APPEND LIBS lvgl::demos)
endif()

if(CONFIG_LV_BUILD_EXAMPLES)
list(APPEND LIBS lvgl::examples)
endif()
find_package(Threads REQUIRED)
if(WIN32 AND CONFIG_LV_USE_WINDOWS)
    list(APPEND LIBS user32 gdi32 imm32 shell32 ole32 oleaut32 uuid comctl32)
endif()

target_link_libraries(lvgl_linux PUBLIC ${LIBS})
add_executable(main src/main.cpp)
add_subdirectory(easylvgl)
target_include_directories(main PRIVATE src/lib)
target_link_libraries(main PRIVATE easylvgl_lib)
target_link_libraries(main PUBLIC lvgl_linux ${EXTRA_LIBS} Threads::Threads)

if(CONFIG_LV_USE_DEMO_TRUCK)
    target_compile_definitions(main PUBLIC CONFIG_LV_USE_DEMO_TRUCK)
endif()

if(WERROR)
    target_compile_options(main PRIVATE -Werror)
    target_compile_options(lvgl_linux PRIVATE -Werror)
    if(TARGET lvgl)
        target_compile_options(lvgl PRIVATE -Werror)
    endif()
endif()


# Install the lvgl_linux library and its headers
install(DIRECTORY src/lib/
    DESTINATION include/lvgl
    FILES_MATCHING
    PATTERN "backends*" EXCLUDE
    PATTERN "*.h")

install(TARGETS lvgl_linux
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

add_custom_target(run COMMAND ${EXECUTABLE_OUTPUT_PATH}/main DEPENDS main)

# ===== EasyLVGL：示例与单元测试 =====
set(EASYLVGL_APP_LIBS easylvgl_lib lvgl::lvgl lvgl::demos lvgl::thorvg lvgl_linux Threads::Threads)

foreach(app layer1_demo layer2_demo complete_demo components_basic_demo
            components_advanced_demo components_containers_demo untested_components_demo)
    add_executable(${app} examples/${app}.cpp)
    target_link_libraries(${app} PRIVATE ${EASYLVGL_APP_LIBS})
endforeach()

# ===== luic：.lui -> C++ 代码生成器 =====
add_executable(luic tools/luic/main.cpp)
target_include_directories(luic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tools/luic)
target_compile_features(luic PRIVATE cxx_std_17)
set_target_properties(luic PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# ui/*.lui -> ui_generated/*_ui.hpp
file(GLOB LUI_FILES "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.lui")
set(LUI_GENERATED "")
foreach(LUI_FILE ${LUI_FILES})
    get_filename_component(LUI_NAME ${LUI_FILE} NAME_WE)
    set(LUI_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/ui_generated/${LUI_NAME}_ui.hpp")
    add_custom_command(
        OUTPUT ${LUI_OUTPUT}
        COMMAND $<TARGET_FILE:luic> ${LUI_FILE} -o ${LUI_OUTPUT}
        DEPENDS ${LUI_FILE} luic
        COMMENT "luic: ${LUI_NAME}.lui -> ${LUI_NAME}_ui.hpp"
    )
    list(APPEND LUI_GENERATED ${LUI_OUTPUT})
endforeach()
add_custom_target(ui_compile ALL DEPENDS ${LUI_GENERATED})

# examples/widgets/*.lui -> examples/widgets/generated/*_ui.hpp
file(GLOB WIDGET_LUI_FILES "${CMAKE_CURRENT_SOURCE_DIR}/examples/widgets/*.lui")
set(WIDGET_GENERATED "")
foreach(LUI_FILE ${WIDGET_LUI_FILES})
    get_filename_component(LUI_NAME ${LUI_FILE} NAME_WE)
    set(LUI_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/examples/widgets/generated/${LUI_NAME}_ui.hpp")
    add_custom_command(
        OUTPUT ${LUI_OUTPUT}
        COMMAND $<TARGET_FILE:luic> ${LUI_FILE} -o ${LUI_OUTPUT}
        DEPENDS ${LUI_FILE} luic
        COMMENT "luic: widgets/${LUI_NAME}.lui -> ${LUI_NAME}_ui.hpp"
    )
    list(APPEND WIDGET_GENERATED ${LUI_OUTPUT})
endforeach()
add_custom_target(widgets_ui_compile ALL DEPENDS ${WIDGET_GENERATED})

add_executable(widgets_demo examples/widgets_demo.cpp)
add_dependencies(widgets_demo widgets_ui_compile)
target_include_directories(widgets_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/examples/widgets/generated)
target_link_libraries(widgets_demo PRIVATE ${EASYLVGL_APP_LIBS})
add_custom_target(run_widgets COMMAND $<TARGET_FILE:widgets_demo> DEPENDS widgets_demo)

# ===== luic 端到端示例：使用 ui_compile 生成的主界面 =====
add_executable(luic_demo examples/luic_demo.cpp)
add_dependencies(luic_demo ui_compile)
target_include_directories(luic_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ui_generated)
target_link_libraries(luic_demo PRIVATE ${EASYLVGL_APP_LIBS})
add_custom_target(run_luic_demo COMMAND $<TARGET_FILE:luic_demo> DEPENDS luic_demo)

# ===== 图片资源编译期嵌入示例（qrc 风格） =====
# assets/*.c 与 lvgl_assets.h 由 scripts/assets/convert_assets.py 在构建时生成（不入库）。
find_package(Python3 COMPONENTS Interpreter QUIET)
if(Python3_Interpreter_FOUND)
    set(EASYLVGL_PYTHON ${Python3_EXECUTABLE})
else()
    set(EASYLVGL_PYTHON python)
endif()

set(EMBED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/examples/embedded_image)
set(EMBED_GEN ${EMBED_DIR}/generated/screen_ui.hpp)
set(EMBED_ASSETS_C ${EMBED_DIR}/assets/caret.c)
set(EMBED_ASSETS_H ${EMBED_DIR}/assets/lvgl_assets.h)
add_custom_command(
    OUTPUT ${EMBED_ASSETS_C} ${EMBED_ASSETS_H}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${EMBED_DIR}/assets
    COMMAND ${EASYLVGL_PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/assets/convert_assets.py
            ${EMBED_DIR}/screen.lui
    DEPENDS ${EMBED_DIR}/screen.lui ${CMAKE_CURRENT_SOURCE_DIR}/scripts/assets/convert_assets.py
    COMMENT "convert_assets: screen.lui resources -> assets/lvgl_assets.h"
)
add_custom_command(
    OUTPUT ${EMBED_GEN}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${EMBED_DIR}/generated
    COMMAND $<TARGET_FILE:luic> ${EMBED_DIR}/screen.lui -o ${EMBED_GEN}
            --assets-include "assets/lvgl_assets.h"
    DEPENDS ${EMBED_DIR}/screen.lui luic ${EMBED_ASSETS_H}
    COMMENT "luic: embedded_image/screen.lui -> generated/screen_ui.hpp"
)
add_executable(embedded_image_demo examples/embedded_image/main.cpp ${EMBED_GEN} ${EMBED_ASSETS_C})
target_include_directories(embedded_image_demo PRIVATE ${EMBED_DIR} ${EMBED_DIR}/generated)
target_link_libraries(embedded_image_demo PRIVATE ${EASYLVGL_APP_LIBS})
add_custom_target(run_embedded_image COMMAND $<TARGET_FILE:embedded_image_demo> DEPENDS embedded_image_demo)

add_custom_target(convert_assets
    COMMAND ${EASYLVGL_PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/assets/convert_assets.py ${EMBED_DIR}/screen.lui
    COMMENT "把 .lui 里声明的图片资源转换成 LVGL C 数组")

add_executable(cpp_tests tests/cpp/test_wrappers.cpp)
target_include_directories(cpp_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/easylvgl)
target_link_libraries(cpp_tests PRIVATE ${EASYLVGL_APP_LIBS})

add_custom_target(run_cpp_tests COMMAND $<TARGET_FILE:cpp_tests> DEPENDS cpp_tests)

# ===== 合并为单一静态库 libeasylvgl.a（GNU ar MRI / MSVC lib.exe） =====
set(LVGLCPP_LIB_DIR "${CMAKE_BINARY_DIR}/lib")
set(LVGLCPP_INPUTS
    "${CMAKE_BINARY_DIR}/lvgl/liblvgl.a"
    "${CMAKE_BINARY_DIR}/lvgl/liblvgl_demos.a"
    "${CMAKE_BINARY_DIR}/lvgl/liblvgl_thorvg.a")

if(MSVC)
    set(LVGLCPP_LIB "${LVGLCPP_LIB_DIR}/easylvgl.lib")
    add_custom_command(
        OUTPUT ${LVGLCPP_LIB}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${LVGLCPP_LIB_DIR}"
        COMMAND ${CMAKE_AR} /NOLOGO /OUT:${LVGLCPP_LIB} ${LVGLCPP_INPUTS}
        DEPENDS lvgl lvgl_demos lvgl_thorvg
        COMMENT "Merging -> easylvgl.lib"
    )
else()
    set(LVGLCPP_LIB "${LVGLCPP_LIB_DIR}/libeasylvgl.a")
    add_custom_command(
        OUTPUT ${LVGLCPP_LIB}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${LVGLCPP_LIB_DIR}"
        COMMAND ${CMAKE_COMMAND}
            -D "LIB_OUT=${LVGLCPP_LIB}"
            -D "LIB_A=${CMAKE_BINARY_DIR}/lvgl/liblvgl.a"
            -D "LIB_B=${CMAKE_BINARY_DIR}/lvgl/liblvgl_demos.a"
            -D "LIB_C=${CMAKE_BINARY_DIR}/lvgl/liblvgl_thorvg.a"
            -D "AR=${CMAKE_AR}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/merge_libs.cmake"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        DEPENDS lvgl lvgl_demos lvgl_thorvg
        COMMENT "Merging -> libeasylvgl.a"
    )
endif()
add_custom_target(easylvgl_archive ALL DEPENDS ${LVGLCPP_LIB})
install(FILES ${LVGLCPP_LIB} DESTINATION lib)
install(TARGETS luic RUNTIME DESTINATION bin)
