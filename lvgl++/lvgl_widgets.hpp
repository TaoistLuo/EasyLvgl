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
#include "lvgl++/widgets/button.hpp"
#include "lvgl++/widgets/imagebutton.hpp"
#include "lvgl++/widgets/checkbox.hpp"
#include "lvgl++/widgets/radiobutton.hpp"
#include "lvgl++/widgets/switch.hpp"
#include "lvgl++/widgets/buttonmatrix.hpp"

// Input
#include "lvgl++/widgets/lineedit.hpp"
#include "lvgl++/widgets/textedit.hpp"
#include "lvgl++/widgets/combobox.hpp"
#include "lvgl++/widgets/spinbox.hpp"
#include "lvgl++/widgets/slider.hpp"
#include "lvgl++/widgets/roller.hpp"

// Display
#include "lvgl++/widgets/label.hpp"
#include "lvgl++/widgets/image.hpp"
#include "lvgl++/widgets/bar.hpp"
#include "lvgl++/widgets/arc.hpp"
#include "lvgl++/widgets/spinner.hpp"
#include "lvgl++/widgets/led.hpp"
#include "lvgl++/widgets/line.hpp"
#include "lvgl++/widgets/scale.hpp"
#include "lvgl++/widgets/calendar.hpp"
#include "lvgl++/widgets/chart.hpp"
#include "lvgl++/widgets/table.hpp"
#include "lvgl++/widgets/canvas.hpp"
#include "lvgl++/widgets/span.hpp"
#include "lvgl++/widgets/keyboard.hpp"
#include "lvgl++/widgets/animimage.hpp"

// Containers
#include "lvgl++/widgets/groupbox.hpp"
#include "lvgl++/widgets/list.hpp"
#include "lvgl++/widgets/tabview.hpp"
#include "lvgl++/widgets/menu.hpp"
#include "lvgl++/widgets/tileview.hpp"
#include "lvgl++/widgets/win.hpp"

// Dialogs
#include "lvgl++/widgets/msgbox.hpp"

// Layouts
#include "lvgl++/layouts/layout.hpp"

/**
 * @namespace lvgl
 * @brief LVGL++ namespace containing all widget classes
 * 
 * All LVGL++ components are in the lvgl namespace.
 * Use `using namespace lvgl;` for convenience.
 * 
 * @example
 * @code
 * #include "lvgl++/lvgl_widgets.hpp"
 * using namespace lvgl;
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
 * #include "lvgl++/lvgl_widgets.hpp"
 * using namespace lvgl;
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
