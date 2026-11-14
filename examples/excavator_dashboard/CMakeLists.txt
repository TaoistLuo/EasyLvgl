cmake_minimum_required(VERSION 3.16)
project(excavator_dashboard C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# easylvgl 仓库根目录；换机器时用 -DEASYLVGL_ROOT=... 覆盖
if(NOT DEFINED EASYLVGL_ROOT)
    get_filename_component(EASYLVGL_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
endif()
message(STATUS "EASYLVGL_ROOT = ${EASYLVGL_ROOT}")

add_subdirectory(${EASYLVGL_ROOT} ${CMAKE_BINARY_DIR}/easylvgl_root EXCLUDE_FROM_ALL)
find_package(Threads REQUIRED)

set(MANIFEST "${CMAKE_CURRENT_SOURCE_DIR}/lvglpp.lpro")
set(GEN_DIR "${CMAKE_BINARY_DIR}/ui_generated")
set(ASSET_DIR "${CMAKE_BINARY_DIR}/assets")

file(GLOB LUI_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/ui/screens/*.lui"
    "${CMAKE_CURRENT_SOURCE_DIR}/ui/components/*.lui")

# luic：工程清单 -> ui_generated/*_ui.hpp（组件按依赖顺序生成）
add_custom_target(excavator_ui_compile ALL
    COMMAND $<TARGET_FILE:luic> --project "${MANIFEST}" --out-dir "${GEN_DIR}"
    DEPENDS ${LUI_FILES} "${MANIFEST}" luic
    COMMENT "luic: ExcavatorDashboard -> ${GEN_DIR}")

# 图片资源：PNG -> lv_image_dsc_t 常量 + lvgl_assets.h
find_package(Python3 COMPONENTS Interpreter QUIET)
if(Python3_Interpreter_FOUND)
    set(EASYLVGL_PYTHON "${Python3_EXECUTABLE}")
else()
    set(EASYLVGL_PYTHON python)
endif()

set(ASSET_SOURCES "${ASSET_DIR}/excavator.c" "${ASSET_DIR}/gear.c")
set(ASSET_HEADER "${ASSET_DIR}/lvgl_assets.h")
add_custom_command(
    OUTPUT ${ASSET_SOURCES} ${ASSET_HEADER}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ASSET_DIR}"
    COMMAND ${EASYLVGL_PYTHON} "${EASYLVGL_ROOT}/scripts/assets/convert_assets.py"
            "${MANIFEST}" -o "${ASSET_DIR}" --include "assets/lvgl_assets.h"
    DEPENDS "${MANIFEST}"
            "${EASYLVGL_ROOT}/scripts/assets/convert_assets.py"
            "${CMAKE_CURRENT_SOURCE_DIR}/assets/excavator.png"
            "${CMAKE_CURRENT_SOURCE_DIR}/assets/gear.png"
    COMMENT "convert_assets: excavator.png / gear.png -> lvgl_assets.h")
add_custom_target(excavator_assets DEPENDS ${ASSET_SOURCES} ${ASSET_HEADER})

# 多语言：Excel 文案源 -> LVGL 原生翻译数据（构建时生成，不入库）
set(I18N_XLSX "${CMAKE_CURRENT_SOURCE_DIR}/i18n/strings.xlsx")
set(I18N_GEN_DIR "${CMAKE_BINARY_DIR}/i18n")
set(I18N_HEADER "${I18N_GEN_DIR}/i18n_generated.hpp")
add_custom_command(
    OUTPUT ${I18N_HEADER}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${I18N_GEN_DIR}"
    COMMAND ${EASYLVGL_PYTHON} "${CMAKE_CURRENT_SOURCE_DIR}/tools/xlsx2i18n.py"
            "${I18N_XLSX}" "${I18N_HEADER}"
    DEPENDS "${I18N_XLSX}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/xlsx2i18n.py"
    COMMENT "i18n: strings.xlsx -> i18n_generated.hpp")
add_custom_target(excavator_i18n DEPENDS ${I18N_HEADER})

set(APP_SOURCES
    main.cpp
    screens/Dashboard.cpp
    screens/InfoCenter.cpp
    screens/Settings.cpp)
add_executable(excavator_dashboard ${APP_SOURCES} ${ASSET_SOURCES})
add_dependencies(excavator_dashboard excavator_ui_compile excavator_assets excavator_i18n)

# 项目字体：由工程清单的 "font" 决定，运行时 Tiny TTF 加载
set(EXCAVATOR_FONT_FILE "" CACHE FILEPATH "覆盖工程清单里的字体路径（可选）")
if(NOT EXCAVATOR_FONT_FILE)
    file(READ "${MANIFEST}" _project_json)
    if(_project_json MATCHES "\"font\"[ \t\r\n]*:[ \t\r\n]*\"([^\"]+)\"")
        get_filename_component(EXCAVATOR_FONT_FILE
            "${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_MATCH_1}" ABSOLUTE)
    endif()
endif()
if(NOT EXISTS "${EXCAVATOR_FONT_FILE}")
    message(WARNING "project font not found: ${EXCAVATOR_FONT_FILE}")
endif()
target_compile_definitions(excavator_dashboard PRIVATE
    EXCAVATOR_FONT_FILE="${EXCAVATOR_FONT_FILE}")

target_include_directories(excavator_dashboard PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${GEN_DIR}"
    "${I18N_GEN_DIR}"
    "${CMAKE_BINARY_DIR}"            # 解析生成代码里的 "assets/lvgl_assets.h"
    "${EASYLVGL_ROOT}/src/lib")      # driver_backends.h / simulator_settings.h
target_link_libraries(excavator_dashboard PRIVATE
    easylvgl_lib lvgl_linux lvgl::thorvg Threads::Threads)
