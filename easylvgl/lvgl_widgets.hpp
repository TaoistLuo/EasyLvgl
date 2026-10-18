#pragma once

/**
 * @file lvgl_widgets.hpp
 * @brief LVGL++ Widget Library - All Components Entry Point
 * @version 2.0.0
 * @date 2026-03-15
 * 
 * This file includes all LVGL++ widget components.
 * Include this single file to access all 32+ widget components.
 */

// Buttons
#include "easylvgl/widgets/obj.hpp"
#include "easylvgl/widgets/button.hpp"
#include "easylvgl/widgets/imagebutton.hpp"
#include "easylvgl/widgets/checkbox.hpp"
#include "easylvgl/widgets/radiobutton.hpp"
#include "easylvgl/widgets/switch.hpp"
#include "easylvgl/widgets/buttonmatrix.hpp"

// Input
#include "easylvgl/widgets/lineedit.hpp"
#include "easylvgl/widgets/textedit.hpp"
#include "easylvgl/widgets/combobox.hpp"
#include "easylvgl/widgets/spinbox.hpp"
#include "easylvgl/widgets/slider.hpp"
#include "easylvgl/widgets/roller.hpp"

// Display
#include "easylvgl/widgets/label.hpp"
#include "easylvgl/widgets/image.hpp"
#include "easylvgl/widgets/bar.hpp"
#include "easylvgl/widgets/arc.hpp"
#include "easylvgl/widgets/spinner.hpp"
#include "easylvgl/widgets/led.hpp"
#include "easylvgl/widgets/line.hpp"
#include "easylvgl/widgets/scale.hpp"
#include "easylvgl/widgets/calendar.hpp"
#include "easylvgl/widgets/chart.hpp"
#include "easylvgl/widgets/table.hpp"
#include "easylvgl/widgets/canvas.hpp"
#include "easylvgl/widgets/span.hpp"
#include "easylvgl/widgets/keyboard.hpp"
#include "easylvgl/widgets/animimage.hpp"

// Containers
#include "easylvgl/widgets/groupbox.hpp"
#include "easylvgl/widgets/list.hpp"
#include "easylvgl/widgets/tabview.hpp"
#include "easylvgl/widgets/menu.hpp"
#include "easylvgl/widgets/tileview.hpp"
#include "easylvgl/widgets/win.hpp"

// Dialogs
#include "easylvgl/widgets/msgbox.hpp"

// Layouts
#include "easylvgl/layouts/layout.hpp"

/**
 * @namespace easylvgl
 * @brief LVGL++ namespace containing all widget classes
 * 
 * All LVGL++ components are in the lvgl namespace.
 * Use `using namespace easylvgl;` for convenience.
 * 
 * @example
 * @code
 * #include "easylvgl/lvgl_widgets.hpp"
 * using namespace easylvgl;
 * 
 * LButton btn;
 * btn.text("Click Me").size(100, 50).primary();
 * @endcode
 */

/**
 * @mainpage LVGL++ Widget Library
 * 
 * @section intro Introduction
 * LVGL++ is a modern C++ wrapper for LVGL 9.2, providing:
 * - 32+ widget components with fluent API
 * - Type-safe event handling
 * - RAII memory management
 * - Qt-style naming (L prefix)
 * - Chain-able method calls
 * 
 * @section components Component Categories
 * 
 * @subsection core Core Widgets (8)
 * - LButton - Push button
 * - LLabel - Text label
 * - LSlider - Value slider
 * - LCheckbox - Checkbox
 * - LSwitch - Toggle switch
 * - LDropdown - Dropdown list
 * - LTextArea - Text input
 * - LImage - Image display
 * 
 * @subsection highfreq High-Frequency Widgets (6)
 * - LArc - Circular progress/slider
 * - LBar - Progress bar
 * - LSpinner - Loading animation
 * - LLed - LED indicator
 * - LLine - Line drawing
 * - LList - List container
 * 
 * @subsection medfreq Medium-Frequency Widgets (10)
 * - LTable - Data table
 * - LTabView - Tab container
 * - LMenu - Menu system
 * - LRoller - Roller selector
 * - LScale - Scale/gauge
 * - LSpan - Rich text
 * - LSpinbox - Number input
 * - LButtonMatrix - Button grid
 * - LImageButton - Image button
 * - LMsgBox - Message box
 * 
 * @subsection lowfreq Low-Frequency Widgets (8)
 * - LCalendar - Calendar picker
 * - LChart - Data chart
 * - LCanvas - Drawing canvas
 * - LKeyboard - Virtual keyboard
 * - LAnimImage - Animated image
 * - LTileView - Tile view
 * - LWin - Window container
 * - LColorWheel - Color picker wheel
 * 
 * @section usage Quick Start
 * 
 * @code
 * #include "easylvgl/lvgl_widgets.hpp"
 * using namespace easylvgl;
 * 
 * // Create a button
 * LButton btn;
 * btn.text("Click Me")
 *    .size(120, 50)
 *    .primary()
 *    .on_click([](auto*) {
 *        printf("Button clicked!\n");
 *    });
 * 
 * // Create a progress bar
 * LBar bar;
 * bar.range(0, 100)
 *    .value(75, LV_ANIM_ON)
 *    .colors(0xEEEEEE, 0x4CAF50);
 * 
 * // Create a spinner
 * LSpinner spinner;
 * spinner.size(50).color(0x2196F3);
 * @endcode
 * 
 * @section features Key Features
 * 
 * - **Fluent API**: Chain method calls for concise code
 * - **Type Safety**: Compile-time type checking
 * - **RAII**: Automatic memory management
 * - **Modern C++**: C++11/14/17 features
 * - **Qt-Style**: Familiar naming conventions
 * - **Event Handling**: Lambda support for callbacks
 * - **Complete**: 32+ widgets covering all LVGL components
 * 
 * @section license License
 * MIT License - See LICENSE file for details
 */
