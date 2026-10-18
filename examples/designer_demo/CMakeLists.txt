cmake_minimum_required(VERSION 3.10)
project(designer_demo C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT LVGL_SDK_DIR)
    get_filename_component(LVGL_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../build/install" ABSOLUTE)
endif()
message(STATUS "LVGL_SDK_DIR = ${LVGL_SDK_DIR}")

include_directories(
    ${LVGL_SDK_DIR}/include
    ${LVGL_SDK_DIR}/include/lvgl
)

find_package(Threads REQUIRED)

# 桌面窗口后端：Windows 用 LVGL 原生 Win32 后端，其它平台用 SDL2（与主工程一致）
if(WIN32)
    set(EASYLVGL_LIB "${LVGL_SDK_DIR}/lib/easylvgl.lib")
    set(EASYLVGL_DESKTOP_LIBS
        user32 gdi32 imm32 shell32 ole32 oleaut32 uuid comctl32 Threads::Threads)
else()
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image)
    include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
    set(EASYLVGL_LIB "${LVGL_SDK_DIR}/lib/libeasylvgl.a")
    set(EASYLVGL_DESKTOP_LIBS
        ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} m Threads::Threads)
endif()

set(LUIC "${LVGL_SDK_DIR}/bin/luic${CMAKE_EXECUTABLE_SUFFIX}")

file(GLOB LUI_FILES "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.lui")
set(LUI_GENERATED "")
foreach(LUI_FILE ${LUI_FILES})
    get_filename_component(LUI_NAME ${LUI_FILE} NAME_WE)
    set(LUI_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/generated/${LUI_NAME}_ui.hpp")
    add_custom_command(
        OUTPUT ${LUI_OUTPUT}
        COMMAND ${LUIC} ${LUI_FILE} -o ${LUI_OUTPUT}
        DEPENDS ${LUI_FILE}
        COMMENT "luic: ${LUI_NAME}.lui -> ${LUI_NAME}_ui.hpp"
    )
    list(APPEND LUI_GENERATED ${LUI_OUTPUT})
endforeach()
add_custom_target(ui_compile DEPENDS ${LUI_GENERATED})

include_directories(${CMAKE_CURRENT_BINARY_DIR}/generated)

add_executable(designer_demo main.cpp)
add_dependencies(designer_demo ui_compile)

target_link_libraries(designer_demo
    ${EASYLVGL_LIB}
    ${EASYLVGL_DESKTOP_LIBS}
)
