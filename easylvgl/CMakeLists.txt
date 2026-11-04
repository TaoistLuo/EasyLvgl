# EasyLVGL：header-only C++ 封装层
add_library(easylvgl_lib INTERFACE)
target_include_directories(easylvgl_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/../src/lib)
target_compile_features(easylvgl_lib INTERFACE cxx_std_17)
target_link_libraries(easylvgl_lib INTERFACE lvgl::lvgl)
