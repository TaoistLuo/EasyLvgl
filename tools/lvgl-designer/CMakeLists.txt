cmake_minimum_required(VERSION 3.14)
project(lvgl-designer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(LUIC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../luic")

add_executable(lvgl-designer
    src/main.cpp
    src/app/MainWindow.hpp
    src/app/MainWindow.cpp
    src/model/LuiDocument.hpp
    src/model/LuiDocument.cpp
    src/canvas/DesignerScene.hpp
    src/canvas/DesignerScene.cpp
    src/canvas/DesignerView.hpp
    src/canvas/DesignerView.cpp
    src/canvas/WidgetItem.hpp
    src/canvas/WidgetItem.cpp
    src/canvas/items/ButtonItem.hpp
    src/canvas/items/ButtonItem.cpp
    src/canvas/items/LabelItem.hpp
    src/canvas/items/LabelItem.cpp
    src/canvas/items/SliderItem.hpp
    src/canvas/items/SliderItem.cpp
    src/canvas/items/BarItem.hpp
    src/canvas/items/BarItem.cpp
    src/canvas/items/CheckboxItem.hpp
    src/canvas/items/CheckboxItem.cpp
    src/canvas/items/SwitchItem.hpp
    src/canvas/items/SwitchItem.cpp
    src/canvas/items/DropdownItem.hpp
    src/canvas/items/DropdownItem.cpp
    src/canvas/items/TextAreaItem.hpp
    src/canvas/items/TextAreaItem.cpp
    src/canvas/items/ItemFactory.hpp
    src/canvas/items/ItemFactory.cpp
    src/panels/ComponentPanel.hpp
    src/panels/ComponentPanel.cpp
    src/panels/PropertyPanel.hpp
    src/panels/PropertyPanel.cpp
    src/panels/TreePanel.hpp
    src/panels/TreePanel.cpp
    src/commands/UndoCommands.hpp
    src/commands/UndoCommands.cpp
)

target_include_directories(lvgl-designer PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${LUIC_DIR}"
    "${LUIC_DIR}/.."
)

target_link_libraries(lvgl-designer Qt5::Widgets)

set_target_properties(lvgl-designer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../bin"
)
