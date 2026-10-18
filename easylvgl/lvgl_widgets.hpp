/**************************************************************************************************
 * @file        lvgl_widgets.hpp
 * @brief       lvgl_widgets
 * @details     EasyLVGL —— 让 LVGL 开发像 Qt 一样简单：C++ 封装 + 可视化设计器 + 代码生成。
 *
 * @author      taoist.luo
 * @version     1.0.0
 * @date        2026-10-18
 *
 * Copyright (c) 2026 taoist.luo (https://github.com/TaoistLuo/EasyLVGL)
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *************************************************************************************************/

#pragma once
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
