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

find_package(SDL2)
find_package(SDL2_image)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

set(LUIC ${LVGL_SDK_DIR}/bin/luic)

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
    ${LVGL_SDK_DIR}/lib/liblvglcpp.a
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    m pthread
)
