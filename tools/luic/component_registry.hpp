/**************************************************************************************************
 * @file        component_registry.hpp
 * @brief       luic 编译器（.lui -> C++）
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
#include <string>
#include <vector>
#include <map>

namespace luic {


/** 通用样式属性表：对所有组件类型都可用，由 codegen 生成 LStyle 设置块。
 *  判定标准：设计器渲染器用 lv_obj_set_style_* 应用的属性。
 *  （hidden/clickable/scrollable 等是对象 flag，不属于样式属性）
 *  kind: color(hex) / int / bool / src(资源路径) */
inline const std::map<std::string, std::string>& style_props() {
    static const std::map<std::string, std::string> m = {
        {"arc_color", "color"},
        {"arc_opa", "int"},
        {"arc_rounded", "bool"},
        {"arc_width", "int"},
        {"base_dir", "int"},
        {"bg_color", "color"},
        {"bg_grad_color", "color"},
        {"bg_grad_dir", "int"},
        {"bg_grad_stop", "int"},
        {"bg_image_opa", "int"},
        {"bg_image_recolor", "color"},
        {"bg_image_recolor_opa", "int"},
        {"bg_image_src", "src"},
        {"bg_image_tiled", "bool"},
        {"bg_main_stop", "int"},
        {"bg_opa", "int"},
        {"blend_mode", "int"},
        {"border_color", "color"},
        {"border_opa", "int"},
        {"border_post", "bool"},
        {"border_side", "int"},
        {"border_width", "int"},
        {"clip_corner", "bool"},
        {"col_spacing", "int"},
        {"color", "color"},
        {"dash_gap", "int"},
        {"dash_width", "int"},
        {"image_opa", "int"},
        {"image_recolor", "color"},
        {"image_recolor_opa", "int"},
        {"line_color", "color"},
        {"line_dash_gap", "int"},
        {"line_dash_width", "int"},
        {"line_opa", "int"},
        {"line_rounded", "bool"},
        {"line_width", "int"},
        {"margin_bottom", "int"},
        {"margin_left", "int"},
        {"margin_right", "int"},
        {"margin_top", "int"},
        {"max_height", "int"},
        {"max_width", "int"},
        {"min_height", "int"},
        {"min_width", "int"},
        {"opa", "int"},
        {"opacity", "int"},
        {"outline_color", "color"},
        {"outline_opa", "int"},
        {"outline_pad", "int"},
        {"outline_width", "int"},
        {"pad_bottom", "int"},
        {"pad_column", "int"},
        {"pad_left", "int"},
        {"pad_right", "int"},
        {"pad_row", "int"},
        {"pad_top", "int"},
        {"padding", "int"},
        {"radius", "int"},
        {"rounded", "bool"},
        {"row_spacing", "int"},
        {"shadow_color", "color"},
        {"shadow_ofs_x", "int"},
        {"shadow_ofs_y", "int"},
        {"shadow_opa", "int"},
        {"shadow_spread", "int"},
        {"shadow_width", "int"},
        {"text_align", "int"},
        {"text_color", "color"},
        {"text_decor", "int"},
        {"text_font_size", "int"},
        {"text_letter_space", "int"},
        {"text_line_space", "int"},
        {"text_opa", "int"},
        {"transform_pivot_x", "int"},
        {"transform_pivot_y", "int"},
        {"transform_rotation", "int"},
        {"transform_scale_x", "int"},
        {"transform_scale_y", "int"},
        {"transform_skew_x", "int"},
        {"transform_skew_y", "int"},
        {"translate_x", "int"},
        {"translate_y", "int"},
        {"width", "int"},
    };
    return m;
}

/** 返回样式属性类型，空串表示不是样式属性 */
inline std::string style_kind(const std::string& name) {
    const auto& m = style_props();
    auto it = m.find(name);
    return it == m.end() ? std::string() : it->second;
}


