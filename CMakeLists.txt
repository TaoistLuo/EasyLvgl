cmake_minimum_required(VERSION 3.10)
project(lvgl_cpp)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Install path prefix" FORCE)
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(lvgl)
target_include_directories(lvgl PUBLIC ${PROJECT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/ui_generated)

# --- luic compiler ---
add_executable(luic tools/luic/main.cpp)
target_include_directories(luic PRIVATE ${PROJECT_SOURCE_DIR}/tools/luic)
target_compile_features(luic PRIVATE cxx_std_17)
set_target_properties(luic PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# --- Auto-compile .lui files (SDK-level .lui only) ---
file(GLOB LUI_FILES "${CMAKE_SOURCE_DIR}/ui/*.lui")
set(LUI_GENERATED "")
foreach(LUI_FILE ${LUI_FILES})
    get_filename_component(LUI_NAME ${LUI_FILE} NAME_WE)
    set(LUI_OUTPUT "${CMAKE_SOURCE_DIR}/ui_generated/${LUI_NAME}_ui.hpp")
    add_custom_command(
        OUTPUT ${LUI_OUTPUT}
        COMMAND ${PROJECT_SOURCE_DIR}/bin/luic ${LUI_FILE} -o ${LUI_OUTPUT}
        DEPENDS ${LUI_FILE} luic
        COMMENT "luic: ${LUI_NAME}.lui -> ${LUI_NAME}_ui.hpp"
    )
    list(APPEND LUI_GENERATED ${LUI_OUTPUT})
endforeach()
add_custom_target(ui_compile ALL DEPENDS ${LUI_GENERATED})

# --- Auto-compile widget gallery .lui (examples/widgets/*.lui) ---
file(GLOB WIDGET_LUI_FILES "${CMAKE_SOURCE_DIR}/examples/widgets/*.lui")
set(WIDGET_GENERATED "")
foreach(LUI_FILE ${WIDGET_LUI_FILES})
    get_filename_component(LUI_NAME ${LUI_FILE} NAME_WE)
    set(LUI_OUTPUT "${CMAKE_SOURCE_DIR}/examples/widgets/generated/${LUI_NAME}_ui.hpp")
    add_custom_command(
        OUTPUT ${LUI_OUTPUT}
        COMMAND ${PROJECT_SOURCE_DIR}/bin/luic ${LUI_FILE} -o ${LUI_OUTPUT}
        DEPENDS ${LUI_FILE} luic
        COMMENT "luic: widgets/${LUI_NAME}.lui -> ${LUI_NAME}_ui.hpp"
    )
    list(APPEND WIDGET_GENERATED ${LUI_OUTPUT})
endforeach()
add_custom_target(widgets_ui_compile ALL DEPENDS ${WIDGET_GENERATED})

add_executable(main main.cpp mouse_cursor_icon.c)

include(${CMAKE_CURRENT_LIST_DIR}/lvgl/tests/FindLibDRM.cmake)

find_package(SDL2)
find_package(SDL2_image)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

target_link_libraries(main lvgl lvgl::examples lvgl::demos lvgl::thorvg ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} m pthread)

add_executable(layer1_demo examples/layer1_demo.cpp)
target_link_libraries(layer1_demo lvgl lvgl::demos lvgl::thorvg ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} m pthread)

add_executable(layer2_demo examples/layer2_demo.cpp)
target_link_libraries(layer2_demo lvgl lvgl::demos lvgl::thorvg ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} m pthread)

add_executable(complete_demo examples/complete_demo.cpp)
target_link_libraries(complete_demo lvgl lvgl::demos lvgl::thorvg ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} m pthread)

add_executable(components_basic_demo examples/components_basic_demo.cpp)
target_link_libraries(components_basic_demo lvgl lvgl::demos lvgl::thorvg ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} m pthread)

add_executable(components_advanced_demo examples/components_advanced_demo.cpp)
target_link_libraries(components_advanced_demo lvgl lvgl::demos lvgl::thorvg ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} m pthread)

