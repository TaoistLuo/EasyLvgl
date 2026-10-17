#pragma once
#include <string>
#include <vector>
#include <QVariant>
#include <QString>

namespace designer {

struct PropSchema {
    std::string name;
    std::string type;
    QVariant defaultVal;
};

struct PropGroup {
    std::string groupName;
    std::vector<PropSchema> props;
};

struct ComponentSchema {
    static std::vector<PropGroup> propsFor(const std::string& type) {
        std::vector<PropGroup> groups;

        QVariantList pair0;
        pair0.append(QVariant(0));
        pair0.append(QVariant(0));
        QVariantList pair100;
        pair100.append(QVariant(0));
        pair100.append(QVariant(100));

        PropGroup obj;
        obj.groupName = "对象";
        obj.props.push_back({"x", "int", QVariant(0)});
        obj.props.push_back({"y", "int", QVariant(0)});
        obj.props.push_back({"width", "int", QVariant(0)});
        obj.props.push_back({"height", "int", QVariant(0)});
        obj.props.push_back({"align", "int", QVariant(0)});
        obj.props.push_back({"hidden", "bool", QVariant(false)});
        obj.props.push_back({"clickable", "bool", QVariant(true)});
        obj.props.push_back({"scrollable", "bool", QVariant(true)});
        obj.props.push_back({"scroll_dir", "int", QVariant(0)});
        obj.props.push_back({"scrollbar_mode", "int", QVariant(0)});
        obj.props.push_back({"layout", "int", QVariant(0)});
        obj.props.push_back({"ext_click_area", "int", QVariant(0)});
        groups.push_back(std::move(obj));

        PropGroup bg;
        bg.groupName = "背景";
        bg.props.push_back({"bg_color", "hex", QVariant(QString("0x000000"))});
        bg.props.push_back({"bg_opa", "int", QVariant(0)});
        bg.props.push_back({"bg_grad_color", "hex", QVariant(QString("0x000000"))});
        bg.props.push_back({"bg_grad_dir", "int", QVariant(0)});
        bg.props.push_back({"bg_main_stop", "int", QVariant(0)});
        bg.props.push_back({"bg_grad_stop", "int", QVariant(255)});
        bg.props.push_back({"bg_image_src", "string", QVariant(QString(""))});
        bg.props.push_back({"bg_image_opa", "int", QVariant(255)});
        bg.props.push_back({"bg_image_recolor", "hex", QVariant(QString("0x000000"))});
        bg.props.push_back({"bg_image_recolor_opa", "int", QVariant(0)});
        bg.props.push_back({"bg_image_tiled", "bool", QVariant(false)});
        groups.push_back(std::move(bg));

        PropGroup border;
        border.groupName = "边框";
        border.props.push_back({"border_color", "hex", QVariant(QString("0x000000"))});
        border.props.push_back({"border_width", "int", QVariant(0)});
        border.props.push_back({"border_opa", "int", QVariant(255)});
        border.props.push_back({"border_side", "int", QVariant(15)});
        border.props.push_back({"border_post", "bool", QVariant(false)});
        groups.push_back(std::move(border));

        PropGroup outline;
        outline.groupName = "轮廓";
        outline.props.push_back({"outline_color", "hex", QVariant(QString("0x000000"))});
        outline.props.push_back({"outline_width", "int", QVariant(0)});
        outline.props.push_back({"outline_opa", "int", QVariant(255)});
        outline.props.push_back({"outline_pad", "int", QVariant(0)});
        groups.push_back(std::move(outline));

        PropGroup shadow;
        shadow.groupName = "阴影";
        shadow.props.push_back({"shadow_color", "hex", QVariant(QString("0x000000"))});
        shadow.props.push_back({"shadow_width", "int", QVariant(0)});
        shadow.props.push_back({"shadow_ofs_x", "int", QVariant(0)});
        shadow.props.push_back({"shadow_ofs_y", "int", QVariant(0)});
        shadow.props.push_back({"shadow_spread", "int", QVariant(0)});
        shadow.props.push_back({"shadow_opa", "int", QVariant(255)});
        groups.push_back(std::move(shadow));

        PropGroup padding;
        padding.groupName = "内边距";
        padding.props.push_back({"pad_top", "int", QVariant(0)});
        padding.props.push_back({"pad_bottom", "int", QVariant(0)});
        padding.props.push_back({"pad_left", "int", QVariant(0)});
        padding.props.push_back({"pad_right", "int", QVariant(0)});
        padding.props.push_back({"pad_row", "int", QVariant(0)});
        padding.props.push_back({"pad_column", "int", QVariant(0)});
        groups.push_back(std::move(padding));

        PropGroup margin;
        margin.groupName = "外边距";
        margin.props.push_back({"margin_top", "int", QVariant(0)});
        margin.props.push_back({"margin_bottom", "int", QVariant(0)});
        margin.props.push_back({"margin_left", "int", QVariant(0)});
        margin.props.push_back({"margin_right", "int", QVariant(0)});
        groups.push_back(std::move(margin));

        PropGroup sizeConstraints;
        sizeConstraints.groupName = "尺寸约束";
        sizeConstraints.props.push_back({"min_width", "int", QVariant(0)});
        sizeConstraints.props.push_back({"max_width", "int", QVariant(0)});
        sizeConstraints.props.push_back({"min_height", "int", QVariant(0)});
        sizeConstraints.props.push_back({"max_height", "int", QVariant(0)});
        groups.push_back(std::move(sizeConstraints));

        PropGroup misc;
        misc.groupName = "外观";
        misc.props.push_back({"radius", "int", QVariant(0)});
        misc.props.push_back({"clip_corner", "bool", QVariant(false)});
        misc.props.push_back({"opa", "int", QVariant(255)});
        misc.props.push_back({"blend_mode", "int", QVariant(0)});
        misc.props.push_back({"base_dir", "int", QVariant(0)});
        groups.push_back(std::move(misc));

        const bool hasText = type != "LImage" && type != "LCanvas"
                          && type != "LAnimImage" && type != "LLottie";
        if (hasText) {
        PropGroup text;
        text.groupName = "文本样式";
        text.props.push_back({"text_color", "hex", QVariant(QString("0xFFFFFF"))});
        text.props.push_back({"text_opa", "int", QVariant(255)});
        text.props.push_back({"text_font_size", "int", QVariant(14)});
        text.props.push_back({"text_letter_space", "int", QVariant(0)});
        text.props.push_back({"text_line_space", "int", QVariant(0)});
        text.props.push_back({"text_align", "int", QVariant(0)});
        text.props.push_back({"text_decor", "int", QVariant(0)});
        groups.push_back(std::move(text));
        }

        PropGroup transform;
        transform.groupName = "变换";
        transform.props.push_back({"transform_rotation", "int", QVariant(0)});
        transform.props.push_back({"transform_scale_x", "int", QVariant(256)});
        transform.props.push_back({"transform_scale_y", "int", QVariant(256)});
        transform.props.push_back({"transform_pivot_x", "int", QVariant(0)});
        transform.props.push_back({"transform_pivot_y", "int", QVariant(0)});
        transform.props.push_back({"translate_x", "int", QVariant(0)});
        transform.props.push_back({"translate_y", "int", QVariant(0)});
        transform.props.push_back({"transform_skew_x", "int", QVariant(0)});
        transform.props.push_back({"transform_skew_y", "int", QVariant(0)});
        groups.push_back(std::move(transform));

        // line_* 只对真正画线的对象有效（lv_line 等），对按钮之类的对象设置后不会有任何视觉变化
        if (type == "LLine") {
            PropGroup lineStyle;
            lineStyle.groupName = "线条样式";
            lineStyle.props.push_back({"line_color", "hex", QVariant(QString("0xFFFFFF"))});
            lineStyle.props.push_back({"line_width", "int", QVariant(1)});
            lineStyle.props.push_back({"line_opa", "int", QVariant(255)});
            lineStyle.props.push_back({"line_dash_width", "int", QVariant(0)});
            lineStyle.props.push_back({"line_dash_gap", "int", QVariant(0)});
            lineStyle.props.push_back({"line_rounded", "bool", QVariant(false)});
            groups.push_back(std::move(lineStyle));
        }

        if (type == "LArc") {
            PropGroup arcStyle;
            arcStyle.groupName = "圆弧样式";
            arcStyle.props.push_back({"arc_color", "hex", QVariant(QString("0xFFFFFF"))});
            arcStyle.props.push_back({"arc_width", "int", QVariant(2)});
            arcStyle.props.push_back({"arc_opa", "int", QVariant(255)});
            arcStyle.props.push_back({"arc_rounded", "bool", QVariant(false)});
            groups.push_back(std::move(arcStyle));
        }

        if (type == "LImage" || type == "LImageButton" || type == "LAnimImage") {
            PropGroup imgStyle;
            imgStyle.groupName = "图片样式";
            imgStyle.props.push_back({"image_opa", "int", QVariant(255)});
            imgStyle.props.push_back({"image_recolor", "hex", QVariant(QString("0x000000"))});
            imgStyle.props.push_back({"image_recolor_opa", "int", QVariant(0)});
            groups.push_back(std::move(imgStyle));
        }

        PropGroup comp;
        comp.groupName = type;

        if (type == "LButton") {
            comp.props.push_back({"text", "string", QVariant(QString("Button"))});
            comp.props.push_back({"disabled", "bool", QVariant(false)});
            comp.props.push_back({"theme", "string", QVariant(QString("primary"))});
        } else if (type == "LLabel") {
            comp.props.push_back({"text", "string", QVariant(QString("Label"))});
            comp.props.push_back({"color", "hex", QVariant(QString("0xFFFFFF"))});
            comp.props.push_back({"font_size", "int", QVariant(14)});
            comp.props.push_back({"long_mode", "int", QVariant(0)});
        } else if (type == "LSlider") {
            comp.props.push_back({"range", "int_pair", QVariant(pair100)});
            comp.props.push_back({"value", "int", QVariant(50)});
            comp.props.push_back({"left_value", "int", QVariant(0)});
            comp.props.push_back({"mode", "int", QVariant(0)});
        } else if (type == "LCheckbox") {
            comp.props.push_back({"text", "string", QVariant(QString("Checkbox"))});
            comp.props.push_back({"checked", "bool", QVariant(false)});
        } else if (type == "LRadioButton") {
            comp.props.push_back({"text", "string", QVariant(QString("Radio"))});
            comp.props.push_back({"checked", "bool", QVariant(false)});
        } else if (type == "LSwitch") {
            comp.props.push_back({"checked", "bool", QVariant(false)});
            comp.props.push_back({"enabled", "bool", QVariant(true)});
        } else if (type == "LComboBox") {
            comp.props.push_back({"options", "string", QVariant(QString("Option 1\\nOption 2\\nOption 3"))});
            comp.props.push_back({"selected", "int", QVariant(0)});
            comp.props.push_back({"direction", "int", QVariant(0)});
            comp.props.push_back({"symbol", "string", QVariant(QString(""))});
            comp.props.push_back({"text", "string", QVariant(QString(""))});
            comp.props.push_back({"selected_highlight", "bool", QVariant(true)});
            comp.props.push_back({"enabled", "bool", QVariant(true)});
        } else if (type == "LLineEdit") {
            comp.props.push_back({"text", "string", QVariant(QString(""))});
            comp.props.push_back({"placeholder", "string", QVariant(QString("Enter text..."))});
            comp.props.push_back({"password_mode", "bool", QVariant(false)});
            comp.props.push_back({"password_show_time", "int", QVariant(1500)});
            comp.props.push_back({"max_length", "int", QVariant(0)});
            comp.props.push_back({"accepted_chars", "string", QVariant(QString(""))});
            comp.props.push_back({"cursor_click_pos", "bool", QVariant(true)});
            comp.props.push_back({"text_selection", "bool", QVariant(false)});
            comp.props.push_back({"enabled", "bool", QVariant(true)});
        } else if (type == "LTextEdit") {
            comp.props.push_back({"text", "string", QVariant(QString(""))});
            comp.props.push_back({"placeholder", "string", QVariant(QString("Enter text..."))});
            comp.props.push_back({"password_mode", "bool", QVariant(false)});
            comp.props.push_back({"password_show_time", "int", QVariant(1500)});
            comp.props.push_back({"max_length", "int", QVariant(0)});
            comp.props.push_back({"accepted_chars", "string", QVariant(QString(""))});
            comp.props.push_back({"cursor_click_pos", "bool", QVariant(true)});
            comp.props.push_back({"text_selection", "bool", QVariant(false)});
            comp.props.push_back({"enabled", "bool", QVariant(true)});
        } else if (type == "LImage") {
            comp.props.push_back({"src", "string", QVariant(QString(""))});
            comp.props.push_back({"rotation", "int", QVariant(0)});
            comp.props.push_back({"scale", "int", QVariant(256)});
            comp.props.push_back({"scale_x", "int", QVariant(256)});
            comp.props.push_back({"scale_y", "int", QVariant(256)});
            comp.props.push_back({"pivot", "int_pair", QVariant(pair0)});
            comp.props.push_back({"offset", "int_pair", QVariant(pair0)});
            comp.props.push_back({"antialias", "bool", QVariant(true)});
            comp.props.push_back({"inner_align", "int", QVariant(0)});
            comp.props.push_back({"blend_mode", "int", QVariant(0)});
            comp.props.push_back({"opacity", "int", QVariant(255)});
        } else if (type == "LArc") {
            comp.props.push_back({"range", "int_pair", QVariant(pair100)});
            comp.props.push_back({"value", "int", QVariant(0)});
            comp.props.push_back({"rotation", "int", QVariant(0)});
            comp.props.push_back({"mode", "int", QVariant(0)});
            QVariantList angles360;
            angles360.append(QVariant(0));
            angles360.append(QVariant(360));
            comp.props.push_back({"bg_angles", "int_pair", QVariant(angles360)});
            comp.props.push_back({"angles", "int_pair", QVariant(angles360)});
            comp.props.push_back({"change_rate", "int", QVariant(0)});
            comp.props.push_back({"knob_offset", "int", QVariant(0)});
        } else if (type == "LBar") {
            comp.props.push_back({"range", "int_pair", QVariant(pair100)});
            comp.props.push_back({"value", "int", QVariant(0)});
            comp.props.push_back({"start_value", "int", QVariant(0)});
            comp.props.push_back({"mode", "int", QVariant(0)});
            comp.props.push_back({"orientation", "int", QVariant(0)});
        } else if (type == "LSpinner") {
            comp.props.push_back({"color", "hex", QVariant(QString("0x2196F3"))});
            comp.props.push_back({"anim_speed", "int", QVariant(1000)});
            comp.props.push_back({"anim_period", "int", QVariant(1000)});
        } else if (type == "LLed") {
            comp.props.push_back({"color", "hex", QVariant(QString("0x00FF00"))});
            comp.props.push_back({"brightness", "int", QVariant(255)});
            comp.props.push_back({"on", "bool", QVariant(true)});
        } else if (type == "LLine") {
            comp.props.push_back({"points", "string", QVariant(QString("0,0;80,40;160,0"))});
            comp.props.push_back({"width", "int", QVariant(2)});
            comp.props.push_back({"color", "hex", QVariant(QString("0xFFFFFF"))});
            comp.props.push_back({"y_invert", "bool", QVariant(false)});
            comp.props.push_back({"dash_width", "int", QVariant(0)});
            comp.props.push_back({"dash_gap", "int", QVariant(0)});
            comp.props.push_back({"rounded", "bool", QVariant(false)});
        } else if (type == "LRoller") {
            comp.props.push_back({"options", "string", QVariant(QString("Item 1\\nItem 2\\nItem 3"))});
            comp.props.push_back({"selected", "int", QVariant(0)});
            comp.props.push_back({"visible_row_count", "int", QVariant(3)});
        } else if (type == "LSpinbox") {
            comp.props.push_back({"range", "int_pair", QVariant(pair100)});
            comp.props.push_back({"value", "int", QVariant(0)});
            comp.props.push_back({"step", "int", QVariant(1)});
            QVariantList df;
            df.append(QVariant(3));
            df.append(QVariant(0));
            comp.props.push_back({"digit_format", "int_pair", QVariant(df)});
            comp.props.push_back({"rollover", "bool", QVariant(false)});
            comp.props.push_back({"cursor_pos", "int", QVariant(0)});
            comp.props.push_back({"digit_step_direction", "int", QVariant(0)});
        } else if (type == "LButtonMatrix") {
            comp.props.push_back({"map", "string", QVariant(QString("A B C\\nD E F"))});
            comp.props.push_back({"selected", "int", QVariant(0)});
            comp.props.push_back({"one_checked", "bool", QVariant(false)});
        } else if (type == "LKeyboard") {
            comp.props.push_back({"textarea", "string", QVariant(QString(""))});
            comp.props.push_back({"mode", "int", QVariant(0)});
            comp.props.push_back({"popovers", "bool", QVariant(false)});
        } else if (type == "LTable") {
            comp.props.push_back({"row_count", "int", QVariant(3)});
            comp.props.push_back({"column_count", "int", QVariant(3)});
            comp.props.push_back({"column_widths", "string", QVariant(QString(""))});
            comp.props.push_back({"data", "string", QVariant(QString("A|B|C\\nD|E|F"))});
        } else if (type == "LChart") {
            comp.props.push_back({"type", "int", QVariant(0)});
            comp.props.push_back({"point_count", "int", QVariant(10)});
            QVariantList divLines;
            divLines.append(QVariant(3));
            divLines.append(QVariant(5));
            comp.props.push_back({"div_line_count", "int_pair", QVariant(divLines)});
            comp.props.push_back({"update_mode", "int", QVariant(0)});
            comp.props.push_back({"series", "string", QVariant(QString("10,20,30,20,10"))});
        } else if (type == "LScale") {
            comp.props.push_back({"mode", "int", QVariant(0)});
            comp.props.push_back({"range", "int_pair", QVariant(pair100)});
            comp.props.push_back({"total_tick_count", "int", QVariant(11)});
            comp.props.push_back({"major_tick_every", "int", QVariant(5)});
            comp.props.push_back({"rotation", "int", QVariant(0)});
            comp.props.push_back({"angle_range", "int", QVariant(270)});
            comp.props.push_back({"label_show", "bool", QVariant(true)});
            comp.props.push_back({"draw_ticks_on_top", "bool", QVariant(false)});
        } else if (type == "LCalendar") {
            QVariantList today;
            today.append(QVariant(2026));
            today.append(QVariant(1));
            today.append(QVariant(1));
            comp.props.push_back({"today_date", "int_triple", QVariant(today)});
            QVariantList showed;
            showed.append(QVariant(2026));
            showed.append(QVariant(1));
            comp.props.push_back({"showed_date", "int_pair", QVariant(showed)});
        } else if (type == "LGroupBox") {
            comp.props.push_back({"title", "string", QVariant(QString("Group"))});
            comp.props.push_back({"padding", "int", QVariant(10)});
        } else if (type == "LList") {
            comp.props.push_back({"items", "string", QVariant(QString("#Header\\nItem 1\\nItem 2"))});
        } else if (type == "LTileView") {
        } else if (type == "LTile") {
            comp.props.push_back({"col", "int", QVariant(0)});
            comp.props.push_back({"row", "int", QVariant(0)});
            comp.props.push_back({"dir", "int", QVariant(2)});
        } else if (type == "LCanvas") {
        } else if (type == "LWin") {
            comp.props.push_back({"title", "string", QVariant(QString("Window"))});
        } else if (type == "LVBoxLayout") {
            comp.props.push_back({"spacing", "int", QVariant(10)});
            comp.props.push_back({"padding", "int", QVariant(10)});
            comp.props.push_back({"align_cross", "int", QVariant(0)});
        } else if (type == "LHBoxLayout") {
            comp.props.push_back({"spacing", "int", QVariant(10)});
            comp.props.push_back({"padding", "int", QVariant(10)});
            comp.props.push_back({"align_cross", "int", QVariant(0)});
        } else if (type == "LGridLayout") {
            comp.props.push_back({"columns", "int", QVariant(2)});
            comp.props.push_back({"rows", "int", QVariant(2)});
            comp.props.push_back({"col_spacing", "int", QVariant(5)});
            comp.props.push_back({"row_spacing", "int", QVariant(5)});
            comp.props.push_back({"padding", "int", QVariant(10)});
        } else if (type == "LStackedLayout") {
            comp.props.push_back({"current_index", "int", QVariant(0)});
            comp.props.push_back({"padding", "int", QVariant(6)});
        } else if (type == "LSpan") {
            comp.props.push_back({"spans", "string", QVariant(QString("Span one\\nSpan two"))});
            comp.props.push_back({"mode", "int", QVariant(0)});
            comp.props.push_back({"overflow", "int", QVariant(0)});
            comp.props.push_back({"indent", "int", QVariant(0)});
        } else if (type == "LLottie") {
            comp.props.push_back({"src", "string", QVariant(QString(""))});
            comp.props.push_back({"loop", "bool", QVariant(true)});
            comp.props.push_back({"speed", "int", QVariant(1)});
        } else if (type == "LMsgBox") {
            comp.props.push_back({"title", "string", QVariant(QString("Title"))});
            comp.props.push_back({"text", "string", QVariant(QString("Message"))});
        } else if (type == "LAnimImage") {
            comp.props.push_back({"duration", "int", QVariant(1000)});
            comp.props.push_back({"repeat_count", "int", QVariant(0)});
        } else if (type == "LTabView") {
            comp.props.push_back({"tabs", "string", QVariant(QString("Tab 1\\nTab 2"))});
            comp.props.push_back({"tab_bar_position", "int", QVariant(0)});
            comp.props.push_back({"tab_bar_size", "int", QVariant(40)});
        } else if (type == "LMenu") {
            comp.props.push_back({"mode_header", "int", QVariant(0)});
            comp.props.push_back({"mode_root_back_button", "int", QVariant(0)});
            comp.props.push_back({"active_page", "string", QVariant(QString(""))});
        } else if (type == "LMenuPage") {
            comp.props.push_back({"title", "string", QVariant(QString("Page"))});
        } else if (type == "LMenuCont" || type == "LMenuSection") {
        } else if (type == "LImageButton") {
            comp.props.push_back({"state", "int", QVariant(0)});
            comp.props.push_back({"src_released", "string", QVariant(QString(""))});
            comp.props.push_back({"src_pressed", "string", QVariant(QString(""))});
            comp.props.push_back({"src_disabled", "string", QVariant(QString(""))});
        }

        if (!comp.props.empty()) {
            groups.insert(groups.begin() + 1, std::move(comp));
        }

        return groups;
    }
};

}