struct prop_def_t {
    std::string name;
    std::string type;     // "string", "int", "hex", "int_pair", "bool"
    std::string method;   // C++ method name, e.g. "text", "size", "range"
    bool is_theme = false;
    bool after_children = false;  // emit after children are created (layout state depending on child count)
    std::string prefix_arg;       // fixed leading argument, e.g. "LV_IMAGEBUTTON_STATE_RELEASED"
    std::string expand;           // special expansion: "argv", "lines", "cells", ...
};

struct event_def_t {
    std::string name;     // "clicked", "value_changed"
    std::string method;   // "on_click", "on_change"
};

struct component_def_t {
    std::string class_name;
    bool is_container;
    std::vector<prop_def_t> props;
    std::vector<event_def_t> events;
    std::string ctor_prop;   // single prop passed as extra constructor arg (legacy)
    std::vector<std::string> ctor_props;  // props passed as extra constructor args, in order

    /* 具默认参数的构造函数：替代 C++20 指定初始化器，兼容 GCC 7 (MinGW 7.3) 等旧编译器。
     * class_name 由 ComponentRegistry::reg() 填充，无需在此传入。 */
    component_def_t(bool is_container_ = false,
                    std::vector<prop_def_t> props_ = {},
                    std::vector<event_def_t> events_ = {},
                    std::string ctor_prop_ = {},
                    std::vector<std::string> ctor_props_ = {})
        : is_container(is_container_),
          props(std::move(props_)),
          events(std::move(events_)),
          ctor_prop(std::move(ctor_prop_)),
          ctor_props(std::move(ctor_props_)) {}

    const prop_def_t* find_prop(const std::string& name) const {
        for (auto& p : props) {
            if (p.name == name) return &p;
        }
        return nullptr;
    }

    const event_def_t* find_event(const std::string& name) const {
        for (auto& e : events) {
            if (e.name == name) return &e;
        }
        return nullptr;
    }
};

class ComponentRegistry {
    std::map<std::string, component_def_t> m_components;