add_executable(components_containers_demo examples/components_containers_demo.cpp)
target_link_libraries(components_containers_demo lvgl lvgl::demos lvgl::thorvg ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} m pthread)

add_executable(untested_components_demo examples/untested_components_demo.cpp)
target_link_libraries(untested_components_demo lvgl lvgl::demos lvgl::thorvg ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} m pthread)

add_custom_target(run COMMAND ${EXECUTABLE_OUTPUT_PATH}/main DEPENDS main)
add_custom_target(run_layer1 COMMAND ${EXECUTABLE_OUTPUT_PATH}/layer1_demo DEPENDS layer1_demo)
add_custom_target(run_layer2 COMMAND ${EXECUTABLE_OUTPUT_PATH}/layer2_demo DEPENDS layer2_demo)
add_custom_target(run_complete COMMAND ${EXECUTABLE_OUTPUT_PATH}/complete_demo DEPENDS complete_demo)
add_custom_target(run_basic COMMAND ${EXECUTABLE_OUTPUT_PATH}/components_basic_demo DEPENDS components_basic_demo)
add_custom_target(run_advanced COMMAND ${EXECUTABLE_OUTPUT_PATH}/components_advanced_demo DEPENDS components_advanced_demo)
add_custom_target(run_containers COMMAND ${EXECUTABLE_OUTPUT_PATH}/components_containers_demo DEPENDS components_containers_demo)
add_custom_target(run_untested COMMAND ${EXECUTABLE_OUTPUT_PATH}/untested_components_demo DEPENDS untested_components_demo)

# --- luic demo (end-to-end test) ---
add_executable(luic_demo examples/luic_demo.cpp)
add_dependencies(luic_demo ui_compile)
target_link_libraries(luic_demo lvgl lvgl::demos lvgl::thorvg ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} m pthread)
add_custom_target(run_luic_demo COMMAND ${EXECUTABLE_OUTPUT_PATH}/luic_demo DEPENDS luic_demo)

# --- Per-component widget gallery examples ---
add_executable(widgets_demo examples/widgets_demo.cpp)
add_dependencies(widgets_demo widgets_ui_compile)
target_include_directories(widgets_demo PRIVATE ${CMAKE_SOURCE_DIR}/examples/widgets/generated)
target_link_libraries(widgets_demo lvgl lvgl::demos lvgl::thorvg ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} m pthread)
add_custom_target(run_widgets COMMAND ${EXECUTABLE_OUTPUT_PATH}/widgets_demo DEPENDS widgets_demo)

# ── 合并为单一库 liblvglcpp.a ───────────────────────────────────────
set(LVGLCPP_LIB "${CMAKE_BINARY_DIR}/lib/liblvglcpp.a")

add_custom_command(
    OUTPUT ${LVGLCPP_LIB}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/lib"
    COMMAND ${CMAKE_COMMAND}
        -D "LIB_OUT=${LVGLCPP_LIB}"
        -D "LIB_A=${CMAKE_BINARY_DIR}/lib/liblvgl.a"
        -D "LIB_B=${CMAKE_BINARY_DIR}/lib/liblvgl_demos.a"
        -D "LIB_C=${CMAKE_BINARY_DIR}/lib/liblvgl_thorvg.a"
        -D "AR=${CMAKE_AR}"
        -P "${CMAKE_SOURCE_DIR}/cmake/merge_libs.cmake"
    DEPENDS lvgl lvgl_demos lvgl_thorvg
    COMMENT "Merging -> liblvglcpp.a"
)
add_custom_target(lvglcpp_lib ALL DEPENDS ${LVGLCPP_LIB})

install(FILES ${LVGLCPP_LIB} DESTINATION lib)
install(TARGETS luic RUNTIME DESTINATION bin)
install(FILES ${PROJECT_SOURCE_DIR}/lv_conf.h DESTINATION include)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/lvgl/ DESTINATION include/lvgl
        FILES_MATCHING PATTERN "*.h"
        PATTERN "demos" EXCLUDE
        PATTERN "examples" EXCLUDE)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/lvgl++/ DESTINATION include/lvgl++
        FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")


