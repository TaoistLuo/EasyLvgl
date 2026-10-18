#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
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
        bg.props.push_back({"bg_opa", "int", QVariant(255)});   // LVGL 主题对普通对象/按钮实际生效值为 255
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

        // text_* 是会被子控件继承的样式；对自身无文本、也无文本子控件的基础控件是空操作
        static const std::set<std::string> noText = {
            "LImage", "LCanvas", "LAnimImage", "LLottie",
            "LLine", "LLed", "LBar", "LArc", "LSpinner", "LSlider", "LSwitch",
            "LImageButton", "LChart"
        };
        const bool hasText = noText.find(type) == noText.end();
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
            /* 设计器画布是浅色底（#F5F5F5 / #FFFFFF），白字会完全看不见 */
            comp.props.push_back({"color", "hex", QVariant(QString("0x000000"))});
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
            comp.props.push_back({"direction", "int", QVariant(8)});
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
            comp.props.push_back({"orientation", "bool", QVariant(false)});
        } else if (type == "LSpinner") {
            comp.props.push_back({"color", "hex", QVariant(QString("0x2196F3"))});
                    } else if (type == "LLed") {
            comp.props.push_back({"color", "hex", QVariant(QString("0x00FF00"))});
            comp.props.push_back({"brightness", "int", QVariant(255)});
            comp.props.push_back({"on", "bool", QVariant(true)});
        } else if (type == "LLine") {
            comp.props.push_back({"points", "string", QVariant(QString("0,0;80,40;160,0"))});
            comp.props.push_back({"width", "int", QVariant(2)});
            comp.props.push_back({"color", "hex", QVariant(QString("0x000000"))});
            comp.props.push_back({"y_invert", "bool", QVariant(false)});
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

        /* 这些属性在对应组件上没有效果（LVGL 无该绘制路径，或由布局器接管），
         * 继续显示会让人以为“改了没反应”，因此不显示。 */
        static const std::map<std::string, std::set<std::string>> kHidden = [] {
            const std::set<std::string> transformAll = {
                "transform_rotation", "transform_scale_x", "transform_scale_y",
                "transform_pivot_x", "transform_pivot_y",
                "transform_skew_x", "transform_skew_y", "translate_x", "translate_y"};
            std::map<std::string, std::set<std::string>> m;
            /* 无内容可变换：LVGL 9.2 在“无内容对象 + transform”下绘制会死循环 */
            for (const char* t : {"LCanvas", "LTile", "LAnimImage"})
                m[t].insert(transformAll.begin(), transformAll.end());
            /* 非 0 混合模式在这些对象上会死循环 */
            for (const char* t : {"LSpan", "LCanvas", "LTile", "LAnimImage"})
                m[t].insert("blend_mode");
            /* 布局器由 spacing / row_spacing / col_spacing 控制，pad_row/pad_column 会被覆盖 */
            for (const char* t : {"LVBoxLayout", "LHBoxLayout", "LGridLayout"}) {
                m[t].insert("pad_row");
                m[t].insert("pad_column");
            }
            /* 菜单页内的容器位置由菜单布局决定，坐标/对齐无效；
             * 图块由 tileview 按 col/row 定位，坐标/尺寸同样无效 */
            for (const char* t : {"LMenuCont", "LMenuSection", "LTile"}) {
                m[t].insert("x"); m[t].insert("y"); m[t].insert("align");
            }
            m["LTile"].insert("width");
            m["LTile"].insert("height");
            /* 正方形控件（宽高始终一致），高度字段无意义 */
            for (const char* t : {"LArc", "LLed", "LSpinner"}) m[t].insert("height");
            /* 标签的文本颜色/字号由组件属性 color / font_size 提供，避免两行重复 */
            m["LLabel"].insert("text_color");
            m["LLabel"].insert("text_font_size");
            /* 通用 layout 只把布局类切成 flex，却不设排布方向，实测无效果；
             * 容器排布请使用 LVBoxLayout / LHBoxLayout / LGridLayout 组件 */
            for (const char* t : {"LAnimImage","LArc","LBar","LButton","LButtonMatrix",
                                  "LCalendar","LCanvas","LChart","LCheckbox","LComboBox",
                                  "LGridLayout","LGroupBox","LHBoxLayout","LImage","LImageButton",
                                  "LKeyboard","LLabel","LLed","LLine","LLineEdit","LList","LLottie",
                                  "LMenu","LMenuCont","LMenuPage","LMenuSection","LMsgBox",
                                  "LRadioButton","LRoller","LScale","LSlider","LSpan","LSpinbox",
                                  "LSpinner","LStackedLayout","LSwitch","LTable","LTabView",
                                  "LTextEdit","LTile","LTileView","LVBoxLayout","LWin"})
                m[t].insert("layout");
            /* 日历/滚轮自行绘制文字，text_align 无效果 */
            m["LCalendar"].insert("text_align");
            m["LRoller"].insert("text_align");
            return m;
        }();
        auto hit = kHidden.find(type);
        if (hit != kHidden.end()) {
            for (auto& g : groups) {
                std::vector<PropSchema> kept;
                for (auto& p : g.props)
                    if (!hit->second.count(p.name)) kept.push_back(p);
                g.props = std::move(kept);
            }
            groups.erase(std::remove_if(groups.begin(), groups.end(),
                                        [](const PropGroup& g) { return g.props.empty(); }),
                         groups.end());
        }

        return groups;
    }
};
/* ---------------- 枚举属性的可选项 ----------------
 * 这些属性的取值是 LVGL 的枚举，直接让用户填数字既容易错也不直观，
 * 属性面板据此渲染成下拉框。symbol 作为 tooltip 显示对应的 LVGL 常量。 */