    void reg(const std::string& name, component_def_t def) {
        def.class_name = name;
        m_components[name] = std::move(def);
    }

public:
    ComponentRegistry() {
        // ===== Basic Widgets =====

        reg("LButton", {
            false,
            {
                {"text",     "string",   "set_text"},
                {"size",     "int_pair", "set_size"},
                {"pos",      "int_pair", "set_pos"},
                {"disabled", "bool",     "set_disabled"},
                {"theme",    "string",   "", true},
            },
            {
                {"clicked",  "on_click"},
                {"pressed",  "on_click"},
                {"released", "on_click"},
            }
        });

        reg("LLabel", {
            false,
            {
                {"text",      "string",   "set_text"},
                {"color",     "hex",      "set_color"},
                {"font_size", "int",      "set_font_size"},
                {"long_mode", "int",      "set_long_mode"},
                {"size",      "int_pair", "set_size"},
                {"pos",       "int_pair", "set_pos"},
            },
            {}
        });

        reg("LSlider", {
            false,
            {
                {"range",      "int_pair", "set_range"},
                {"value",      "int",      "set_value"},
                {"left_value", "int",      "set_left_value"},
                {"mode",       "int",      "set_mode"},
                {"size",       "int_pair", "set_size"},
                {"pos",        "int_pair", "set_pos"},
            },
            {
                {"value_changed", "on_change"},
            }
        });

        reg("LCheckbox", {
            false,
            {
                {"text",    "string",   "set_text"},
                {"checked", "bool",     "set_checked"},
                {"size",    "int_pair", "set_size"},
                {"pos",     "int_pair", "set_pos"},
            },
            {
                {"value_changed", "on_change"},
            }
        });

        reg("LSwitch", {
            false,
            {
                {"checked",  "bool",     "set_checked"},
                {"enabled",  "bool",     "set_enabled"},
                {"size",     "int_pair", "set_size"},
                {"pos",      "int_pair", "set_pos"},
            },
            {
                {"value_changed", "on_change"},
            }
        });

        // ===== Display Widgets =====

        reg("LImage", {
            false,
            {
                {"src",       "string",   "set_src"},
                {"rotation",  "int",      "set_rotation"},
                {"scale",     "int",      "set_scale"},
                {"scale_x",   "int",      "set_scale_x"},
                {"scale_y",   "int",      "set_scale_y"},
                {"pivot",     "int_pair", "set_pivot"},
                {"offset",    "int_pair", "set_offset"},
                {"opacity",   "int",      "set_opacity"},
                {"antialias", "bool",     "set_antialias"},
                {"inner_align","int",     "set_inner_align"},
                {"size",      "int_pair", "set_size"},
                {"pos",       "int_pair", "set_pos"},
            },
            {
                {"clicked", "on_click"},
            }
        });

        reg("LArc", {
            false,
            {
                {"range",       "int_pair", "set_range"},
                {"value",       "int",      "set_value"},
                {"rotation",    "int",      "set_rotation"},
                {"mode",        "int",      "set_mode"},
                {"bg_angles",   "int_pair", "set_bg_angles"},
                {"angles",      "int_pair", "set_angles"},
                {"knob_offset", "int",      "set_knob_offset"},
                {"change_rate", "int",      "set_change_rate"},
                {"knob_enabled","bool",     "set_knob_enabled"},
                {"size",        "int_pair", "set_size"},
                {"pos",         "int_pair", "set_pos"},
            },
            {
                {"value_changed", "on_change"},
            }
        });

        reg("LBar", {
            false,
            {
                {"range",       "int_pair", "set_range"},
                {"value",       "int",      "set_value"},
                {"start_value", "int",      "set_start_value"},
                {"mode",        "int",      "set_mode"},
                {"orientation", "bool",     "set_orientation"},
                {"size",        "int_pair", "set_size"},
                {"pos",         "int_pair", "set_pos"},
            },
            {}
        });

        reg("LSpinner", {
            false,
            {
                {"color", "hex",      "set_color"},
                {"size",  "int_pair", "set_size"},
                {"pos",   "int_pair", "set_pos"},
            },
            {}
        });

        reg("LLed", {
            false,
            {
                {"color",      "hex",      "set_color"},
                {"brightness", "int",      "set_brightness"},
                {"on",         "bool",     "on"},
                {"size",       "int_pair", "set_size"},
                {"pos",        "int_pair", "set_pos"},
            },
            {}
        });

        reg("LLine", {
            false,
            {
                {"points",   "string",   "set_points", false, false, "", "points"},
                {"width",    "int",      "set_width"},
                {"color",    "hex",      "set_color"},
                {"y_invert",  "bool",     "set_y_invert"},
                {"dash_width","int",      "set_dash_width"},
                {"dash_gap",  "int",      "set_dash_gap"},
                {"rounded",   "bool",     "set_rounded"},
                {"size",      "int_pair", "set_size"},
                {"pos",       "int_pair", "set_pos"},
            },
            {}
        });

        // ===== Input Widgets =====

        reg("LRoller", {
            false,
            {
                {"options",           "string",   "set_options"},
                {"selected",          "int",      "set_selected"},
                {"visible_row_count", "int",      "set_visible_row_count"},
                {"size",              "int_pair", "set_size"},
                {"pos",               "int_pair", "set_pos"},
            },
            {
                {"value_changed", "on_change"},
            }
        });

        reg("LSpinbox", {
            false,
            {
                {"cursor_pos", "int", "set_cursor_pos"},
                {"digit_step_direction", "int", "set_digit_step_direction"},
                {"range",        "int_pair", "set_range"},
                {"value",        "int",      "set_value"},
                {"step",         "int",      "set_step"},
                {"digit_format", "int_pair", "set_digit_format"},
                {"rollover",     "bool",     "set_rollover"},
                {"size",         "int_pair", "set_size"},
                {"pos",          "int_pair", "set_pos"},
            },
            {
                {"value_changed", "on_change"},
            }
        });

        reg("LButtonMatrix", {
            false,
            {
                {"map",         "string",   "set_map", false, false, "", "argv"},
                {"selected",    "int",      "set_selected"},
                {"one_checked", "bool",     "set_one_checked"},
                {"size",        "int_pair", "set_size"},
                {"pos",         "int_pair", "set_pos"},
            },
            {
                {"value_changed", "on_value_changed"},
            }
        });

        reg("LKeyboard", {
            false,
            {
                {"textarea", "string",   "set_textarea", false, false, "", "ref"},
                {"mode",     "int",      "set_mode"},
                {"popovers", "bool",     "set_popovers"},
                {"size",     "int_pair", "set_size"},
                {"pos",      "int_pair", "set_pos"},
            },
            {}
        });

        reg("LImageButton", {
            false,
            {
                {"state",        "int",      "set_state"},
                {"src_released", "string",   "set_src", false, false, "LV_IMAGEBUTTON_STATE_RELEASED"},
                {"src_pressed",  "string",   "set_src", false, false, "LV_IMAGEBUTTON_STATE_PRESSED"},
                {"src_disabled", "string",   "set_src", false, false, "LV_IMAGEBUTTON_STATE_DISABLED"},
                {"size",         "int_pair", "set_size"},
                {"pos",          "int_pair", "set_pos"},
            },
            {
                {"clicked", "on_click"},
            }
        });

        // ===== Data Display Widgets =====

        reg("LTable", {
            false,
            {
                {"row_count",     "int",      "set_row_count"},
                {"column_count",  "int",      "set_column_count"},
                {"column_widths", "string",   "set_column_width", false, false, "", "int_2arg"},
                {"data",          "string",   "set_cell_value", false, false, "", "cells"},
                {"size",          "int_pair", "set_size"},
                {"pos",           "int_pair", "set_pos"},
            },
            {}
        });

        reg("LChart", {
            false,
            {
                {"type",           "int",      "set_type"},
                {"point_count",    "int",      "set_point_count"},
                {"div_line_count", "int_pair", "set_div_line_count"},
                {"update_mode",    "int",      "set_update_mode"},
                {"series",         "string",   "set_next_value", false, false, "", "series"},
                {"size",           "int_pair", "set_size"},
                {"pos",            "int_pair", "set_pos"},
            },
            {}
        });

        reg("LScale", {
            false,
            {
                {"draw_ticks_on_top", "bool", "set_draw_ticks_on_top"},
                {"mode",             "int",      "set_mode"},
                {"range",            "int_pair", "set_range"},
                {"total_tick_count", "int",      "set_total_tick_count"},
                {"major_tick_every", "int",      "set_major_tick_every"},
                {"rotation",         "int",      "set_rotation"},
                {"angle_range",      "int",      "set_angle_range"},
                {"label_show",       "bool",     "set_label_show"},
                {"size",             "int_pair", "set_size"},
                {"pos",              "int_pair", "set_pos"},
            },
            {}
        });

        reg("LCalendar", {
            false,
            {
                {"today_date",  "int_triple", "set_today_date"},
                {"showed_date", "int_pair",   "set_showed_date"},
                {"size",        "int_pair",   "set_size"},
                {"pos",         "int_pair",   "set_pos"},
            },
            {}
        });

        reg("LCanvas", {
            false,
            {
                {"size", "int_pair", "set_size"},
                {"pos",  "int_pair", "set_pos"},
            },
            {}
        });

        // ===== Container Widgets =====

        reg("LList", {
            true,
            {
                {"items", "string",   "add_button", false, false, "", "list_items"},
                {"size",  "int_pair", "set_size"},
                {"pos",   "int_pair", "set_pos"},
            },
            {}
        });

        reg("LTabView", {
            true,
            {
                {"tabs",             "string",   "add_tab", false, false, "", "lines"},
                {"tab_bar_position", "int",      "set_tab_bar_position"},
                {"tab_bar_size",     "int",      "set_tab_bar_size"},
                {"size",             "int_pair", "set_size"},
                {"pos",              "int_pair", "set_pos"},
            },
            {}
        });

        reg("LMenu", {
            true,
            {
                {"mode_header",           "int",    "set_mode_header"},
                {"mode_root_back_button", "int",    "set_mode_root_back_button"},
                {"active_page",           "string", "set_page", false, false, "", "ref"},
                {"size",                  "int_pair", "set_size"},
                {"pos",                   "int_pair", "set_pos"},
            },
            {}
        });

        reg("LMenuPage", {
            true,
            {
                {"title", "string",   "set_title"},
                {"size",  "int_pair", "set_size"},
                {"pos",   "int_pair", "set_pos"},
            },
            {},
            "title"
        });

        reg("LTile", {
            true,
            {
                {"col",  "int",      ""},
                {"row",  "int",      ""},
                {"dir",  "int",      ""},
                {"size", "int_pair", "set_size"},
                {"pos",  "int_pair", "set_pos"},
            },
            {},
            "",
            {"col", "row", "dir"}
        });

        reg("LMenuCont", {
            true,
            {
                {"size", "int_pair", "set_size"},
                {"pos",  "int_pair", "set_pos"},
            },
            {}
        });

        reg("LMenuSection", {
            true,
            {
                {"size", "int_pair", "set_size"},
                {"pos",  "int_pair", "set_pos"},
            },
            {}
        });

        reg("LTileView", {
            true,
            {
                {"size", "int_pair", "set_size"},
                {"pos",  "int_pair", "set_pos"},
            },
            {}
        });

        reg("LWin", {
            true,
            {
                {"title", "string",   "add_title"},
                {"size",  "int_pair", "set_size"},
                {"pos",   "int_pair", "set_pos"},
            },
            {}
        });

        reg("LMsgBox", {
            false,
            {
                {"title", "string",   "set_title"},
                {"text",  "string",   "set_text"},
                {"size",  "int_pair", "set_size"},
                {"pos",   "int_pair", "set_pos"},
            },
            {}
        });

        // ===== Rich Text =====

        reg("LSpan", {
            false,
            {
                {"spans",    "string",   "new_span", false, false, "", "spans"},
                {"mode",     "int",      "set_mode"},
                {"overflow", "int",      "set_overflow"},
                {"indent",   "int",      "set_indent"},
                {"size",     "int_pair", "set_size"},
                {"pos",      "int_pair", "set_pos"},
            },
            {}
        });

        // ===== Animation =====

        reg("LAnimImage", {
            false,
            {
                {"duration",     "int",      "set_duration"},
                {"repeat_count", "int",      "set_repeat_count"},
                {"size",         "int_pair", "set_size"},
                {"pos",          "int_pair", "set_pos"},
            },
            {}
        });

        reg("LLottie", {
            false,
            {
                {"src",   "string",   "set_src"},
                {"loop",  "bool",     "set_loop"},
                {"speed", "int",      "set_speed"},
                {"size",  "int_pair", "set_size"},
                {"pos",   "int_pair", "set_pos"},
            },
            {
                {"ready", "on_complete"},
            }
        });

        // ===== Layout Containers =====

        reg("LVBoxLayout", {
            true,
            {
                {"spacing",     "int",      "set_spacing"},
                {"padding",     "int",      "set_padding"},
                {"align_main",  "int",      "set_align_main"},
                {"align_cross", "int",      "set_align_cross"},
                {"size",        "int_pair", "set_size"},
                {"pos",         "int_pair", "set_pos"},
            },
            {}
        });

        reg("LHBoxLayout", {
            true,
            {
                {"spacing",     "int",      "set_spacing"},
                {"padding",     "int",      "set_padding"},
                {"align_main",  "int",      "set_align_main"},
                {"align_cross", "int",      "set_align_cross"},
                {"size",        "int_pair", "set_size"},
                {"pos",         "int_pair", "set_pos"},
            },
            {}
        });

        reg("LGridLayout", {
            true,
            {
                {"columns",      "int",      "set_columns"},
                {"rows",         "int",      "set_rows"},
                {"col_spacing",  "int",      "set_column_spacing"},
                {"row_spacing",  "int",      "set_row_spacing"},
                {"padding",      "int",      "set_padding"},
                {"size",         "int_pair", "set_size"},
                {"pos",          "int_pair", "set_pos"},
            },
            {}
        });

        reg("LStackedLayout", {
            true,
            {
                {"current_index", "int",      "set_current_index", false, true},
                {"padding",       "int",      "set_padding"},
                {"size",          "int_pair", "set_size"},
                {"pos",           "int_pair", "set_pos"},
            },
            {}
        });

        // ===== New Widgets =====

        reg("LComboBox", {
            false,
            {
                {"options",            "string",   "set_options"},
                {"selected",           "int",      "set_selected"},
                {"direction",          "int",      "set_direction"},
                {"symbol",             "string",   "set_symbol"},
                {"text",               "string",   "set_text"},
                {"selected_highlight", "bool",     "set_selected_highlight"},
                {"enabled",            "bool",     "set_enabled"},
                {"size",               "int_pair", "set_size"},
                {"pos",                "int_pair", "set_pos"},
            },
            {
                {"value_changed", "on_change"},
            }
        });

        reg("LLineEdit", {
            false,
            {
                {"text",              "string",   "set_text"},
                {"placeholder",       "string",   "set_placeholder"},
                {"password_mode",     "bool",     "set_password_mode"},
                {"password_show_time","int",      "set_password_show_time"},
                {"max_length",        "int",      "set_max_length"},
                {"accepted_chars",    "string",   "set_accepted_chars"},
                {"cursor_click_pos",  "bool",     "set_cursor_click_pos"},
                {"text_selection",    "bool",     "set_text_selection"},
                {"enabled",           "bool",     "set_enabled"},
                {"size",              "int_pair", "set_size"},
                {"pos",               "int_pair", "set_pos"},
            },
            {
                {"value_changed", "on_change"},
                {"ready",         "on_ready"},
            }
        });

        reg("LTextEdit", {
            false,
            {
                {"text",              "string",   "set_text"},
                {"placeholder",       "string",   "set_placeholder"},
                {"password_mode",     "bool",     "set_password_mode"},
                {"password_show_time","int",      "set_password_show_time"},
                {"max_length",        "int",      "set_max_length"},
                {"accepted_chars",    "string",   "set_accepted_chars"},
                {"cursor_click_pos",  "bool",     "set_cursor_click_pos"},
                {"text_selection",    "bool",     "set_text_selection"},
                {"enabled",           "bool",     "set_enabled"},
                {"size",              "int_pair", "set_size"},
                {"pos",               "int_pair", "set_pos"},
            },
            {
                {"value_changed", "on_change"},
                {"ready",         "on_ready"},
            }
        });

        reg("LRadioButton", {
            false,
            {
                {"text",    "string",   "set_text"},
                {"checked", "bool",     "set_checked"},
                {"size",    "int_pair", "set_size"},
                {"pos",     "int_pair", "set_pos"},
            },
            {
                {"value_changed", "on_change"},
            }
        });

        reg("LGroupBox", {
            true,
            {
                {"title",   "string",   "set_title"},
                {"padding", "int",      "set_padding"},
                {"size",    "int_pair", "set_size"},
                {"pos",     "int_pair", "set_pos"},
            },
            {}
        });
    }

    const component_def_t* find(const std::string& name) const {
        auto it = m_components.find(name);
        return it != m_components.end() ? &it->second : nullptr;
    }

    const std::map<std::string, component_def_t>& all() const {
        return m_components;
    }
};

} // namespace luic
