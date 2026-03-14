cmake_minimum_required(VERSION 3.10)
project(lvgl_cpp)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(lvgl)
target_include_directories(lvgl PUBLIC ${PROJECT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR})

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

