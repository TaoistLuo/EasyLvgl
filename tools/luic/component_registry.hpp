#pragma once
#include <string>
#include <vector>
#include <map>

namespace luic {

struct prop_def_t {
    std::string name;
    std::string type;     // "string", "int", "hex", "int_pair", "bool"
    std::string method;   // C++ method name, e.g. "text", "size", "range"
    bool is_theme = false;
};

struct event_def_t {
    std::string name;     // "clicked", "value_changed"
    std::string method;   // "on_click", "on_change"
};

struct component_def_t {
    std::string class_name;
    bool is_container = false;
    std::vector<prop_def_t> props;
    std::vector<event_def_t> events;

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
            .is_container = false,
            .props = {
                {"text",     "string",   "set_text"},
                {"size",     "int_pair", "set_size"},
                {"pos",      "int_pair", "set_pos"},
                {"disabled", "bool",     "set_disabled"},
                {"theme",    "string",   "", true},
            },
            .events = {
                {"clicked",  "on_click"},
                {"pressed",  "on_click"},
                {"released", "on_click"},
            }
        });

        reg("LLabel", {
            .is_container = false,
            .props = {
                {"text",      "string",   "set_text"},
                {"color",     "hex",      "set_color"},
                {"font_size", "int",      "set_font_size"},
                {"long_mode", "int",      "set_long_mode"},
                {"size",      "int_pair", "set_size"},
                {"pos",       "int_pair", "set_pos"},
            },
            .events = {}
        });

        reg("LSlider", {
            .is_container = false,
            .props = {
                {"range",      "int_pair", "set_range"},
                {"value",      "int",      "set_value"},
                {"left_value", "int",      "set_left_value"},
                {"mode",       "int",      "set_mode"},
                {"size",       "int_pair", "set_size"},
                {"pos",        "int_pair", "set_pos"},
            },
            .events = {
                {"value_changed", "on_change"},
            }
        });

        reg("LCheckbox", {
            .is_container = false,
            .props = {
                {"text",    "string",   "set_text"},
                {"checked", "bool",     "set_checked"},
                {"size",    "int_pair", "set_size"},
                {"pos",     "int_pair", "set_pos"},
            },
            .events = {
                {"value_changed", "on_change"},
            }
        });

        reg("LSwitch", {
            .is_container = false,
            .props = {
                {"checked",  "bool",     "set_checked"},
                {"enabled",  "bool",     "set_enabled"},
                {"size",     "int_pair", "set_size"},
                {"pos",      "int_pair", "set_pos"},
            },
            .events = {
                {"value_changed", "on_change"},
            }
        });

        // ===== Display Widgets =====

        reg("LImage", {
            .is_container = false,
            .props = {
                {"src",       "string",   "set_src"},
                {"rotation",  "int",      "set_rotation"},
                {"scale",     "int",      "set_scale"},
                {"pivot",     "int_pair", "set_pivot"},
                {"offset",    "int_pair", "set_offset"},
                {"opacity",   "int",      "set_opacity"},
                {"antialias", "bool",     "set_antialias"},
                {"size",      "int_pair", "set_size"},
                {"pos",       "int_pair", "set_pos"},
            },
            .events = {
                {"clicked", "on_click"},
            }
        });

        reg("LArc", {
            .is_container = false,
            .props = {
                {"range",       "int_pair", "set_range"},
                {"value",       "int",      "set_value"},
                {"rotation",    "int",      "set_rotation"},
                {"mode",        "int",      "set_mode"},
                {"bg_angles",   "int_pair", "set_bg_angles"},
                {"angles",      "int_pair", "set_angles"},
                {"change_rate", "int",      "set_change_rate"},
                {"knob_enabled","bool",     "set_knob_enabled"},
                {"size",        "int",      "set_size"},
                {"pos",         "int_pair", "set_pos"},
            },
            .events = {
                {"value_changed", "on_change"},
            }
        });

        reg("LBar", {
            .is_container = false,
            .props = {
                {"range",       "int_pair", "set_range"},
                {"value",       "int",      "set_value"},
                {"start_value", "int",      "set_start_value"},
                {"mode",        "int",      "set_mode"},
                {"orientation", "bool",     "set_orientation"},
                {"size",        "int_pair", "set_size"},
                {"pos",         "int_pair", "set_pos"},
            },
            .events = {}
        });

        reg("LSpinner", {
            .is_container = false,
            .props = {
                {"color", "hex",      "set_color"},
                {"size",  "int",      "set_size"},
                {"pos",   "int_pair", "set_pos"},
            },
            .events = {}
        });

        reg("LLed", {
            .is_container = false,
            .props = {
                {"color",      "hex",      "set_color"},
                {"brightness", "int",      "set_brightness"},
                {"on",         "bool",     "on"},
                {"size",       "int",      "set_size"},
                {"pos",        "int_pair", "set_pos"},
            },
            .events = {}
        });

        reg("LLine", {
            .is_container = false,
            .props = {
                {"width",    "int",      "set_width"},
                {"color",    "hex",      "set_color"},
                {"y_invert", "bool",     "set_y_invert"},
                {"size",     "int_pair", "set_size"},
                {"pos",      "int_pair", "set_pos"},
            },
            .events = {}
        });

        // ===== Input Widgets =====

        reg("LRoller", {
            .is_container = false,
            .props = {
                {"options",           "string",   "set_options"},
                {"selected",          "int",      "set_selected"},
                {"visible_row_count", "int",      "set_visible_row_count"},
                {"size",              "int_pair", "set_size"},
                {"pos",               "int_pair", "set_pos"},
            },
            .events = {
                {"value_changed", "on_change"},
            }
        });

        reg("LSpinbox", {
            .is_container = false,
            .props = {
                {"range",        "int_pair", "set_range"},
                {"value",        "int",      "set_value"},
                {"step",         "int",      "set_step"},
                {"digit_format", "int_pair", "set_digit_format"},
                {"rollover",     "bool",     "set_rollover"},
                {"size",         "int_pair", "set_size"},
                {"pos",          "int_pair", "set_pos"},
            },
            .events = {
                {"value_changed", "on_change"},
            }
        });

        reg("LButtonMatrix", {
            .is_container = false,
            .props = {
                {"one_checked", "bool",     "set_one_checked"},
                {"size",        "int_pair", "set_size"},
                {"pos",         "int_pair", "set_pos"},
            },
            .events = {
                {"value_changed", "on_value_changed"},
            }
        });

        reg("LKeyboard", {
            .is_container = false,
            .props = {
                {"mode",     "int",      "set_mode"},
                {"popovers", "bool",     "set_popovers"},
                {"size",     "int_pair", "set_size"},
                {"pos",      "int_pair", "set_pos"},
            },
            .events = {}
        });

        reg("LImageButton", {
            .is_container = false,
            .props = {
                {"size", "int_pair", "set_size"},
                {"pos",  "int_pair", "set_pos"},
            },
            .events = {
                {"clicked", "on_click"},
            }
        });

        // ===== Data Display Widgets =====

        reg("LTable", {
            .is_container = false,
            .props = {
                {"row_count",    "int",      "set_row_count"},
                {"column_count", "int",      "set_column_count"},
                {"size",         "int_pair", "set_size"},
                {"pos",          "int_pair", "set_pos"},
            },
            .events = {}
        });

        reg("LChart", {
            .is_container = false,
            .props = {
                {"type",           "int",      "set_type"},
                {"point_count",    "int",      "set_point_count"},
                {"div_line_count", "int_pair", "set_div_line_count"},
                {"update_mode",    "int",      "set_update_mode"},
                {"size",           "int_pair", "set_size"},
                {"pos",            "int_pair", "set_pos"},
            },
            .events = {}
        });

        reg("LScale", {
            .is_container = false,
            .props = {
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
            .events = {}
        });

        reg("LCalendar", {
            .is_container = false,
            .props = {
                {"today_date",  "int_triple", "set_today_date"},
                {"showed_date", "int_pair",   "set_showed_date"},
                {"size",        "int_pair",   "set_size"},
                {"pos",         "int_pair",   "set_pos"},
            },
            .events = {}
        });

        reg("LCanvas", {
            .is_container = false,
            .props = {
                {"size", "int_pair", "set_size"},
                {"pos",  "int_pair", "set_pos"},
            },
            .events = {}
        });

        // ===== Container Widgets =====

        reg("LList", {
            .is_container = true,
            .props = {
                {"size", "int_pair", "set_size"},
                {"pos",  "int_pair", "set_pos"},
            },
            .events = {}
        });

        reg("LTabView", {
            .is_container = true,
            .props = {
                {"tab_bar_position", "int",      "set_tab_bar_position"},
                {"tab_bar_size",     "int",      "set_tab_bar_size"},
                {"size",             "int_pair", "set_size"},
                {"pos",              "int_pair", "set_pos"},
            },
            .events = {}
        });

        reg("LMenu", {
            .is_container = true,
            .props = {
                {"mode_header",           "int",  "set_mode_header"},
                {"mode_root_back_button", "int",  "set_mode_root_back_button"},
                {"size",                  "int_pair", "set_size"},
                {"pos",                   "int_pair", "set_pos"},
            },
            .events = {}
        });

        reg("LTileView", {
            .is_container = true,
            .props = {
                {"size", "int_pair", "set_size"},
                {"pos",  "int_pair", "set_pos"},
            },
            .events = {}
        });

        reg("LWin", {
            .is_container = true,
            .props = {
                {"size", "int_pair", "set_size"},
                {"pos",  "int_pair", "set_pos"},
            },
            .events = {}
        });

        reg("LMsgBox", {
            .is_container = false,
            .props = {
                {"title", "string", "set_title"},
                {"text",  "string", "set_text"},
            },
            .events = {}
        });

        // ===== Rich Text =====

        reg("LSpan", {
            .is_container = false,
            .props = {
                {"mode",     "int",      "set_mode"},
                {"overflow", "int",      "set_overflow"},
                {"indent",   "int",      "set_indent"},
                {"size",     "int_pair", "set_size"},
                {"pos",      "int_pair", "set_pos"},
            },
            .events = {}
        });

        // ===== Animation =====

        reg("LAnimImage", {
            .is_container = false,
            .props = {
                {"duration",     "int",      "set_duration"},
                {"repeat_count", "int",      "set_repeat_count"},
                {"size",         "int_pair", "set_size"},
                {"pos",          "int_pair", "set_pos"},
            },
            .events = {}
        });

        reg("LLottie", {
            .is_container = false,
            .props = {
                {"src",   "string",   "set_src"},
                {"loop",  "bool",     "set_loop"},
                {"speed", "float",    "set_speed"},
                {"size",  "int_pair", "set_size"},
                {"pos",   "int_pair", "set_pos"},
            },
            .events = {
                {"ready", "on_complete"},
            }
        });

        // ===== Layout Containers =====

        reg("LVBoxLayout", {
            .is_container = true,
            .props = {
                {"spacing",     "int",      "set_spacing"},
                {"padding",     "int",      "set_padding"},
                {"align_main",  "int",      "set_align_main"},
                {"align_cross", "int",      "set_align_cross"},
                {"size",        "int_pair", "set_size"},
                {"pos",         "int_pair", "set_pos"},
            },
            .events = {}
        });

        reg("LHBoxLayout", {
            .is_container = true,
            .props = {
                {"spacing",     "int",      "set_spacing"},
                {"padding",     "int",      "set_padding"},
                {"align_main",  "int",      "set_align_main"},
                {"align_cross", "int",      "set_align_cross"},
                {"size",        "int_pair", "set_size"},
                {"pos",         "int_pair", "set_pos"},
            },
            .events = {}
        });

        reg("LGridLayout", {
            .is_container = true,
            .props = {
                {"columns",      "int",      "set_columns"},
                {"rows",         "int",      "set_rows"},
                {"col_spacing",  "int",      "set_column_spacing"},
                {"row_spacing",  "int",      "set_row_spacing"},
                {"size",         "int_pair", "set_size"},
                {"pos",          "int_pair", "set_pos"},
            },
            .events = {}
        });

        reg("LFormLayout", {
            .is_container = true,
            .props = {
                {"label_width",    "int",      "set_label_width"},
                {"row_spacing",     "int",      "set_row_spacing"},
                {"col_spacing",     "int",      "set_column_spacing"},
                {"size",        "int_pair", "set_size"},
                {"pos",         "int_pair", "set_pos"},
            },
            .events = {}
        });

        reg("LStackedLayout", {
            .is_container = true,
            .props = {
                {"current_index", "int",      "set_current_index"},
                {"padding",       "int",      "set_padding"},
                {"size",          "int_pair", "set_size"},
                {"pos",           "int_pair", "set_pos"},
            },
            .events = {}
        });

        // ===== New Widgets =====

        reg("LComboBox", {
            .is_container = false,
            .props = {
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
            .events = {
                {"value_changed", "on_change"},
            }
        });

        reg("LLineEdit", {
            .is_container = false,
            .props = {
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
            .events = {
                {"value_changed", "on_change"},
                {"ready",         "on_ready"},
            }
        });

        reg("LTextEdit", {
            .is_container = false,
            .props = {
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
            .events = {
                {"value_changed", "on_change"},
                {"ready",         "on_ready"},
            }
        });

        reg("LRadioButton", {
            .is_container = false,
            .props = {
                {"text",    "string",   "set_text"},
                {"checked", "bool",     "set_checked"},
                {"size",    "int_pair", "set_size"},
                {"pos",     "int_pair", "set_pos"},
            },
            .events = {
                {"value_changed", "on_change"},
            }
        });

        reg("LGroupBox", {
            .is_container = true,
            .props = {
                {"title",   "string",   "set_title"},
                {"padding", "int",      "set_padding"},
                {"size",    "int_pair", "set_size"},
                {"pos",     "int_pair", "set_pos"},
            },
            .events = {}
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