struct EnumOption {
    int value;
    const char* label;
    const char* symbol;
};

struct StringOption {
    const char* value;
    const char* label;
};

/* 返回该属性的枚举选项；nullptr 表示不是枚举属性 */
static inline const std::vector<EnumOption>* enumOptionsFor(const std::string& type,
                                                           const std::string& prop) {
    static const std::vector<EnumOption> alignOpts = {
        {0,  "默认（左上，跟随坐标）", "LV_ALIGN_DEFAULT"},
        {1,  "左上", "LV_ALIGN_TOP_LEFT"},
        {2,  "中上", "LV_ALIGN_TOP_MID"},
        {3,  "右上", "LV_ALIGN_TOP_RIGHT"},
        {4,  "左下", "LV_ALIGN_BOTTOM_LEFT"},
        {5,  "中下", "LV_ALIGN_BOTTOM_MID"},
        {6,  "右下", "LV_ALIGN_BOTTOM_RIGHT"},
        {7,  "左中", "LV_ALIGN_LEFT_MID"},
        {8,  "右中", "LV_ALIGN_RIGHT_MID"},
        {9,  "居中", "LV_ALIGN_CENTER"},
        {10, "外部·左上", "LV_ALIGN_OUT_TOP_LEFT"},
        {11, "外部·中上", "LV_ALIGN_OUT_TOP_MID"},
        {12, "外部·右上", "LV_ALIGN_OUT_TOP_RIGHT"},
        {13, "外部·左下", "LV_ALIGN_OUT_BOTTOM_LEFT"},
        {14, "外部·中下", "LV_ALIGN_OUT_BOTTOM_MID"},
        {15, "外部·右下", "LV_ALIGN_OUT_BOTTOM_RIGHT"},
        {16, "外部·左端·上", "LV_ALIGN_OUT_LEFT_TOP"},
        {17, "外部·左端·中", "LV_ALIGN_OUT_LEFT_MID"},
        {18, "外部·左端·下", "LV_ALIGN_OUT_LEFT_BOTTOM"},
        {19, "外部·右端·上", "LV_ALIGN_OUT_RIGHT_TOP"},
        {20, "外部·右端·中", "LV_ALIGN_OUT_RIGHT_MID"},
        {21, "外部·右端·下", "LV_ALIGN_OUT_RIGHT_BOTTOM"},
    };
    static const std::vector<EnumOption> dirOpts = {
        {0,  "无（不滚动）", "LV_DIR_NONE"},
        {1,  "左", "LV_DIR_LEFT"},
        {2,  "右", "LV_DIR_RIGHT"},
        {4,  "上", "LV_DIR_TOP"},
        {8,  "下", "LV_DIR_BOTTOM"},
        {3,  "水平", "LV_DIR_HOR"},
        {12, "垂直", "LV_DIR_VER"},
        {15, "全部", "LV_DIR_ALL"},
    };
    static const std::vector<EnumOption> scrollbarOpts = {
        {0, "关闭", "LV_SCROLLBAR_MODE_OFF"},
        {1, "常显", "LV_SCROLLBAR_MODE_ON"},
        {2, "滚动时显示", "LV_SCROLLBAR_MODE_ACTIVE"},
        {3, "自动", "LV_SCROLLBAR_MODE_AUTO"},
    };
    static const std::vector<EnumOption> gradOpts = {
        {0, "无", "LV_GRAD_DIR_NONE"},
        {1, "垂直", "LV_GRAD_DIR_VER"},
        {2, "水平", "LV_GRAD_DIR_HOR"},
        {3, "线性", "LV_GRAD_DIR_LINEAR"},
        {4, "径向", "LV_GRAD_DIR_RADIAL"},
        {5, "锥形", "LV_GRAD_DIR_CONICAL"},
    };
    static const std::vector<EnumOption> borderSideOpts = {
        {0,  "无", "LV_BORDER_SIDE_NONE"},
        {1,  "下", "LV_BORDER_SIDE_BOTTOM"},
        {2,  "上", "LV_BORDER_SIDE_TOP"},
        {4,  "左", "LV_BORDER_SIDE_LEFT"},
        {8,  "右", "LV_BORDER_SIDE_RIGHT"},
        {15, "全部", "LV_BORDER_SIDE_FULL"},
        {3,  "上下", "BOTTOM|TOP"},
        {12, "左右", "LEFT|RIGHT"},
        {5,  "上+左", "TOP|LEFT"},
        {10, "下+右", "BOTTOM|RIGHT"},
        {16, "内部（按钮矩阵）", "LV_BORDER_SIDE_INTERNAL"},
    };
    static const std::vector<EnumOption> baseDirOpts = {
        {0, "从左到右", "LV_BASE_DIR_LTR"},
        {1, "从右到左", "LV_BASE_DIR_RTL"},
        {32, "中性", "LV_BASE_DIR_NEUTRAL"},
    };
    static const std::vector<EnumOption> textAlignOpts = {
        {0, "自动（继承）", "LV_TEXT_ALIGN_AUTO"},
        {1, "左对齐", "LV_TEXT_ALIGN_LEFT"},
        {2, "居中", "LV_TEXT_ALIGN_CENTER"},
        {3, "右对齐", "LV_TEXT_ALIGN_RIGHT"},
    };
    static const std::vector<EnumOption> textDecorOpts = {
        {0, "无", "LV_TEXT_DECOR_NONE"},
        {1, "下划线", "LV_TEXT_DECOR_UNDERLINE"},
        {2, "删除线", "LV_TEXT_DECOR_STRIKETHROUGH"},
    };
    static const std::vector<EnumOption> blendOpts = {
        {0, "正常", "LV_BLEND_MODE_NORMAL"},
        {1, "叠加", "LV_BLEND_MODE_ADDITIVE"},
        {2, "相减", "LV_BLEND_MODE_SUBTRACTIVE"},
        {3, "正片叠底", "LV_BLEND_MODE_MULTIPLY"},
        {4, "差值", "LV_BLEND_MODE_DIFFERENCE"},
    };
    static const std::vector<EnumOption> flexAlignOpts = {
        {0, "起始", "LV_FLEX_ALIGN_START"},
        {1, "末尾", "LV_FLEX_ALIGN_END"},
        {2, "居中", "LV_FLEX_ALIGN_CENTER"},
        {3, "均匀分布", "LV_FLEX_ALIGN_SPACE_EVENLY"},
        {4, "环绕分布", "LV_FLEX_ALIGN_SPACE_AROUND"},
        {5, "两端对齐", "LV_FLEX_ALIGN_SPACE_BETWEEN"},
    };

    if (prop == "align") return &alignOpts;
    if (prop == "scroll_dir") return &dirOpts;
    if (prop == "scrollbar_mode") return &scrollbarOpts;
    if (prop == "bg_grad_dir") return &gradOpts;
    if (prop == "border_side") return &borderSideOpts;
    if (prop == "base_dir") return &baseDirOpts;
    if (prop == "text_align") return &textAlignOpts;
    if (prop == "text_decor") return &textDecorOpts;
    if (prop == "blend_mode") return &blendOpts;
    if (prop == "align_cross") return &flexAlignOpts;

    if (prop == "mode") {
        static const std::vector<EnumOption> sliderMode = {
            {0, "普通", "LV_SLIDER_MODE_NORMAL"},
            {1, "对称", "LV_SLIDER_MODE_SYMMETRICAL"},
            {2, "双值范围", "LV_SLIDER_MODE_RANGE"},
        };
        static const std::vector<EnumOption> barMode = {
            {0, "普通", "LV_BAR_MODE_NORMAL"},
            {1, "对称", "LV_BAR_MODE_SYMMETRICAL"},
            {2, "双值范围", "LV_BAR_MODE_RANGE"},
        };
        static const std::vector<EnumOption> arcMode = {
            {0, "普通", "LV_ARC_MODE_NORMAL"},
            {1, "对称", "LV_ARC_MODE_SYMMETRICAL"},
            {2, "反向", "LV_ARC_MODE_REVERSE"},
        };
        static const std::vector<EnumOption> scaleMode = {
            {0,  "水平·刻度在上", "LV_SCALE_MODE_HORIZONTAL_TOP"},
            {1,  "水平·刻度在下", "LV_SCALE_MODE_HORIZONTAL_BOTTOM"},
            {2,  "垂直·刻度在左", "LV_SCALE_MODE_VERTICAL_LEFT"},
            {4,  "垂直·刻度在右", "LV_SCALE_MODE_VERTICAL_RIGHT"},
            {8,  "圆盘·向内", "LV_SCALE_MODE_ROUND_INNER"},
            {16, "圆盘·向外", "LV_SCALE_MODE_ROUND_OUTER"},
        };
        static const std::vector<EnumOption> spanMode = {
            {0, "固定尺寸", "LV_SPAN_MODE_FIXED"},
            {1, "按内容扩展", "LV_SPAN_MODE_EXPAND"},
            {2, "换行扩展高度", "LV_SPAN_MODE_BREAK"},
        };
        static const std::vector<EnumOption> kbMode = {
            {0, "小写字母", "LV_KEYBOARD_MODE_TEXT_LOWER"},
            {1, "大写字母", "LV_KEYBOARD_MODE_TEXT_UPPER"},
            {2, "特殊字符", "LV_KEYBOARD_MODE_SPECIAL"},
            {3, "数字", "LV_KEYBOARD_MODE_NUMBER"},
            {4, "自定义 1", "LV_KEYBOARD_MODE_USER_1"},
            {5, "自定义 2", "LV_KEYBOARD_MODE_USER_2"},
            {6, "自定义 3", "LV_KEYBOARD_MODE_USER_3"},
            {7, "自定义 4", "LV_KEYBOARD_MODE_USER_4"},
        };
        if (type == "LSlider") return &sliderMode;
        if (type == "LBar") return &barMode;
        if (type == "LArc") return &arcMode;
        if (type == "LScale") return &scaleMode;
        if (type == "LSpan") return &spanMode;
        if (type == "LKeyboard") return &kbMode;
        return nullptr;
    }
    if (prop == "direction" && type == "LComboBox") return &dirOpts;
    if (prop == "tab_bar_position" && type == "LTabView") return &dirOpts;
    /* 工程里只编入了这 7 档 Montserrat 字体，其它字号会被归到最近的一档 */
    if (prop == "text_font_size" || (prop == "font_size" && type == "LLabel")) {
        static const std::vector<EnumOption> v = {
            {12, "12 px", "lv_font_montserrat_12"},
            {14, "14 px", "lv_font_montserrat_14"},
            {16, "16 px", "lv_font_montserrat_16"},
            {18, "18 px", "lv_font_montserrat_18"},
            {20, "20 px", "lv_font_montserrat_20"},
            {24, "24 px", "lv_font_montserrat_24"},
            {28, "28 px（最大）", "lv_font_montserrat_28"},
        };
        return &v;
    }
    if (prop == "dir" && type == "LTile") return &dirOpts;
    if (prop == "digit_step_direction" && type == "LSpinbox") return &dirOpts;

    if (prop == "long_mode" && type == "LLabel") {
        static const std::vector<EnumOption> v = {
            {0, "换行", "LV_LABEL_LONG_WRAP"},
            {1, "省略号", "LV_LABEL_LONG_DOT"},
            {2, "来回滚动", "LV_LABEL_LONG_SCROLL"},
            {3, "循环滚动", "LV_LABEL_LONG_SCROLL_CIRCULAR"},
            {4, "裁剪", "LV_LABEL_LONG_CLIP"},
        };
        return &v;
    }
    if (prop == "overflow" && type == "LSpan") {
        static const std::vector<EnumOption> v = {
            {0, "裁剪", "LV_SPAN_OVERFLOW_CLIP"},
            {1, "省略号", "LV_SPAN_OVERFLOW_ELLIPSIS"},
        };
        return &v;
    }
    if (prop == "type" && type == "LChart") {
        static const std::vector<EnumOption> v = {
            {0, "无", "LV_CHART_TYPE_NONE"},
            {1, "折线", "LV_CHART_TYPE_LINE"},
            {2, "柱状", "LV_CHART_TYPE_BAR"},
            {3, "散点", "LV_CHART_TYPE_SCATTER"},
        };
        return &v;
    }
    if (prop == "update_mode" && type == "LChart") {
        static const std::vector<EnumOption> v = {
            {0, "平移", "LV_CHART_UPDATE_MODE_SHIFT"},
            {1, "环绕", "LV_CHART_UPDATE_MODE_CIRCULAR"},
        };
        return &v;
    }
    if (prop == "mode_header" && type == "LMenu") {
        static const std::vector<EnumOption> v = {
            {0, "顶部固定", "LV_MENU_HEADER_TOP_FIXED"},
            {1, "顶部（可滚出视野）", "LV_MENU_HEADER_TOP_UNFIXED"},
            {2, "底部固定", "LV_MENU_HEADER_BOTTOM_FIXED"},
        };
        return &v;
    }
    if (prop == "mode_root_back_button" && type == "LMenu") {
        static const std::vector<EnumOption> v = {
            {0, "禁用", "LV_MENU_ROOT_BACK_BUTTON_DISABLED"},
            {1, "启用", "LV_MENU_ROOT_BACK_BUTTON_ENABLED"},
        };
        return &v;
    }
    if (prop == "state" && type == "LImageButton") {
        static const std::vector<EnumOption> v = {
            {0, "释放", "LV_IMAGEBUTTON_STATE_RELEASED"},
            {1, "按下", "LV_IMAGEBUTTON_STATE_PRESSED"},
            {2, "禁用", "LV_IMAGEBUTTON_STATE_DISABLED"},
            {3, "选中·释放", "LV_IMAGEBUTTON_STATE_CHECKED_RELEASED"},
            {4, "选中·按下", "LV_IMAGEBUTTON_STATE_CHECKED_PRESSED"},
            {5, "选中·禁用", "LV_IMAGEBUTTON_STATE_CHECKED_DISABLED"},
        };
        return &v;
    }
    return nullptr;
}

/* 字符串型枚举（少量） */
static inline const std::vector<StringOption>* stringOptionsFor(const std::string& type,
                                                               const std::string& prop) {
    if (prop == "theme" && type == "LButton") {
        static const std::vector<StringOption> v = {
            {"", "默认（主题色）"},
            {"primary", "主色（蓝）"},
            {"danger", "危险（红）"},
            {"success", "成功（绿）"},
            {"warning", "警告（橙）"},
        };
        return &v;
    }
    return nullptr;
}

}
