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
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")

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
    set(DEFCONFIG "${CMAKE_CURRENT_LIST_DIR}/configs/linux-sdl.defconfig")
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

# ===== EasyLVGL：示例与单元测试（工具链在后续阶段接入） =====
set(EASYLVGL_APP_LIBS easylvgl_lib lvgl::lvgl lvgl::demos lvgl::thorvg lvgl_linux Threads::Threads)

foreach(app layer1_demo layer2_demo complete_demo components_basic_demo
            components_advanced_demo components_containers_demo untested_components_demo)
    add_executable(${app} examples/${app}.cpp)
    target_link_libraries(${app} PRIVATE ${EASYLVGL_APP_LIBS})
endforeach()

add_executable(widgets_demo examples/widgets_demo.cpp)
target_include_directories(widgets_demo PRIVATE ${CMAKE_SOURCE_DIR}/examples/widgets/generated)
target_link_libraries(widgets_demo PRIVATE ${EASYLVGL_APP_LIBS})

add_executable(cpp_tests tests/cpp/test_wrappers.cpp)
target_include_directories(cpp_tests PRIVATE ${CMAKE_SOURCE_DIR}/easylvgl)
target_link_libraries(cpp_tests PRIVATE ${EASYLVGL_APP_LIBS})

add_custom_target(run_cpp_tests COMMAND $<TARGET_FILE:cpp_tests> DEPENDS cpp_tests)
