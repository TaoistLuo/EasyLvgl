cmake_minimum_required(VERSION 3.10)
project(luic)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8)
endif()

add_executable(luic main.cpp)
target_include_directories(luic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(luic PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
