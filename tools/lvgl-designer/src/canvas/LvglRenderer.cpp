#include "canvas/LvglRenderer.hpp"
#include "lvgl++/core/style.hpp"
#include <set>
#include <cstdio>
#include <algorithm>
#include <algorithm>
#include <lvgl/src/widgets/label/lv_label.h>
#include <lvgl/src/widgets/button/lv_button.h>
#include <lvgl/src/widgets/slider/lv_slider.h>
#include <lvgl/src/widgets/bar/lv_bar.h>
#include <lvgl/src/widgets/checkbox/lv_checkbox.h>
#include <lvgl/src/widgets/switch/lv_switch.h>
#include <lvgl/src/widgets/dropdown/lv_dropdown.h>
#include <lvgl/src/widgets/textarea/lv_textarea.h>
#include <lvgl/src/widgets/image/lv_image.h>
#include <lvgl/src/widgets/arc/lv_arc.h>
#include <lvgl/src/widgets/spinner/lv_spinner.h>
#include <lvgl/src/widgets/led/lv_led.h>
#include <lvgl/src/widgets/line/lv_line.h>
#include <lvgl/src/widgets/table/lv_table.h>
#include <lvgl/src/widgets/chart/lv_chart.h>
#include <lvgl/src/widgets/scale/lv_scale.h>
#include <lvgl/src/widgets/keyboard/lv_keyboard.h>
#include <lvgl/src/widgets/roller/lv_roller.h>
#include <lvgl/src/widgets/list/lv_list.h>
#include <lvgl/src/widgets/menu/lv_menu.h>
#include <lvgl/src/widgets/msgbox/lv_msgbox.h>
#include <lvgl/src/widgets/tabview/lv_tabview.h>
#include <lvgl/src/widgets/tileview/lv_tileview.h>
#include <lvgl/src/widgets/canvas/lv_canvas.h>
#include <lvgl/src/widgets/animimage/lv_animimage.h>
#include <lvgl/src/widgets/span/lv_span.h>
#include <lvgl/src/widgets/spinbox/lv_spinbox.h>
#include <lvgl/src/widgets/calendar/lv_calendar.h>
#include <lvgl/src/widgets/imagebutton/lv_imagebutton.h>
#include <lvgl/src/widgets/buttonmatrix/lv_buttonmatrix.h>
#include <lvgl/src/widgets/win/lv_win.h>

namespace designer {

static void applyTheme(lv_obj_t* obj, const std::string& theme) {
    if (theme == "primary") {
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x2196F3), 0);
        lv_obj_set_style_radius(obj, 8, 0);
        lv_obj_set_style_pad_all(obj, 10, 0);
    } else if (theme == "danger") {
        lv_obj_set_style_bg_color(obj, lv_color_hex(0xF44336), 0);
        lv_obj_set_style_radius(obj, 8, 0);
        lv_obj_set_style_pad_all(obj, 10, 0);
    } else if (theme == "success") {
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x4CAF50), 0);
        lv_obj_set_style_radius(obj, 8, 0);
        lv_obj_set_style_pad_all(obj, 10, 0);
    } else if (theme == "warning") {
        lv_obj_set_style_bg_color(obj, lv_color_hex(0xFF9800), 0);
        lv_obj_set_style_radius(obj, 8, 0);
        lv_obj_set_style_pad_all(obj, 10, 0);
    }
}

static void applyRadioButtonStyle(lv_obj_t* obj, const WidgetNode* node = nullptr) {
    if (!obj) return;
    /* 单选按钮的"透明底板"只是缺省外观；用户显式设置的样式优先 */
    auto has = [&](const char* k) { return node && node->props.count(k) > 0; };
    if (!has("bg_opa")) lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, LV_PART_MAIN);
    if (!has("border_width")) lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
    if (!has("pad_top")) lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN);
    if (!has("pad_bottom")) lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN);
    if (!has("pad_left")) lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN);
    if (!has("pad_right")) lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN);
    if (!has("radius")) lv_obj_set_style_radius(obj, 0, LV_PART_MAIN);
    if (!has("shadow_width")) lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN);

    lv_obj_t* ring = lv_obj_get_child(obj, 0);
    lv_obj_t* dot = ring ? lv_obj_get_child(ring, 0) : nullptr;
    lv_obj_t* label = lv_obj_get_child(obj, 1);
    if (!ring || !dot || !label) return;

    lv_obj_remove_style_all(ring);
    lv_obj_set_style_bg_opa(ring, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(ring, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(ring, lv_color_hex(0x888888), LV_PART_MAIN);
    lv_obj_set_style_radius(ring, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_pad_all(ring, 0, LV_PART_MAIN);
    lv_obj_clear_flag(ring, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_remove_style_all(dot);
    lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(dot, lv_color_hex(0x0e639c), LV_PART_MAIN);
    lv_obj_set_style_border_width(dot, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_clear_flag(dot, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_clear_flag(label, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_update_layout(obj);
    const int height = lv_obj_get_height(obj);
    const int width = lv_obj_get_width(obj);
    int ring_size = height - 8;
    if (ring_size > 18) ring_size = 18;
    if (ring_size < 8) ring_size = 8;
    const int x = 3;
    const int y = (height - ring_size) / 2;
    const int gap = 8;

    lv_obj_set_pos(ring, x, y);
    lv_obj_set_size(ring, ring_size, ring_size);

    int dot_size = ring_size / 2;
    if (dot_size < 6) dot_size = 6;
    lv_obj_set_size(dot, dot_size, dot_size);
    lv_obj_center(dot);

    const lv_font_t* font = lv_obj_get_style_text_font(label, LV_PART_MAIN);
    const int font_h = font ? lv_font_get_line_height(font) : 16;
    int label_x = x + ring_size + gap;
    int label_width = width - label_x;
    if (label_width < 0) label_width = 0;
    int label_y = (height - font_h) / 2;
    if (label_y < 0) label_y = 0;
    lv_obj_set_pos(label, label_x, label_y);
    lv_obj_set_size(label, label_width, font_h);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);

    if (lv_obj_has_state(obj, LV_STATE_CHECKED)) lv_obj_clear_flag(dot, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_add_flag(dot, LV_OBJ_FLAG_HIDDEN);
}

static std::string unescapeNewlines(std::string s) {
    size_t pos = 0;
    while ((pos = s.find("\\n", pos)) != std::string::npos) {
        s.replace(pos, 2, "\n");
        pos += 1;
    }
    return s;
}

static std::vector<std::string> splitStr(const std::string& s, char sep) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : s) {
        if (c == sep) { out.push_back(cur); cur.clear(); }
        else cur += c;
    }
    out.push_back(cur);
    return out;
}

static std::string trimStr(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r");
    return s.substr(a, b - a + 1);
}

static void applyCheckableAlignment(lv_obj_t* obj, const WidgetNode* node = nullptr) {
    if (!obj) return;
    lv_obj_update_layout(obj);
    const lv_font_t* font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    const int font_h = font ? lv_font_get_line_height(font) : 14;
    const int obj_h = lv_obj_get_height(obj);
    const int extra = obj_h > font_h ? (obj_h - font_h) / 2 : 0;
    const int top_extra = extra > 0 ? extra + 1 : 0;
    const int bottom_extra = extra > 0 ? extra - 1 : 0;

    /* 垂直居中只是缺省排版；用户显式设了对应的 padding 就以用户为准 */
    auto has = [&](const char* k) { return node && node->props.count(k) > 0; };
    if (!has("pad_top")) lv_obj_set_style_pad_top(obj, top_extra, LV_PART_MAIN);
    if (!has("pad_bottom")) lv_obj_set_style_pad_bottom(obj, bottom_extra, LV_PART_MAIN);
    if (!has("pad_column")) lv_obj_set_style_pad_column(obj, 6, LV_PART_MAIN);
    lv_obj_set_style_pad_all(obj, 1, LV_PART_INDICATOR);
}

lv_obj_t* LvglRenderer::createWidget(const WidgetNode& node, lv_obj_t* parent, bool selectable) {
    lv_obj_t* obj = nullptr;
    lv_obj_t* targetParent = parent ? parent : lv_scr_act();

    const bool isInstance = !node.type.empty() && node.type[0] == '@';

    if (isInstance) {
        /* 组件实例：透明根容器，稍后把组件内容递归建到它下面 */
        obj = lv_obj_create(targetParent);
        lv_obj_remove_style_all(obj);
        lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    } else if (node.type == "LButton") {
        obj = lv_button_create(targetParent);
    } else if (node.type == "LLabel") {
        obj = lv_label_create(targetParent);
    } else if (node.type == "LSlider") {
        obj = lv_slider_create(targetParent);
    } else if (node.type == "LBar") {
        obj = lv_bar_create(targetParent);
    } else if (node.type == "LCheckbox") {
        obj = lv_checkbox_create(targetParent);
        applyCheckableAlignment(obj, &node);
    } else if (node.type == "LSwitch") {
        obj = lv_switch_create(targetParent);
    } else if (node.type == "LComboBox") {
        obj = lv_dropdown_create(targetParent);
    } else if (node.type == "LLineEdit") {
        obj = lv_textarea_create(targetParent);
        lv_textarea_set_one_line(obj, true);
    } else if (node.type == "LTextEdit") {
        obj = lv_textarea_create(targetParent);
    } else if (node.type == "LImage") {
        obj = lv_image_create(targetParent);
    } else if (node.type == "LArc") {
        obj = lv_arc_create(targetParent);
    } else if (node.type == "LSpinner") {
        obj = lv_spinner_create(targetParent);
    } else if (node.type == "LLed") {
        obj = lv_led_create(targetParent);
    } else if (node.type == "LLine") {
        obj = lv_line_create(targetParent);
    } else if (node.type == "LTable") {
        obj = lv_table_create(targetParent);
    } else if (node.type == "LChart") {
        obj = lv_chart_create(targetParent);
    } else if (node.type == "LScale") {
        obj = lv_scale_create(targetParent);
    } else if (node.type == "LKeyboard") {
        obj = lv_keyboard_create(targetParent);
    } else if (node.type == "LRoller") {
        obj = lv_roller_create(targetParent);
    } else if (node.type == "LList") {
        obj = lv_list_create(targetParent);
    } else if (node.type == "LMenu") {
        obj = lv_menu_create(targetParent);
    } else if (node.type == "LMenuPage") {
        std::string title = node.props.count("title") ? node.propString("title") : "";
        /* lv_menu_page_create 要求父对象必须是菜单：拖到别处（父级是屏幕或普通容器）
         * 会直接核心转储。父级不是菜单时退化为普通容器占位，保证不崩且可见可选。 */
        bool underMenu = false;
        if (targetParent) {
            auto it = m_idMap.find(targetParent);
            if (it != m_idMap.end()) {
                auto ti = m_types.find(it->second);
                underMenu = ti != m_types.end() && ti->second == "LMenu";
            }
        }
        obj = underMenu ? lv_menu_page_create(targetParent, title.c_str())
                        : lv_obj_create(targetParent);
    } else if (node.type == "LMenuCont") {
        obj = lv_menu_cont_create(targetParent);
    } else if (node.type == "LMenuSection") {
        obj = lv_menu_section_create(targetParent);
    } else if (node.type == "LMsgBox") {
        obj = lv_msgbox_create(targetParent);
    } else if (node.type == "LTabView") {
        obj = lv_tabview_create(targetParent);
    } else if (node.type == "LTileView") {
        obj = lv_tileview_create(targetParent);
    } else if (node.type == "LTile") {
        int col = node.props.count("col") ? node.propInt("col") : 0;
        int row = node.props.count("row") ? node.propInt("row") : 0;
        int dir = node.props.count("dir") ? node.propInt("dir") : LV_DIR_RIGHT;
        obj = lv_tileview_add_tile(targetParent, (uint8_t)col, (uint8_t)row, (lv_dir_t)dir);
    } else if (node.type == "LCanvas") {
        obj = lv_canvas_create(targetParent);
    } else if (node.type == "LAnimImage") {
        obj = lv_animimg_create(targetParent);
    } else if (node.type == "LSpan") {
        obj = lv_spangroup_create(targetParent);
    } else if (node.type == "LSpinbox") {
        obj = lv_spinbox_create(targetParent);
    } else if (node.type == "LCalendar") {
        obj = lv_calendar_create(targetParent);
    } else if (node.type == "LImageButton") {
        obj = lv_imagebutton_create(targetParent);
    } else if (node.type == "LButtonMatrix") {
        obj = lv_buttonmatrix_create(targetParent);
    } else if (node.type == "LRadioButton") {
        obj = lv_obj_create(targetParent);
        lv_obj_t* ring = lv_obj_create(obj);
        lv_obj_t* dot = lv_obj_create(ring);
        lv_obj_add_flag(dot, LV_OBJ_FLAG_HIDDEN);
        lv_label_create(obj);
    } else if (node.type == "LGroupBox") {
        obj = lv_obj_create(targetParent);
        lv_obj_set_style_border_width(obj, 2, 0);
        lv_obj_set_style_border_color(obj, lv_color_hex(0xCCCCCC), 0);
        lv_obj_set_style_border_opa(obj, LV_OPA_COVER, 0);
        lv_obj_set_style_radius(obj, 4, 0);
    } else if (node.type == "LVBoxLayout") {
        obj = lv_obj_create(targetParent);
        lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(obj,
            LV_FLEX_ALIGN_START,
            LV_FLEX_ALIGN_CENTER,
            LV_FLEX_ALIGN_START);
    } else if (node.type == "LHBoxLayout") {
        obj = lv_obj_create(targetParent);
        lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(obj,
            LV_FLEX_ALIGN_START,
            LV_FLEX_ALIGN_CENTER,
            LV_FLEX_ALIGN_START);
    } else if (node.type == "LGridLayout") {
        obj = lv_obj_create(targetParent);
    } else if (node.type == "LStackedLayout") {
        obj = lv_obj_create(targetParent);
    } else if (node.type == "LWin") {
        obj = lv_win_create(targetParent);
    } else {
        obj = lv_obj_create(targetParent);
    }

    if (obj) {
        if (selectable) {
            registerObject(node.id, node.type, obj);
        } else {
            /* 组件内部节点：不可单独选中，点击冒泡到实例根，同时记录以便应用导出参数 */
            lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
            m_instanceObjects[node.id] = obj;
        }
        applyProps(obj, node);

        if (isInstance && m_instanceDepth < 8) {
            const LuiDocument* cdoc = m_componentResolver ? m_componentResolver(node.type.substr(1)) : nullptr;
            if (cdoc) {
                auto saved = m_instanceObjects;
                m_instanceObjects.clear();
                m_instanceDepth++;
                for (const auto& c : cdoc->children()) createWidget(c, obj, false);
                m_instanceDepth--;
                applyInstanceExports(obj, node, *cdoc);
                m_instanceObjects = saved;
            }
        }

        if (node.type == "LCheckbox") {
            applyCheckableAlignment(obj, &node);
        } else if (node.type == "LRadioButton") {
            applyRadioButtonStyle(obj, &node);
        }

        if (node.type == "LVBoxLayout" || node.type == "LHBoxLayout"
            || node.type == "LGridLayout"
            || node.type == "LStackedLayout" || node.type == "LGroupBox") {
            if (!node.props.count("padding")) {
                lv_obj_set_style_pad_all(obj, 0, 0);
            }
        }

        for (const auto& child : node.children) {
            createWidget(child, obj, selectable);
        }

        if (node.type == "LGridLayout" && node.props.count("columns")) {
            int cols = std::max(1, node.propInt("columns", 2));
            int rows = std::max(1, node.propInt("rows", 2));
            int childCount = lv_obj_get_child_count(obj);
            for (int i = 0; i < childCount; i++) {
                lv_obj_t* c = lv_obj_get_child(obj, i);
                if (i < (int)node.children.size()) {
                    const auto& child = node.children[i];
                    int col = child.propInt("grid_col", i % cols);
                    int row = child.propInt("grid_row", i / cols);
                    int col_span = child.propInt("grid_col_span", 1);
                    int row_span = child.propInt("grid_row_span", 1);
                    lv_obj_set_grid_cell(c, LV_GRID_ALIGN_STRETCH, col, col_span,
                                         LV_GRID_ALIGN_STRETCH, row, row_span);
                } else {
                    int col = i % cols;
                    int row = i / cols;
                    lv_obj_set_grid_cell(c, LV_GRID_ALIGN_STRETCH, col, 1,
                                         LV_GRID_ALIGN_STRETCH, row, 1);
                }
            }
        }

        if (node.type == "LStackedLayout") {
            int idx = node.props.count("current_index")
                ? node.propInt("current_index", 0) : 0;
            createStackedNav(obj, node.id, idx);
            applyStackedVisibility(obj, idx);
        }
    }

    return obj;
}

void LvglRenderer::applyProps(lv_obj_t* obj, const WidgetNode& node) {
    auto propInt = [&](const std::string& key, int def = 0) {
        return node.propInt(key, def);
    };

    auto propStr = [&](const std::string& key, const std::string& def = "") {
        return node.propString(key, def);
    };

    auto propBool = [&](const std::string& key, bool def = false) {
        return node.propBool(key, def);
    };

    /* "#别名" 解析成设计器预览用的 lv_fs 路径；其它值原样返回 */
    auto resolveSrc = [&](const std::string& s) -> std::string {
        if (s.empty() || s[0] != '#') return s;
        auto it = m_resAbs.find(s.substr(1));
        if (it == m_resAbs.end()) return std::string();
        return "A:" + it->second;
    };

    auto propHex = [&](const std::string& key, uint32_t def = 0) -> uint32_t {
        auto it = node.props.find(key);
        if (it == node.props.end()) return def;
        QString s = it->second.toString();
        s.remove("0x").remove("0X");
        bool ok = false;
        uint32_t v = s.toUInt(&ok, 16);
        return ok ? v : def;
    };

    // === Position & Size ===
    /* 图块由 tileview 按 col/row 定位（lv_tileview_add_tile 用百分比坐标并铺满视图），
     * 这里若再写死坐标，预览会和运行时的 grid 布局不一致，col/row 也会失效。 */
    const bool placedByParent = node.type == "LTile";
    if (!placedByParent && node.props.count("pos")) {
        auto pos = node.pos();
        /* 有些控件创建时自带对齐（lv_keyboard 默认 LV_ALIGN_BOTTOM_MID），
         * 而 LVGL 里对齐优先于坐标 —— 不清掉的话 set_pos 会被忽略，
         * 控件会被顶到父对象底部、甚至跑出画布，看起来像"拖进去就消失"。 */
        lv_obj_set_align(obj, LV_ALIGN_DEFAULT);
        lv_obj_set_pos(obj, (int)pos.x(), (int)pos.y());
    }

    if (!placedByParent && node.props.count("size")) {
        auto sz = node.size();
        lv_obj_set_size(obj, (int)sz.width(), (int)sz.height());
    }

    if (node.props.count("theme")) {
        applyTheme(obj, propStr("theme"));
    }

    // === Section A: Generic Object Properties ===
    if (node.props.count("align")) {
        lv_obj_set_align(obj, (lv_align_t)propInt("align"));
    }
    if (node.props.count("hidden") && propBool("hidden")) {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
    if (node.props.count("clickable")) {
        if (propBool("clickable")) lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        else lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    }
    if (node.props.count("scrollable")) {
        if (propBool("scrollable")) lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        else lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    }
    if (node.props.count("scroll_dir")) {
        lv_obj_set_scroll_dir(obj, (lv_dir_t)propInt("scroll_dir"));
    }
    if (node.props.count("scrollbar_mode")) {
        lv_obj_set_scrollbar_mode(obj, (lv_scrollbar_mode_t)propInt("scrollbar_mode"));
    }
    if (node.props.count("layout")) {
        int layout = propInt("layout");
        if (layout == 1) lv_obj_set_layout(obj, LV_LAYOUT_FLEX);
    }
    if (node.props.count("ext_click_area")) {
        lv_obj_set_ext_click_area(obj, propInt("ext_click_area"));
    }

    // === Section B: Background Style ===
    if (node.props.count("bg_color")) lv_obj_set_style_bg_color(obj, lv_color_hex(propHex("bg_color")), 0);
    if (node.props.count("bg_opa")) lv_obj_set_style_bg_opa(obj, propInt("bg_opa"), 0);
    if (node.props.count("bg_grad_color")) lv_obj_set_style_bg_grad_color(obj, lv_color_hex(propHex("bg_grad_color")), 0);
    if (node.props.count("bg_grad_dir")) lv_obj_set_style_bg_grad_dir(obj, (lv_grad_dir_t)propInt("bg_grad_dir"), 0);
    if (node.props.count("bg_main_stop")) lv_obj_set_style_bg_main_stop(obj, propInt("bg_main_stop"), 0);
    if (node.props.count("bg_grad_stop")) lv_obj_set_style_bg_grad_stop(obj, propInt("bg_grad_stop"), 0);
    if (node.props.count("bg_image_src")) { std::string s = resolveSrc(propStr("bg_image_src")); if (!s.empty()) lv_obj_set_style_bg_image_src(obj, intern(s), 0); }
    if (node.props.count("bg_image_tiled")) lv_obj_set_style_bg_image_tiled(obj, propBool("bg_image_tiled"), 0);
    if (node.props.count("bg_image_opa")) lv_obj_set_style_bg_image_opa(obj, propInt("bg_image_opa"), 0);
    if (node.props.count("bg_image_recolor")) lv_obj_set_style_bg_image_recolor(obj, lv_color_hex(propHex("bg_image_recolor")), 0);
    if (node.props.count("bg_image_recolor_opa")) lv_obj_set_style_bg_image_recolor_opa(obj, propInt("bg_image_recolor_opa"), 0);
    if (node.props.count("bg_image_tiled") && propBool("bg_image_tiled")) {
        lv_obj_set_style_bg_image_tiled(obj, true, 0);
    }
    if (node.props.count("bg_image_src")) {
        std::string src = resolveSrc(propStr("bg_image_src"));
        if (!src.empty()) {
            lv_image_header_t hdr;
            if (lv_image_decoder_get_info(src.c_str(), &hdr) == LV_RES_OK) {
                lv_obj_set_style_bg_image_src(obj, intern(src), 0);   // 样式值只存指针
            }
        }
    }

    // === Section C: Border Style ===
    if (node.props.count("border_color")) lv_obj_set_style_border_color(obj, lv_color_hex(propHex("border_color")), 0);
    if (node.props.count("border_width")) lv_obj_set_style_border_width(obj, propInt("border_width"), 0);
    if (node.props.count("border_opa")) lv_obj_set_style_border_opa(obj, propInt("border_opa"), 0);
    if (node.props.count("border_side")) lv_obj_set_style_border_side(obj, (lv_border_side_t)propInt("border_side"), 0);
    if (node.props.count("border_post")) lv_obj_set_style_border_post(obj, propBool("border_post"), 0);

    // === Section D: Outline Style ===
    if (node.props.count("outline_color")) lv_obj_set_style_outline_color(obj, lv_color_hex(propHex("outline_color")), 0);
    if (node.props.count("outline_width")) lv_obj_set_style_outline_width(obj, propInt("outline_width"), 0);
    if (node.props.count("outline_opa")) lv_obj_set_style_outline_opa(obj, propInt("outline_opa"), 0);
    if (node.props.count("outline_pad")) lv_obj_set_style_outline_pad(obj, propInt("outline_pad"), 0);

    // === Section E: Shadow Style ===
    if (node.props.count("shadow_color")) lv_obj_set_style_shadow_color(obj, lv_color_hex(propHex("shadow_color")), 0);
    if (node.props.count("shadow_width")) lv_obj_set_style_shadow_width(obj, propInt("shadow_width"), 0);
    if (node.props.count("shadow_ofs_x")) lv_obj_set_style_shadow_ofs_x(obj, propInt("shadow_ofs_x"), 0);
    if (node.props.count("shadow_ofs_y")) lv_obj_set_style_shadow_ofs_y(obj, propInt("shadow_ofs_y"), 0);
    if (node.props.count("shadow_spread")) lv_obj_set_style_shadow_spread(obj, propInt("shadow_spread"), 0);
    if (node.props.count("shadow_opa")) lv_obj_set_style_shadow_opa(obj, propInt("shadow_opa"), 0);

    // === Section F: Padding ===
    if (node.props.count("pad_top")) lv_obj_set_style_pad_top(obj, propInt("pad_top"), 0);
    if (node.props.count("pad_bottom")) lv_obj_set_style_pad_bottom(obj, propInt("pad_bottom"), 0);
    if (node.props.count("pad_left")) lv_obj_set_style_pad_left(obj, propInt("pad_left"), 0);
    if (node.props.count("pad_right")) lv_obj_set_style_pad_right(obj, propInt("pad_right"), 0);
    if (node.props.count("pad_row")) lv_obj_set_style_pad_row(obj, propInt("pad_row"), 0);
    if (node.props.count("pad_column")) lv_obj_set_style_pad_column(obj, propInt("pad_column"), 0);

    // === Section G: Margin ===
    if (node.props.count("margin_top")) lv_obj_set_style_margin_top(obj, propInt("margin_top"), 0);
    if (node.props.count("margin_bottom")) lv_obj_set_style_margin_bottom(obj, propInt("margin_bottom"), 0);
    if (node.props.count("margin_left")) lv_obj_set_style_margin_left(obj, propInt("margin_left"), 0);
    if (node.props.count("margin_right")) lv_obj_set_style_margin_right(obj, propInt("margin_right"), 0);

    // === Section H: Size Constraints ===
    if (node.props.count("min_width")) lv_obj_set_style_min_width(obj, propInt("min_width"), 0);
    if (node.props.count("max_width")) lv_obj_set_style_max_width(obj, propInt("max_width"), 0);
    if (node.props.count("min_height")) lv_obj_set_style_min_height(obj, propInt("min_height"), 0);
    if (node.props.count("max_height")) lv_obj_set_style_max_height(obj, propInt("max_height"), 0);

    /* LVGL 9.2 的软件渲染在"无内容对象 + transform"以及"无内容对象 + 非 0 混合模式"
     * 下会陷入死循环（空 image / 空 canvas / 空 animimg / 图块 / 富文本组）。
     * 这类对象跳过对应样式，避免改个属性就把设计器卡死。 */
    const bool imgHasSrc = (node.props.count("src") && !propStr("src").empty()) ||
                           (node.props.count("src_released") && !propStr("src_released").empty());
    const bool noContent = node.type == "LCanvas" || node.type == "LTile" ||
                           node.type == "LAnimImage" ||
                           ((node.type == "LImage" || node.type == "LImageButton") && !imgHasSrc);
    const bool noBlend = noContent || node.type == "LSpan";

    // === Section I: Appearance ===
    if (node.props.count("radius")) lv_obj_set_style_radius(obj, propInt("radius"), 0);
    if (node.props.count("clip_corner")) lv_obj_set_style_clip_corner(obj, propBool("clip_corner"), 0);
    if (node.props.count("opa")) lv_obj_set_style_opa(obj, propInt("opa"), 0);
    if (!noBlend && node.props.count("blend_mode")) lv_obj_set_style_blend_mode(obj, (lv_blend_mode_t)propInt("blend_mode"), 0);
    if (node.props.count("base_dir")) lv_obj_set_style_base_dir(obj, (lv_base_dir_t)propInt("base_dir"), 0);

    // === Section J: Text Style (generic, applied to all) ===
    if (node.props.count("text_color")) lv_obj_set_style_text_color(obj, lv_color_hex(propHex("text_color")), 0);
    if (node.props.count("text_opa")) lv_obj_set_style_text_opa(obj, propInt("text_opa"), 0);
    if (node.props.count("text_font_size")) lv_obj_set_style_text_font(obj, ::lvgl::font_by_size(propInt("text_font_size")), 0);
    if (node.props.count("text_font_size")) {
        int fs = propInt("text_font_size");
        if (fs <= 12) lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, 0);
        else if (fs <= 14) lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, 0);
        else if (fs <= 16) lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, 0);
        else if (fs <= 18) lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, 0);
        else if (fs <= 20) lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, 0);
        else if (fs <= 24) lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, 0);
        else lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, 0);
    }
    if (node.props.count("text_letter_space")) lv_obj_set_style_text_letter_space(obj, propInt("text_letter_space"), 0);
    if (node.props.count("text_line_space")) lv_obj_set_style_text_line_space(obj, propInt("text_line_space"), 0);
    if (node.props.count("text_align")) lv_obj_set_style_text_align(obj, (lv_text_align_t)propInt("text_align"), 0);
    if (node.props.count("text_decor")) lv_obj_set_style_text_decor(obj, (lv_text_decor_t)propInt("text_decor"), 0);

    // === Section K: Transform (无内容对象见上方 noContent 说明) ===
    if (!noContent) {
    if (node.props.count("transform_rotation")) lv_obj_set_style_transform_rotation(obj, propInt("transform_rotation"), 0);
    if (node.props.count("transform_scale_x")) lv_obj_set_style_transform_scale_x(obj, propInt("transform_scale_x"), 0);
    if (node.props.count("transform_scale_y")) lv_obj_set_style_transform_scale_y(obj, propInt("transform_scale_y"), 0);
    if (node.props.count("transform_pivot_x")) lv_obj_set_style_transform_pivot_x(obj, propInt("transform_pivot_x"), 0);
    if (node.props.count("transform_pivot_y")) lv_obj_set_style_transform_pivot_y(obj, propInt("transform_pivot_y"), 0);
    if (node.props.count("translate_x")) lv_obj_set_style_translate_x(obj, propInt("translate_x"), 0);
    if (node.props.count("translate_y")) lv_obj_set_style_translate_y(obj, propInt("translate_y"), 0);
    if (node.props.count("transform_skew_x")) lv_obj_set_style_transform_skew_x(obj, propInt("transform_skew_x"), 0);
    if (node.props.count("transform_skew_y")) lv_obj_set_style_transform_skew_y(obj, propInt("transform_skew_y"), 0);
    }

    // === Section L: Line Style ===
    if (node.props.count("line_color")) lv_obj_set_style_line_color(obj, lv_color_hex(propHex("line_color")), 0);
    if (node.props.count("line_width")) lv_obj_set_style_line_width(obj, propInt("line_width"), 0);
    if (node.props.count("line_opa")) lv_obj_set_style_line_opa(obj, propInt("line_opa"), 0);
    if (node.props.count("line_dash_width")) lv_obj_set_style_line_dash_width(obj, propInt("line_dash_width"), 0);
    if (node.props.count("line_dash_gap")) lv_obj_set_style_line_dash_gap(obj, propInt("line_dash_gap"), 0);
    if (node.props.count("line_rounded")) lv_obj_set_style_line_rounded(obj, propBool("line_rounded"), 0);

    // === Section M: Arc Style ===
    if (node.props.count("arc_color")) lv_obj_set_style_arc_color(obj, lv_color_hex(propHex("arc_color")), 0);
    if (node.props.count("arc_width")) lv_obj_set_style_arc_width(obj, propInt("arc_width"), 0);
    if (node.props.count("arc_opa")) lv_obj_set_style_arc_opa(obj, propInt("arc_opa"), 0);
    if (node.props.count("arc_rounded")) lv_obj_set_style_arc_rounded(obj, propBool("arc_rounded"), 0);

    // === Section N: Image Style ===
    if (node.props.count("image_opa")) lv_obj_set_style_image_opa(obj, propInt("image_opa"), 0);
    if (node.props.count("image_recolor")) lv_obj_set_style_image_recolor(obj, lv_color_hex(propHex("image_recolor")), 0);
    if (node.props.count("image_recolor_opa")) lv_obj_set_style_image_recolor_opa(obj, propInt("image_recolor_opa"), 0);

    // === Section O: Component-Specific Properties ===

    if (node.type == "LButton") {
        if (node.props.count("text")) {
            lv_obj_t* label = lv_obj_get_child(obj, 0);
            if (!label) {
                label = lv_label_create(obj);
                lv_obj_center(label);
            }
            lv_label_set_text(label, propStr("text").c_str());
        }
        if (node.props.count("disabled") && propBool("disabled")) {
            lv_obj_add_state(obj, LV_STATE_DISABLED);
        }
    } else if (node.type == "LLabel") {
        if (node.props.count("text")) lv_label_set_text(obj, propStr("text").c_str());
        if (node.props.count("color")) lv_obj_set_style_text_color(obj, lv_color_hex(propHex("color")), 0);
        if (node.props.count("font_size")) {
            int fs = propInt("font_size");
            if (fs <= 12) lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, 0);
            else if (fs <= 14) lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, 0);
            else if (fs <= 16) lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, 0);
            else if (fs <= 18) lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, 0);
            else if (fs <= 20) lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, 0);
            else if (fs <= 24) lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, 0);
            else lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, 0);
        }
        if (node.props.count("long_mode")) {
            lv_label_set_long_mode(obj, (lv_label_long_mode_t)propInt("long_mode"));
        }
    } else if (node.type == "LSlider") {
        if (node.props.count("range")) {
            auto list = node.props.at("range").toList();
            if (list.size() == 2) lv_slider_set_range(obj, list[0].toInt(), list[1].toInt());
        }
        if (node.props.count("value")) lv_slider_set_value(obj, propInt("value"), LV_ANIM_OFF);
        if (node.props.count("left_value") && node.props.count("range")) {
            auto list = node.props.at("range").toList();
            if (list.size() == 2) lv_slider_set_left_value(obj, propInt("left_value"), LV_ANIM_OFF);
        }
        if (node.props.count("mode")) lv_slider_set_mode(obj, (lv_slider_mode_t)propInt("mode"));
    } else if (node.type == "LBar") {
        if (node.props.count("range")) {
            auto list = node.props.at("range").toList();
            if (list.size() == 2) lv_bar_set_range(obj, list[0].toInt(), list[1].toInt());
        }
        if (node.props.count("value")) lv_bar_set_value(obj, propInt("value"), LV_ANIM_OFF);
        if (node.props.count("start_value")) lv_bar_set_start_value(obj, propInt("start_value"), LV_ANIM_OFF);
        if (node.props.count("mode")) lv_bar_set_mode(obj, (lv_bar_mode_t)propInt("mode"));
        if (node.props.count("orientation")) {
            if (propInt("orientation") == 1) {
                int w = lv_obj_get_width(obj);
                int h = lv_obj_get_height(obj);
                lv_obj_set_size(obj, h, w);
            }
        }
    } else if (node.type == "LCheckbox") {
        if (node.props.count("text")) lv_checkbox_set_text(obj, propStr("text").c_str());
        if (node.props.count("checked") && propBool("checked")) {
            lv_obj_add_state(obj, LV_STATE_CHECKED);
        }
    } else if (node.type == "LSwitch") {
        if (node.props.count("checked") && propBool("checked")) {
            lv_obj_add_state(obj, LV_STATE_CHECKED);
        }
        if (node.props.count("enabled") && !propBool("enabled")) {
            lv_obj_add_state(obj, LV_STATE_DISABLED);
        }
    } else if (node.type == "LComboBox") {
        if (node.props.count("options")) {
            std::string opts = propStr("options");
            size_t pos = 0;
            while ((pos = opts.find("\\n")) != std::string::npos) {
                opts.replace(pos, 2, "\n");
            }
            lv_dropdown_set_options(obj, opts.c_str());
        }
        if (node.props.count("selected")) lv_dropdown_set_selected(obj, propInt("selected"));
        if (node.props.count("direction")) {
            int dir = propInt("direction");
            /* 0 是 LV_DIR_NONE：设成它会让 lv_dropdown_open 里四个对齐分支全不匹配，
             * 下拉列表跑到画布左上角。0 视为"用默认方向"（下方）。 */
            if (dir != 0) lv_dropdown_set_dir(obj, (lv_dir_t)dir);
        }
        if (node.props.count("symbol")) {
            std::string sym = propStr("symbol");
            if (!sym.empty()) lv_dropdown_set_symbol(obj, intern(sym));   // LVGL 只存指针，必须用池化字符串
        }
        if (node.props.count("text")) {
            std::string t = propStr("text");
            /* text 非空才覆盖：一旦设置过 text，LVGL 就不再显示"已选项"的文本，
             * 空串会导致下拉框永远显示为空。 */
            if (!t.empty()) lv_dropdown_set_text(obj, t.c_str());
        }
        if (node.props.count("selected_highlight")) lv_dropdown_set_selected_highlight(obj, propBool("selected_highlight"));
        if (node.props.count("enabled") && !propBool("enabled")) lv_obj_add_state(obj, LV_STATE_DISABLED);
    } else if (node.type == "LLineEdit") {
        if (node.props.count("text")) lv_textarea_set_text(obj, propStr("text").c_str());
        if (node.props.count("placeholder")) lv_textarea_set_placeholder_text(obj, propStr("placeholder").c_str());
        if (node.props.count("password_mode")) lv_textarea_set_password_mode(obj, propBool("password_mode"));
        if (node.props.count("max_length")) lv_textarea_set_max_length(obj, propInt("max_length"));
        if (node.props.count("password_show_time")) lv_textarea_set_password_show_time(obj, propInt("password_show_time"));
        if (node.props.count("accepted_chars")) {
            std::string chars = propStr("accepted_chars");
            if (!chars.empty()) lv_textarea_set_accepted_chars(obj, chars.c_str());
        }
        if (node.props.count("cursor_click_pos")) lv_textarea_set_cursor_click_pos(obj, propBool("cursor_click_pos"));
        if (node.props.count("text_selection")) lv_textarea_set_text_selection(obj, propBool("text_selection"));
        if (node.props.count("enabled") && !propBool("enabled")) lv_obj_add_state(obj, LV_STATE_DISABLED);
    } else if (node.type == "LTextEdit") {
        if (node.props.count("text")) lv_textarea_set_text(obj, propStr("text").c_str());
        if (node.props.count("placeholder")) lv_textarea_set_placeholder_text(obj, propStr("placeholder").c_str());
        if (node.props.count("password_mode")) lv_textarea_set_password_mode(obj, propBool("password_mode"));
        if (node.props.count("max_length")) lv_textarea_set_max_length(obj, propInt("max_length"));
        if (node.props.count("password_show_time")) lv_textarea_set_password_show_time(obj, propInt("password_show_time"));
        if (node.props.count("accepted_chars")) {
            std::string chars = propStr("accepted_chars");
            if (!chars.empty()) lv_textarea_set_accepted_chars(obj, chars.c_str());
        }
        if (node.props.count("cursor_click_pos")) lv_textarea_set_cursor_click_pos(obj, propBool("cursor_click_pos"));
        if (node.props.count("text_selection")) lv_textarea_set_text_selection(obj, propBool("text_selection"));
        if (node.props.count("enabled") && !propBool("enabled")) lv_obj_add_state(obj, LV_STATE_DISABLED);
    } else if (node.type == "LRadioButton") {
        lv_obj_t* label = lv_obj_get_child(obj, 1);
        if (label && node.props.count("text")) lv_label_set_text(label, propStr("text").c_str());
        if (node.props.count("checked") && propBool("checked")) lv_obj_add_state(obj, LV_STATE_CHECKED);
        else lv_obj_remove_state(obj, LV_STATE_CHECKED);
    } else if (node.type == "LGroupBox") {
        if (node.props.count("padding") && !node.props.count("pad_top") && !node.props.count("pad_bottom") &&
            !node.props.count("pad_left") && !node.props.count("pad_right"))
            lv_obj_set_style_pad_all(obj, propInt("padding"), 0);
        if (node.props.count("title")) {
            // GroupBox title rendered as a child label
            lv_obj_t* title = lv_label_create(obj);
            lv_label_set_text(title, propStr("title").c_str());
            lv_obj_set_pos(title, 8, -4);
        }
    } else if (node.type == "LImage") {
        if (node.props.count("src")) {
            std::string src = resolveSrc(propStr("src"));
            if (!src.empty()) lv_image_set_src(obj, intern(src));
        }
        if (node.props.count("rotation")) lv_image_set_rotation(obj, propInt("rotation"));
        if (node.props.count("scale")) lv_image_set_scale(obj, propInt("scale"));
        /* 256 表示该轴不单独缩放（跟随 scale 等比缩放）；否则 scale 永远被默认值覆盖 */
        if (node.props.count("scale_x") && propInt("scale_x") != 256) lv_image_set_scale_x(obj, propInt("scale_x"));
        if (node.props.count("scale_y") && propInt("scale_y") != 256) lv_image_set_scale_y(obj, propInt("scale_y"));
        if (node.props.count("pivot")) {
            auto list = node.props.at("pivot").toList();
            if (list.size() == 2) lv_image_set_pivot(obj, list[0].toInt(), list[1].toInt());
        }
        if (node.props.count("offset")) {
            auto list = node.props.at("offset").toList();
            if (list.size() == 2) {
                lv_image_set_offset_x(obj, list[0].toInt());
                lv_image_set_offset_y(obj, list[1].toInt());
            }
        }
        if (node.props.count("antialias")) lv_image_set_antialias(obj, propBool("antialias"));
        if (node.props.count("inner_align")) lv_image_set_inner_align(obj, (lv_image_align_t)propInt("inner_align"));
        if (node.props.count("blend_mode")) lv_image_set_blend_mode(obj, (lv_blend_mode_t)propInt("blend_mode"));
        if (node.props.count("opacity")) lv_obj_set_style_image_opa(obj, propInt("opacity"), 0);
    } else if (node.type == "LArc") {
        if (node.props.count("range")) {
            auto list = node.props.at("range").toList();
            if (list.size() == 2) lv_arc_set_range(obj, list[0].toInt(), list[1].toInt());
        }
        if (node.props.count("value")) lv_arc_set_value(obj, propInt("value"));
        if (node.props.count("rotation")) lv_arc_set_rotation(obj, propInt("rotation"));
        if (node.props.count("mode")) lv_arc_set_mode(obj, (lv_arc_mode_t)propInt("mode"));
        if (node.props.count("bg_angles")) {
            auto list = node.props.at("bg_angles").toList();
            if (list.size() == 2) lv_arc_set_bg_angles(obj, list[0].toInt(), list[1].toInt());
        }
        if (node.props.count("change_rate")) lv_arc_set_change_rate(obj, propInt("change_rate"));
        if (node.props.count("angles")) {
            auto list = node.props.at("angles").toList();
            if (list.size() == 2) lv_arc_set_angles(obj, list[0].toInt(), list[1].toInt());
        }
        if (node.props.count("knob_offset")) lv_arc_set_knob_offset(obj, propInt("knob_offset"));
    } else if (node.type == "LSpinner") {
        if (node.props.count("color")) lv_obj_set_style_arc_color(obj, lv_color_hex(propHex("color")), LV_PART_INDICATOR);
        if (node.props.count("anim_speed") || node.props.count("anim_period")) {
            uint32_t period = propInt("anim_period", 1000);
            if (node.props.count("anim_speed")) {
                uint32_t speed = propInt("anim_speed", 1000);
                period = speed;
            }
            lv_spinner_set_anim_params(obj, period, 60);
        }
    } else if (node.type == "LLed") {
        if (node.props.count("color")) lv_led_set_color(obj, lv_color_hex(propHex("color")));
        /* lv_led_on() 内部会把亮度置为最大值，必须先做开关再设亮度，
         * 否则 brightness 永远被覆盖（实测像素完全不变）。 */
        if (node.props.count("on") ? propBool("on") : true) {
            lv_led_on(obj);
            if (node.props.count("brightness")) lv_led_set_brightness(obj, propInt("brightness"));
        } else {
            lv_led_off(obj);
        }
    } else if (node.type == "LLine") {
        if (node.props.count("points")) {
            std::vector<lv_point_precise_t>& pts = storePoints();
            for (auto& p : splitStr(unescapeNewlines(propStr("points")), ';')) {
                std::string t = trimStr(p);
                if (t.empty()) continue;
                auto xy = splitStr(t, ',');
                if (xy.size() < 2) continue;
                lv_point_precise_t pt;
                pt.x = atoi(trimStr(xy[0]).c_str());
                pt.y = atoi(trimStr(xy[1]).c_str());
                pts.push_back(pt);
            }
            if (!pts.empty()) lv_line_set_points(obj, pts.data(), pts.size());
        }
        if (node.props.count("width")) lv_obj_set_style_line_width(obj, propInt("width"), 0);
        if (node.props.count("color")) lv_obj_set_style_line_color(obj, lv_color_hex(propHex("color")), 0);
        if (node.props.count("y_invert")) lv_line_set_y_invert(obj, propBool("y_invert"));
        if (node.props.count("dash_width")) lv_obj_set_style_line_dash_width(obj, propInt("dash_width"), 0);
        if (node.props.count("dash_gap")) lv_obj_set_style_line_dash_gap(obj, propInt("dash_gap"), 0);
        if (node.props.count("rounded")) lv_obj_set_style_line_rounded(obj, propBool("rounded"), 0);
    } else if (node.type == "LTable") {
        if (node.props.count("row_count") && node.props.count("column_count")) {
            lv_table_set_row_count(obj, propInt("row_count"));
            lv_table_set_column_count(obj, propInt("column_count"));
        }
        if (node.props.count("column_widths")) {
            int idx = 0;
            for (auto& tok : splitStr(propStr("column_widths"), ',')) {
                std::string t = trimStr(tok);
                if (!t.empty()) lv_table_set_column_width(obj, idx, atoi(t.c_str()));
                idx++;
            }
        }
        if (node.props.count("data")) {
            int r = 0;
            for (auto& row : splitStr(unescapeNewlines(propStr("data")), '\n')) {
                int c = 0;
                for (auto& cell : splitStr(row, '|')) {
                    if (!cell.empty()) lv_table_set_cell_value(obj, r, c, cell.c_str());
                    c++;
                }
                r++;
            }
        }
    } else if (node.type == "LChart") {
        if (node.props.count("type")) lv_chart_set_type(obj, (lv_chart_type_t)propInt("type"));
        if (node.props.count("point_count")) lv_chart_set_point_count(obj, propInt("point_count"));
        if (node.props.count("div_line_count")) {
            auto list = node.props.at("div_line_count").toList();
            if (list.size() == 2) lv_chart_set_div_line_count(obj, list[0].toInt(), list[1].toInt());
        }
        if (node.props.count("update_mode")) lv_chart_set_update_mode(obj, (lv_chart_update_mode_t)propInt("update_mode"));
        if (node.props.count("series")) {
            for (auto& ser : splitStr(unescapeNewlines(propStr("series")), ';')) {
                if (ser.empty()) continue;
                lv_chart_series_t* sp = lv_chart_add_series(obj, lv_color_hex(0xFF0000), LV_CHART_AXIS_PRIMARY_Y);
                for (auto& v : splitStr(ser, ',')) {
                    std::string t = trimStr(v);
                    if (!t.empty()) lv_chart_set_next_value(obj, sp, atoi(t.c_str()));
                }
            }
        }
    } else if (node.type == "LScale") {
        if (node.props.count("mode")) lv_scale_set_mode(obj, (lv_scale_mode_t)propInt("mode"));
        if (node.props.count("range")) {
            auto list = node.props.at("range").toList();
            if (list.size() == 2) lv_scale_set_range(obj, list[0].toInt(), list[1].toInt());
        }
        if (node.props.count("total_tick_count")) lv_scale_set_total_tick_count(obj, propInt("total_tick_count"));
        if (node.props.count("major_tick_every")) lv_scale_set_major_tick_every(obj, propInt("major_tick_every"));
        if (node.props.count("label_show")) lv_scale_set_label_show(obj, propBool("label_show"));
        if (node.props.count("rotation")) lv_scale_set_rotation(obj, propInt("rotation"));
        if (node.props.count("angle_range")) lv_scale_set_angle_range(obj, propInt("angle_range"));
        if (node.props.count("draw_ticks_on_top")) lv_scale_set_draw_ticks_on_top(obj, propBool("draw_ticks_on_top"));
    } else if (node.type == "LKeyboard") {
        if (node.props.count("mode")) lv_keyboard_set_mode(obj, (lv_keyboard_mode_t)propInt("mode"));
        if (node.props.count("popovers")) lv_keyboard_set_popovers(obj, propBool("popovers"));
    } else if (node.type == "LRoller") {
        if (node.props.count("options")) {
            std::string opts = propStr("options");
            size_t pos = 0;
            while ((pos = opts.find("\\n")) != std::string::npos) {
                opts.replace(pos, 2, "\n");
            }
            lv_roller_set_options(obj, opts.c_str(), LV_ROLLER_MODE_NORMAL);
        }
        if (node.props.count("selected")) lv_roller_set_selected(obj, propInt("selected"), LV_ANIM_OFF);
        if (node.props.count("visible_row_count")) lv_roller_set_visible_row_count(obj, propInt("visible_row_count"));
    } else if (node.type == "LVBoxLayout" || node.type == "LHBoxLayout") {
        if (node.props.count("spacing")) {
            if (node.type == "LVBoxLayout")
                lv_obj_set_style_pad_row(obj, propInt("spacing"), 0);
            else
                lv_obj_set_style_pad_column(obj, propInt("spacing"), 0);
        }
        if (node.props.count("padding") &&
            !node.props.count("pad_top") && !node.props.count("pad_bottom") &&
            !node.props.count("pad_left") && !node.props.count("pad_right")) {
            lv_obj_set_style_pad_all(obj, propInt("padding"), 0);
        }
        if (node.props.count("align_cross") || node.props.count("align_items")) {
            lv_flex_align_t align = (lv_flex_align_t)(node.props.count("align_cross")
                ? propInt("align_cross")
                : propInt("align_items"));
            /* LVGL 的 cross_place 只把子项对齐到"轨道"内部，而轨道宽度等于最宽子项，
             * 等宽子项时看不出变化；轨道对齐一并设置才符合"交叉轴对齐"的直觉。 */
            lv_obj_set_flex_align(obj,
                                  lv_obj_get_style_flex_main_place(obj, 0),
                                  align,
                                  align);
        }
    } else if (node.type == "LGridLayout") {
        if (node.props.count("padding") && !node.props.count("pad_top") && !node.props.count("pad_bottom") &&
            !node.props.count("pad_left") && !node.props.count("pad_right"))
            lv_obj_set_style_pad_all(obj, propInt("padding"), 0);
        if (node.props.count("col_spacing")) lv_obj_set_style_pad_column(obj, propInt("col_spacing"), 0);
        if (node.props.count("row_spacing")) lv_obj_set_style_pad_row(obj, propInt("row_spacing"), 0);
        int cols = node.props.count("columns") ? propInt("columns") : 0;
        int rows = node.props.count("rows") ? propInt("rows") : 0;
        if (cols > 0 && rows > 0) {
            std::vector<lv_coord_t> col_dsc(cols + 1);
            for (int i = 0; i < cols; ++i) col_dsc[i] = LV_GRID_FR(1);
            col_dsc[cols] = LV_GRID_TEMPLATE_LAST;

            std::vector<lv_coord_t> row_dsc(rows + 1);
            for (int i = 0; i < rows; ++i) row_dsc[i] = LV_GRID_FR(1);
            row_dsc[rows] = LV_GRID_TEMPLATE_LAST;

            lv_obj_set_grid_dsc_array(obj, col_dsc.data(), row_dsc.data());
            m_gridDescArrays.push_back(std::move(col_dsc));
            m_gridDescArrays.push_back(std::move(row_dsc));
        }
    } else if (node.type == "LStackedLayout") {
        if (node.props.count("padding") && !node.props.count("pad_top") && !node.props.count("pad_bottom") &&
            !node.props.count("pad_left") && !node.props.count("pad_right"))
            lv_obj_set_style_pad_all(obj, node.propInt("padding", 0), 0);
    } else if (node.type == "LTabView") {
        if (node.props.count("tabs")) {
            for (auto& name : splitStr(unescapeNewlines(propStr("tabs")), '\n')) {
                if (!name.empty()) lv_tabview_add_tab(obj, name.c_str());
            }
        }
        if (node.props.count("tab_bar_position")) {
            lv_dir_t dir = (lv_dir_t)propInt("tab_bar_position");
            lv_tabview_set_tab_bar_position(obj, dir);
        }
        if (node.props.count("tab_bar_size")) {
            lv_tabview_set_tab_bar_size(obj, propInt("tab_bar_size"));
        }
    } else if (node.type == "LList") {
        if (node.props.count("items")) {
            for (auto& line : splitStr(unescapeNewlines(propStr("items")), '\n')) {
                if (line.empty()) continue;
                if (line[0] == '#') lv_list_add_text(obj, line.substr(1).c_str());
                else lv_list_add_button(obj, NULL, line.c_str());
            }
        }
    } else if (node.type == "LMenu") {
        if (node.props.count("mode_header")) {
            lv_menu_mode_header_t mode = (lv_menu_mode_header_t)propInt("mode_header");
            lv_menu_set_mode_header(obj, mode);
        }
        if (node.props.count("mode_root_back_button")) {
            lv_menu_mode_root_back_button_t mode = (lv_menu_mode_root_back_button_t)propInt("mode_root_back_button");
            lv_menu_set_mode_root_back_button(obj, mode);
        }
    } else if (node.type == "LSpinbox") {
        if (node.props.count("range")) {
            auto list = node.props.at("range").toList();
            if (list.size() == 2) lv_spinbox_set_range(obj, list[0].toInt(), list[1].toInt());
        }
        if (node.props.count("value")) lv_spinbox_set_value(obj, propInt("value"));
        /* lv_spinbox_set_cursor_pos 会按光标所在位重算 step，必须放在 step 之前，
         * 否则用户设置的 step 一定被重置为 1。 */
        if (node.props.count("cursor_pos")) lv_spinbox_set_cursor_pos(obj, propInt("cursor_pos"));
        if (node.props.count("step")) lv_spinbox_set_step(obj, propInt("step"));
        if (node.props.count("digit_format")) {
            auto list = node.props.at("digit_format").toList();
            if (list.size() == 2) lv_spinbox_set_digit_format(obj, list[0].toUInt(), list[1].toUInt());
        }
        if (node.props.count("rollover")) lv_spinbox_set_rollover(obj, propBool("rollover"));
        if (node.props.count("digit_step_direction")) lv_spinbox_set_digit_step_direction(obj, (lv_dir_t)propInt("digit_step_direction"));
    } else if (node.type == "LButtonMatrix") {
        if (node.props.count("map")) {
            std::vector<const char*>& arr = storeMap();
            for (auto& row : splitStr(unescapeNewlines(propStr("map")), '\n')) {
                if (!arr.empty()) arr.push_back(intern("\n"));
                for (auto& btn : splitStr(row, ' ')) {
                    if (!btn.empty()) arr.push_back(intern(btn));
                }
            }
            arr.push_back(nullptr);
            lv_buttonmatrix_set_map(obj, arr.data());
        }
        if (node.props.count("selected")) {
            lv_buttonmatrix_set_selected_button(obj, propInt("selected"));
        }
        if (node.props.count("one_checked") && propBool("one_checked")) {
            lv_buttonmatrix_set_one_checked(obj, true);
        }
    } else if (node.type == "LCalendar") {
        if (node.props.count("today_date")) {
            auto list = node.props.at("today_date").toList();
            if (list.size() == 3) lv_calendar_set_today_date(obj, list[0].toUInt(), list[1].toUInt(), list[2].toUInt());
        }
        if (node.props.count("showed_date")) {
            auto list = node.props.at("showed_date").toList();
            if (list.size() == 2) lv_calendar_set_showed_date(obj, list[0].toUInt(), list[1].toUInt());
        }
    } else if (node.type == "LSpan") {
        if (node.props.count("spans")) {
            for (auto& line : splitStr(unescapeNewlines(propStr("spans")), '\n')) {
                if (line.empty()) continue;
                lv_span_t* sp = lv_spangroup_new_span(obj);
                lv_span_set_text(sp, line.c_str());
            }
        }
        if (node.props.count("mode")) lv_spangroup_set_mode(obj, (lv_span_mode_t)propInt("mode"));
        if (node.props.count("overflow")) lv_spangroup_set_overflow(obj, (lv_span_overflow_t)propInt("overflow"));
        if (node.props.count("indent")) lv_spangroup_set_indent(obj, propInt("indent"));
    } else if (node.type == "LMsgBox") {
        if (node.props.count("title")) lv_msgbox_add_title(obj, propStr("title").c_str());
        if (node.props.count("text")) lv_msgbox_add_text(obj, propStr("text").c_str());
    } else if (node.type == "LAnimImage") {
        if (node.props.count("duration")) lv_animimg_set_duration(obj, propInt("duration"));
        if (node.props.count("repeat_count")) lv_animimg_set_repeat_count(obj, propInt("repeat_count"));
    } else if (node.type == "LImageButton") {
        if (node.props.count("state")) {
            lv_imagebutton_set_state(obj, (lv_imagebutton_state_t)propInt("state"));
        }
        auto setSrc = [&](const char* key, lv_imagebutton_state_t st) {
            if (!node.props.count(key)) return;
            std::string s = resolveSrc(propStr(key));
            if (!s.empty()) lv_imagebutton_set_src(obj, st, nullptr, intern(s), nullptr);
        };
        setSrc("src_released", LV_IMAGEBUTTON_STATE_RELEASED);
        setSrc("src_pressed",  LV_IMAGEBUTTON_STATE_PRESSED);
        setSrc("src_disabled", LV_IMAGEBUTTON_STATE_DISABLED);
    } else if (node.type == "LWin") {
        if (node.props.count("title")) {
            lv_win_add_title(obj, propStr("title").c_str());
        }
    }
}

bool LvglRenderer::isLayoutChild(lv_obj_t* obj) const {
    if (!obj) return false;
    lv_obj_t* parent = lv_obj_get_parent(obj);
    if (!parent || parent == lv_scr_act()) return false;
    auto it = m_idMap.find(parent);
    if (it == m_idMap.end()) return false;
    auto typeIt = m_types.find(it->second);
    if (typeIt == m_types.end()) return false;
    const auto& t = typeIt->second;
    return t == "LVBoxLayout" || t == "LHBoxLayout" ||
           t == "LGridLayout" || t == "LStackedLayout";
}

void LvglRenderer::applyStackedVisibility(lv_obj_t* stackedObj, int currentIndex) {
    if (!stackedObj) return;
    lv_obj_update_layout(stackedObj);

    auto navIt = m_stackedNavs.find(m_idMap.count(stackedObj) ? m_idMap[stackedObj] : "");
    bool hasNav = (navIt != m_stackedNavs.end());

    int contentChildCount = 0;
    int totalChildCount = lv_obj_get_child_count(stackedObj);
    for (int i = 0; i < totalChildCount; i++) {
        lv_obj_t* child = lv_obj_get_child(stackedObj, i);
        if (!child) continue;
        if (hasNav && child == navIt->second.navBar) continue;
        contentChildCount++;
    }

    if (contentChildCount <= 0) return;
    if (currentIndex < 0 || currentIndex >= contentChildCount) currentIndex = 0;

    lv_area_t contentArea;
    lv_obj_get_content_coords(stackedObj, &contentArea);
    int width = contentArea.x2 - contentArea.x1 + 1;
    int height = contentArea.y2 - contentArea.y1 + 1;
    if (width < 0) width = 0;
    if (height < 0) height = 0;

    int contentIdx = 0;
    for (int i = 0; i < totalChildCount; i++) {
        lv_obj_t* child = lv_obj_get_child(stackedObj, i);
        if (!child) continue;
        if (hasNav && child == navIt->second.navBar) continue;
        lv_obj_set_pos(child, 0, 0);
        lv_obj_set_size(child, width, height);
        if (contentIdx == currentIndex) {
            lv_obj_remove_flag(child, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(child, LV_OBJ_FLAG_HIDDEN);
        }
        contentIdx++;
    }

    if (hasNav) {
        navIt->second.currentIndex = currentIndex;
        navIt->second.childCount = contentChildCount;
        lv_obj_move_foreground(navIt->second.navBar);
    }
}

void LvglRenderer::createStackedNav(lv_obj_t* stackedObj, const std::string& id, int currentIndex) {
    if (!stackedObj) return;

    StackedNav nav;
    nav.currentIndex = currentIndex;
    nav.childCount = lv_obj_get_child_count(stackedObj);

    lv_obj_t* navBar = lv_obj_create(stackedObj);
    lv_obj_set_size(navBar, LV_SIZE_CONTENT, 26);
    lv_obj_align(navBar, LV_ALIGN_TOP_RIGHT, -4, 4);
    lv_obj_set_flex_flow(navBar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(navBar, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(navBar, 2, 0);
    lv_obj_set_style_pad_column(navBar, 4, 0);
    lv_obj_set_style_bg_color(navBar, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(navBar, LV_OPA_30, 0);
    lv_obj_set_style_radius(navBar, 4, 0);
    lv_obj_set_style_border_width(navBar, 0, 0);
    nav.navBar = navBar;

    lv_obj_t* leftBtn = lv_button_create(navBar);
    lv_obj_set_size(leftBtn, 20, 20);
    lv_obj_set_style_bg_color(leftBtn, lv_color_hex(0x666666), 0);
    lv_obj_set_style_bg_opa(leftBtn, LV_OPA_20, 0);
    lv_obj_set_style_radius(leftBtn, 3, 0);
    lv_obj_set_style_border_width(leftBtn, 0, 0);
    lv_obj_set_style_pad_all(leftBtn, 0, 0);
    lv_obj_t* leftLabel = lv_label_create(leftBtn);
    lv_label_set_text(leftLabel, LV_SYMBOL_LEFT);
    lv_obj_center(leftLabel);
    lv_obj_add_event_cb(leftBtn, [](lv_event_t* e) {
        auto* self = static_cast<LvglRenderer*>(lv_event_get_user_data(e));
        if (!self) return;
        lv_obj_t* btn = static_cast<lv_obj_t*>(lv_event_get_target(e));
        lv_obj_t* navBar = lv_obj_get_parent(btn);
        if (!navBar) return;
        lv_obj_t* stacked = lv_obj_get_parent(navBar);
        if (!stacked) return;
        auto it = self->m_idMap.find(stacked);
        if (it == self->m_idMap.end()) return;
        auto navIt = self->m_stackedNavs.find(it->second);
        if (navIt == self->m_stackedNavs.end()) return;
        navIt->second.currentIndex--;
        if (navIt->second.currentIndex < 0)
            navIt->second.currentIndex = navIt->second.childCount - 1;
        self->applyStackedVisibility(stacked, navIt->second.currentIndex);
        self->updateStackedNav(it->second);
    }, LV_EVENT_CLICKED, this);
    nav.leftBtn = leftBtn;

    lv_obj_t* indexLabel = lv_label_create(navBar);
    lv_obj_set_style_text_color(indexLabel, lv_color_hex(0xCCCCCC), 0);
    lv_obj_set_style_text_font(indexLabel, &lv_font_montserrat_12, 0);
    nav.indexLabel = indexLabel;

    lv_obj_t* rightBtn = lv_button_create(navBar);
    lv_obj_set_size(rightBtn, 20, 20);
    lv_obj_set_style_bg_color(rightBtn, lv_color_hex(0x666666), 0);
    lv_obj_set_style_bg_opa(rightBtn, LV_OPA_20, 0);
    lv_obj_set_style_radius(rightBtn, 3, 0);
    lv_obj_set_style_border_width(rightBtn, 0, 0);
    lv_obj_set_style_pad_all(rightBtn, 0, 0);
    lv_obj_t* rightLabel = lv_label_create(rightBtn);
    lv_label_set_text(rightLabel, LV_SYMBOL_RIGHT);
    lv_obj_center(rightLabel);
    lv_obj_add_event_cb(rightBtn, [](lv_event_t* e) {
        auto* self = static_cast<LvglRenderer*>(lv_event_get_user_data(e));
        if (!self) return;
        lv_obj_t* btn = static_cast<lv_obj_t*>(lv_event_get_target(e));
        lv_obj_t* navBar = lv_obj_get_parent(btn);
        if (!navBar) return;
        lv_obj_t* stacked = lv_obj_get_parent(navBar);
        if (!stacked) return;
        auto it = self->m_idMap.find(stacked);
        if (it == self->m_idMap.end()) return;
        auto navIt = self->m_stackedNavs.find(it->second);
        if (navIt == self->m_stackedNavs.end()) return;
        navIt->second.currentIndex++;
        if (navIt->second.currentIndex >= navIt->second.childCount)
            navIt->second.currentIndex = 0;
        self->applyStackedVisibility(stacked, navIt->second.currentIndex);
        self->updateStackedNav(it->second);
    }, LV_EVENT_CLICKED, this);
    nav.rightBtn = rightBtn;

    m_stackedNavs[id] = nav;
    updateStackedNav(id);
}

void LvglRenderer::updateStackedNav(const std::string& id) {
    auto it = m_stackedNavs.find(id);
    if (it == m_stackedNavs.end()) return;
    const auto& nav = it->second;
    if (!nav.indexLabel) return;
    int displayIndex = nav.currentIndex + 1;
    if (displayIndex < 1) displayIndex = 1;
    std::string text = std::to_string(displayIndex) + " / " + std::to_string(nav.childCount);
    lv_label_set_text(nav.indexLabel, text.c_str());
}

/* 把实例上的参数应用到底层组件的对应节点上：
 * 导出是 "<节点id>.<属性名>"，这里复用 applyProps，保证与组件内部属性的应用方式一致。 */
void LvglRenderer::applyInstanceExports(lv_obj_t* instanceRoot, const WidgetNode& node,
                                        const LuiDocument& componentDoc) {
    std::function<const WidgetNode*(const std::vector<WidgetNode>&, const std::string&)> findNode =
        [&](const std::vector<WidgetNode>& nodes, const std::string& id) -> const WidgetNode* {
            for (const auto& n : nodes) {
                if (n.id == id) return &n;
                if (auto* r = findNode(n.children, id)) return r;
            }
            return nullptr;
        };

    for (const auto& kv : node.props) {
        const std::string& key = kv.first;
        if (key == "pos" || key == "size" || key == "align" || key == "hidden" || key == "clickable")
            continue;
        auto ex = componentDoc.exports().find(key);
        if (ex == componentDoc.exports().end()) continue;      // 普通样式属性，已由 applyProps 处理

        std::string path = ex->second;
        auto dot = path.find('.');
        if (dot == std::string::npos) continue;
        std::string targetId = path.substr(0, dot);
        std::string propName = path.substr(dot + 1);
        if (targetId.rfind("m_", 0) == 0) targetId = targetId.substr(2);

        lv_obj_t* targetObj = nullptr;
        WidgetNode tmp;
        if (targetId == "root") {
            targetObj = instanceRoot;
            tmp.type = "LObj";
        } else {
            auto objIt = m_instanceObjects.find(targetId);
            if (objIt == m_instanceObjects.end()) continue;
            targetObj = objIt->second;
            const WidgetNode* tn = findNode(componentDoc.children(), targetId);
            if (!tn) continue;
            tmp.type = tn->type;
        }
        tmp.id = targetId;
        tmp.props[propName] = kv.second;
        applyProps(targetObj, tmp);
    }
}

void LvglRenderer::registerObject(const std::string& id, const std::string& type, lv_obj_t* obj) {
    m_objects[id] = obj;
    m_idMap[obj] = id;
    m_types[id] = type;

    if (type == "LVBoxLayout" || type == "LHBoxLayout" || type == "LGridLayout"
        || type == "LStackedLayout") {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    }

    lv_obj_add_event_cb(obj, [](lv_event_t* e) {
        auto* self = static_cast<LvglRenderer*>(lv_event_get_user_data(e));
        if (!self) return;
        lv_obj_t* target = static_cast<lv_obj_t*>(lv_event_get_target(e));
        /* 组件实例内部的子对象没有注册：向上找到最近的已注册对象（实例根） */
        while (target && self->m_idMap.find(target) == self->m_idMap.end())
            target = lv_obj_get_parent(target);
        if (!target) return;
        lv_obj_t* selectionTarget = target;

        lv_obj_t* parent = lv_obj_get_parent(target);
        if (parent && parent != lv_scr_act()) {
            auto parentIt = self->m_idMap.find(parent);
            if (parentIt != self->m_idMap.end()) {
                auto typeIt = self->m_types.find(parentIt->second);
                if (typeIt != self->m_types.end() && typeIt->second == "LStackedLayout") {
                    selectionTarget = parent;
                }
            }
        }

        if (selectionTarget == self->m_selectedObj) return;
        auto it = self->m_idMap.find(selectionTarget);
        if (it != self->m_idMap.end()) {
            PendingEvent evt;
            evt.type = PendingEvent::Click;
            evt.id = it->second;
            self->m_pendingMutex.lock();
            self->m_pendingEvents.push_back(std::move(evt));
            self->m_pendingMutex.unlock();
        }
    }, LV_EVENT_PRESSED, this);

    lv_obj_add_event_cb(obj, [](lv_event_t* e) {
        auto* self = static_cast<LvglRenderer*>(lv_event_get_user_data(e));
        if (!self) return;
        lv_obj_t* target = static_cast<lv_obj_t*>(lv_event_get_target(e));
        if (target != self->m_selectedObj) return;
        if (self->m_handleDragging) return;

        lv_obj_t* parent = lv_obj_get_parent(target);
        if (parent && parent != lv_scr_act()) {
            auto parentIt = self->m_idMap.find(parent);
            if (parentIt != self->m_idMap.end()) {
                auto typeIt = self->m_types.find(parentIt->second);
                if (typeIt != self->m_types.end() &&
                    (typeIt->second == "LVBoxLayout" || typeIt->second == "LHBoxLayout" ||
                     typeIt->second == "LGridLayout" || typeIt->second == "LStackedLayout")) {
                    return;
                }
            }
        }

        lv_indev_t* indev = lv_indev_active();
        if (!indev) return;
        lv_point_t point;
        lv_indev_get_point(indev, &point);

        if (!self->m_widgetMoving) {
            self->m_widgetMoving = true;
            self->m_widgetOrigX = lv_obj_get_x(target);
            self->m_widgetOrigY = lv_obj_get_y(target);
            self->m_mouseStartX = point.x;
            self->m_mouseStartY = point.y;
            self->m_widgetMoved = false;
        }

        int dx = point.x - self->m_mouseStartX;
        int dy = point.y - self->m_mouseStartY;

        if (!self->m_widgetMoved && (dx * dx + dy * dy) < 16) return;
        self->m_widgetMoved = true;

        lv_obj_set_pos(target, self->m_widgetOrigX + dx, self->m_widgetOrigY + dy);
        self->updateHandlePositions();
    }, LV_EVENT_PRESSING, this);

    lv_obj_add_event_cb(obj, [](lv_event_t* e) {
        auto* self = static_cast<LvglRenderer*>(lv_event_get_user_data(e));
        if (!self) return;
        lv_obj_t* target = static_cast<lv_obj_t*>(lv_event_get_target(e));
        if (target != self->m_selectedObj) {
            self->m_widgetMoving = false;
            return;
        }

        if (self->m_widgetMoving) {
            self->m_widgetMoving = false;
            if (self->m_widgetMoved) {
                auto idIt = self->m_idMap.find(target);
                if (idIt != self->m_idMap.end()) {
                    PendingEvent evt;
                    evt.type = PendingEvent::Moved;
                    evt.id = idIt->second;
                    evt.x = lv_obj_get_x(target);
                    evt.y = lv_obj_get_y(target);
                    evt.w = lv_obj_get_width(target);
                    evt.h = lv_obj_get_height(target);
                    self->m_pendingMutex.lock();
                    self->m_pendingEvents.push_back(std::move(evt));
                    self->m_pendingMutex.unlock();
                }
            }
        }
    }, LV_EVENT_RELEASED, this);

}

void LvglRenderer::pollEvents() {
    std::vector<PendingEvent> events;
    m_pendingMutex.lock();
    events.swap(m_pendingEvents);
    m_pendingMutex.unlock();

    for (auto& evt : events) {
        if (evt.type == PendingEvent::Click) {
            if (m_onClick) m_onClick(evt.id);
        } else if (evt.type == PendingEvent::Moved) {
            if (m_onMoved) m_onMoved(evt.id, evt.x, evt.y, evt.w, evt.h);
        }
    }
}

void LvglRenderer::buildTree(const LuiDocument& doc) {
    clear();

    /* 资源别名 -> 绝对路径，供预览时按 lv_fs 路径加载 */
    m_resAbs.clear();
    for (auto& kv : doc.resources()) {
        QString p = QString::fromStdString(kv.second);
        m_resAbs[kv.first] = (QDir::isAbsolutePath(p)
                              ? p
                              : QDir(doc.baseDir()).absoluteFilePath(p)).toStdString();
    }

    lv_obj_t* screen = lv_scr_act();

    m_objects[kRootId] = screen;
    m_idMap[screen] = kRootId;
    m_types[kRootId] = "LScreen";
    lv_obj_add_flag(screen, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(screen, [](lv_event_t* e) {
        auto* self = static_cast<LvglRenderer*>(lv_event_get_user_data(e));
        if (!self) return;
        if (lv_event_get_target(e) != lv_event_get_current_target(e)) return;
        if (self->m_selectedObj == lv_scr_act()) return;

        PendingEvent evt;
        evt.type = PendingEvent::Click;
        evt.id = kRootId;
        self->m_pendingMutex.lock();
        self->m_pendingEvents.push_back(std::move(evt));
        self->m_pendingMutex.unlock();
    }, LV_EVENT_PRESSED, this);

    if (doc.isComponent()) {
        /* 组件文档：预览时建一个透明根容器（与生成的 LObj 根一致） */
        lv_obj_t* root = lv_obj_create(screen);
        lv_obj_remove_style_all(root);
        lv_obj_remove_flag(root, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_pos(root, 20, 20);
        lv_obj_set_size(root, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        m_objects[kRootId] = root;
        m_idMap[root] = kRootId;
        m_types[kRootId] = "LComponentRoot";
        for (const auto& child : doc.children()) {
            createWidget(child, root);
        }
    } else {
        for (const auto& child : doc.children()) {
            createWidget(child, nullptr);
        }
    }

    /* 有些控件默认没有 LV_OBJ_FLAG_CLICKABLE（如 lv_line / lv_arc / lv_bar / lv_led /
     * lv_spinner / lv_image / lv_chart），LVGL 的命中判定会直接跳过它们，导致在设计器
     * 里点不中、无法选中也就改不了属性。给"没有子对象"的叶节点补上该标志；
     * 有子对象的（按钮等）不动，避免抢走本该由父控件接收的点击。 */
    std::set<std::string> explicitlyNotClickable;
    std::function<void(const std::vector<WidgetNode>&)> collectNotClickable =
        [&](const std::vector<WidgetNode>& nodes) {
            for (const auto& n : nodes) {
                auto it = n.props.find("clickable");
                if (it != n.props.end() && !it->second.toBool()) explicitlyNotClickable.insert(n.id);
                collectNotClickable(n.children);
            }
        };
    collectNotClickable(doc.children());
    for (auto& [id, obj] : m_objects) {
        if (obj && lv_obj_is_valid(obj) && lv_obj_get_child_count(obj) == 0 &&
            !explicitlyNotClickable.count(id))
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    }

    std::function<void(const std::vector<WidgetNode>&)> resolveRefs =
        [&](const std::vector<WidgetNode>& nodes) {
            for (const auto& n : nodes) {
                if (n.type == "LKeyboard" && n.props.count("textarea")) {
                    auto kb = m_objects.find(n.id);
                    auto ta = m_objects.find(n.propString("textarea"));
                    if (kb != m_objects.end() && ta != m_objects.end() && kb->second && ta->second)
                        lv_keyboard_set_textarea(kb->second, ta->second);
                }
                if (n.type == "LMenu" && n.props.count("active_page")) {
                    auto mn = m_objects.find(n.id);
                    auto pg = m_objects.find(n.propString("active_page"));
                    if (mn != m_objects.end() && pg != m_objects.end() && mn->second && pg->second)
                        lv_menu_set_page(mn->second, pg->second);
                }
                resolveRefs(n.children);
            }
        };
    resolveRefs(doc.children());
}


namespace {
/* 属性名 -> LVGL 样式属性枚举；用于读取对象当前实际生效的样式值 */
lv_style_prop_t stylePropId(const std::string& name) {
    static const std::map<std::string, lv_style_prop_t> m = {
        {"arc_color", LV_STYLE_ARC_COLOR},
        {"arc_opa", LV_STYLE_ARC_OPA},
        {"arc_rounded", LV_STYLE_ARC_ROUNDED},
        {"arc_width", LV_STYLE_ARC_WIDTH},
        {"base_dir", LV_STYLE_BASE_DIR},
        {"bg_color", LV_STYLE_BG_COLOR},
        {"bg_grad_color", LV_STYLE_BG_GRAD_COLOR},
        {"bg_grad_dir", LV_STYLE_BG_GRAD_DIR},
        {"bg_grad_stop", LV_STYLE_BG_GRAD_STOP},
        {"bg_image_opa", LV_STYLE_BG_IMAGE_OPA},
        {"bg_image_recolor", LV_STYLE_BG_IMAGE_RECOLOR},
        {"bg_image_recolor_opa", LV_STYLE_BG_IMAGE_RECOLOR_OPA},
        {"bg_image_src", LV_STYLE_BG_IMAGE_SRC},
        {"bg_image_tiled", LV_STYLE_BG_IMAGE_TILED},
        {"bg_main_stop", LV_STYLE_BG_MAIN_STOP},
        {"bg_opa", LV_STYLE_BG_OPA},
        {"blend_mode", LV_STYLE_BLEND_MODE},
        {"border_color", LV_STYLE_BORDER_COLOR},
        {"border_opa", LV_STYLE_BORDER_OPA},
        {"border_post", LV_STYLE_BORDER_POST},
        {"border_side", LV_STYLE_BORDER_SIDE},
        {"border_width", LV_STYLE_BORDER_WIDTH},
        {"clip_corner", LV_STYLE_CLIP_CORNER},
        {"image_opa", LV_STYLE_IMAGE_OPA},
        {"image_recolor", LV_STYLE_IMAGE_RECOLOR},
        {"image_recolor_opa", LV_STYLE_IMAGE_RECOLOR_OPA},
        {"line_color", LV_STYLE_LINE_COLOR},
        {"line_dash_gap", LV_STYLE_LINE_DASH_GAP},
        {"line_dash_width", LV_STYLE_LINE_DASH_WIDTH},
        {"line_opa", LV_STYLE_LINE_OPA},
        {"line_rounded", LV_STYLE_LINE_ROUNDED},
        {"line_width", LV_STYLE_LINE_WIDTH},
        {"margin_bottom", LV_STYLE_MARGIN_BOTTOM},
        {"margin_left", LV_STYLE_MARGIN_LEFT},
        {"margin_right", LV_STYLE_MARGIN_RIGHT},
        {"margin_top", LV_STYLE_MARGIN_TOP},
        {"max_height", LV_STYLE_MAX_HEIGHT},
        {"max_width", LV_STYLE_MAX_WIDTH},
        {"min_height", LV_STYLE_MIN_HEIGHT},
        {"min_width", LV_STYLE_MIN_WIDTH},
        {"opa", LV_STYLE_OPA},
        {"outline_color", LV_STYLE_OUTLINE_COLOR},
        {"outline_opa", LV_STYLE_OUTLINE_OPA},
        {"outline_pad", LV_STYLE_OUTLINE_PAD},
        {"outline_width", LV_STYLE_OUTLINE_WIDTH},
        {"pad_bottom", LV_STYLE_PAD_BOTTOM},
        {"pad_column", LV_STYLE_PAD_COLUMN},
        {"pad_left", LV_STYLE_PAD_LEFT},
        {"pad_right", LV_STYLE_PAD_RIGHT},
        {"pad_row", LV_STYLE_PAD_ROW},
        {"pad_top", LV_STYLE_PAD_TOP},
        {"radius", LV_STYLE_RADIUS},
        {"shadow_color", LV_STYLE_SHADOW_COLOR},
        {"shadow_opa", LV_STYLE_SHADOW_OPA},
        {"shadow_spread", LV_STYLE_SHADOW_SPREAD},
        {"shadow_width", LV_STYLE_SHADOW_WIDTH},
        {"text_align", LV_STYLE_TEXT_ALIGN},
        {"text_color", LV_STYLE_TEXT_COLOR},
        {"text_decor", LV_STYLE_TEXT_DECOR},
        {"text_letter_space", LV_STYLE_TEXT_LETTER_SPACE},
        {"text_line_space", LV_STYLE_TEXT_LINE_SPACE},
        {"text_opa", LV_STYLE_TEXT_OPA},
        {"transform_pivot_x", LV_STYLE_TRANSFORM_PIVOT_X},
        {"transform_pivot_y", LV_STYLE_TRANSFORM_PIVOT_Y},
        {"transform_rotation", LV_STYLE_TRANSFORM_ROTATION},
        {"transform_scale_x", LV_STYLE_TRANSFORM_SCALE_X},
        {"transform_scale_y", LV_STYLE_TRANSFORM_SCALE_Y},
        {"transform_skew_x", LV_STYLE_TRANSFORM_SKEW_X},
        {"transform_skew_y", LV_STYLE_TRANSFORM_SKEW_Y},
        {"translate_x", LV_STYLE_TRANSLATE_X},
        {"translate_y", LV_STYLE_TRANSLATE_Y},
        {"width", LV_STYLE_WIDTH},
    };
    auto it = m.find(name);
    return it == m.end() ? LV_STYLE_PROP_INV : it->second;
}

bool isColorStyleProp(const std::string& name) {
    static const std::set<std::string> s = {
        "arc_color",
        "bg_color",
        "bg_grad_color",
        "bg_image_recolor",
        "border_color",
        "image_recolor",
        "line_color",
        "outline_color",
        "shadow_color",
        "text_color",
    };
    return s.count(name) > 0;
}
} // namespace

QVariant LvglRenderer::styleValue(const std::string& id, const std::string& prop) const {
    auto it = m_objects.find(id);
    if (it == m_objects.end() || !it->second || !lv_obj_is_valid(it->second)) return QVariant();
    /* 这些不是样式，但属性面板同样需要显示"实际生效值"：
     * 未显式设置时若退回 schema 默认值，会和画布上的真实状态不一致
     * （例如标签显示 scrollable=勾选，实际 LVGL 标签滚动标志是关闭的）。 */
    if (prop == "scrollable") return QVariant(lv_obj_has_flag(it->second, LV_OBJ_FLAG_SCROLLABLE));
    if (prop == "clickable") return QVariant(lv_obj_has_flag(it->second, LV_OBJ_FLAG_CLICKABLE));
    if (prop == "hidden") return QVariant(lv_obj_has_flag(it->second, LV_OBJ_FLAG_HIDDEN));
    if (prop == "scroll_dir") return QVariant((int)lv_obj_get_scroll_dir(it->second));
    if (prop == "scrollbar_mode") return QVariant((int)lv_obj_get_scrollbar_mode(it->second));

    lv_style_prop_t sp = stylePropId(prop);
    if (sp == LV_STYLE_PROP_INV) return QVariant();

    lv_style_value_t v = lv_obj_get_style_prop(it->second, 0, sp);
    if (isColorStyleProp(prop)) {
        char buf[16];
        std::snprintf(buf, sizeof(buf), "0x%02X%02X%02X", v.color.red, v.color.green, v.color.blue);
        return QVariant(QString::fromUtf8(buf));
    }
    return QVariant((int)v.num);
}

void LvglRenderer::clear() {
    destroyHandles();

    m_selectedObj = nullptr;
    m_multiSelectedObjs.clear();
    m_objects.clear();
    m_idMap.clear();
    m_types.clear();
    m_gridDescArrays.clear();
    m_stackedNavs.clear();
    m_strStore.clear();
    m_mapStore.clear();
    m_ptsStore.clear();

    lv_obj_clean(lv_scr_act());
}

lv_obj_t* LvglRenderer::findObject(const std::string& id) const {
    auto it = m_objects.find(id);
    return it != m_objects.end() ? it->second : nullptr;
}

std::string LvglRenderer::findId(lv_obj_t* obj) const {
    auto it = m_idMap.find(obj);
    return it != m_idMap.end() ? it->second : "";
}

void LvglRenderer::selectObject(const std::string& id) {
    deselectAll();
    auto it = m_objects.find(id);
    if (it != m_objects.end()) {
        lv_obj_t* obj = it->second;
        if (obj) {
            m_selectedObj = obj;
            lv_obj_set_style_outline_width(obj, 2, 0);
            lv_obj_set_style_outline_color(obj, lv_color_hex(0x2196F3), 0);
            lv_obj_set_style_outline_opa(obj, LV_OPA_COVER, 0);
            lv_obj_set_style_outline_pad(obj, 2, 0);
            if (!isLayoutChild(obj) && obj != lv_scr_act()) {
                createHandles(obj);
            }
        }
    }
}

void LvglRenderer::deselectAll() {
    destroyHandles();
    if (m_selectedObj && lv_obj_is_valid(m_selectedObj)) {
        lv_obj_set_style_outline_width(m_selectedObj, 0, 0);
    }
    m_selectedObj = nullptr;
}

void LvglRenderer::createHandles(lv_obj_t* widget) {
    destroyHandles();

    lv_obj_t* screen = lv_scr_act();
    for (int i = 0; i < 8; i++) {
        lv_obj_t* h = lv_obj_create(screen);
        lv_obj_set_size(h, HANDLE_SIZE, HANDLE_SIZE);
        lv_obj_set_style_bg_color(h, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_bg_opa(h, LV_OPA_COVER, 0);
        lv_obj_set_style_border_color(h, lv_color_hex(0x2196F3), 0);
        lv_obj_set_style_border_width(h, 2, 0);
        lv_obj_set_style_radius(h, 0, 0);
        lv_obj_set_style_pad_all(h, 0, 0);
        lv_obj_add_flag(h, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(h, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_clear_flag(h, LV_OBJ_FLAG_CLICK_FOCUSABLE);
        lv_obj_set_ext_click_area(h, 8);
        lv_obj_move_foreground(h);

        int* idx = new int(i);
        lv_obj_set_user_data(h, idx);

        lv_obj_add_event_cb(h, [](lv_event_t* e) {
            auto* self = static_cast<LvglRenderer*>(lv_event_get_user_data(e));
            if (!self || !self->m_selectedObj) return;
            lv_indev_t* indev = lv_indev_active();
            if (!indev) return;
            lv_point_t point;
            lv_indev_get_point(indev, &point);

            lv_obj_t* handle = static_cast<lv_obj_t*>(lv_event_get_target(e));
            int idx = *static_cast<int*>(lv_obj_get_user_data(handle));

            if (!self->m_handleDragging) {
                self->m_handleDragging = true;
                self->m_handleIndex = idx;
                self->m_widgetOrigX = lv_obj_get_x(self->m_selectedObj);
                self->m_widgetOrigY = lv_obj_get_y(self->m_selectedObj);
                self->m_widgetOrigW = lv_obj_get_width(self->m_selectedObj);
                self->m_widgetOrigH = lv_obj_get_height(self->m_selectedObj);
                self->m_mouseStartX = point.x;
                self->m_mouseStartY = point.y;
            }

            int dx = point.x - self->m_mouseStartX;
            int dy = point.y - self->m_mouseStartY;

            lv_coord_t nx = self->m_widgetOrigX;
            lv_coord_t ny = self->m_widgetOrigY;
            lv_coord_t nw = self->m_widgetOrigW;
            lv_coord_t nh = self->m_widgetOrigH;

            // Handle index: 0=TL, 1=TC, 2=TR, 3=ML, 4=MR, 5=BL, 6=BC, 7=BR
            switch (self->m_handleIndex) {
                case 0: nx += dx; ny += dy; nw -= dx; nh -= dy; break;
                case 1: ny += dy; nh -= dy; break;
                case 2: ny += dy; nw += dx; nh -= dy; break;
                case 3: nx += dx; nw -= dx; break;
                case 4: nw += dx; break;
                case 5: nx += dx; nw -= dx; nh += dy; break;
                case 6: nh += dy; break;
                case 7: nw += dx; nh += dy; break;
            }

            if (nw < MIN_WIDGET_SIZE) {
                if (self->m_handleIndex == 0 || self->m_handleIndex == 3 || self->m_handleIndex == 5)
                    nx = self->m_widgetOrigX + self->m_widgetOrigW - MIN_WIDGET_SIZE;
                nw = MIN_WIDGET_SIZE;
            }
            if (nh < MIN_WIDGET_SIZE) {
                if (self->m_handleIndex == 0 || self->m_handleIndex == 1 || self->m_handleIndex == 2)
                    ny = self->m_widgetOrigY + self->m_widgetOrigH - MIN_WIDGET_SIZE;
                nh = MIN_WIDGET_SIZE;
            }

            lv_obj_set_pos(self->m_selectedObj, nx, ny);
            lv_obj_set_size(self->m_selectedObj, nw, nh);
            self->updateHandlePositions();
        }, LV_EVENT_PRESSING, this);

        lv_obj_add_event_cb(h, [](lv_event_t* e) {
            auto* self = static_cast<LvglRenderer*>(lv_event_get_user_data(e));
            if (!self) return;
            if (self->m_handleDragging) {
                self->m_handleDragging = false;
                if (self->m_selectedObj) {
                    auto idIt = self->m_idMap.find(self->m_selectedObj);
                    if (idIt != self->m_idMap.end()) {
                        PendingEvent evt;
                        evt.type = PendingEvent::Moved;
                        evt.id = idIt->second;
                        evt.x = lv_obj_get_x(self->m_selectedObj);
                        evt.y = lv_obj_get_y(self->m_selectedObj);
                        evt.w = lv_obj_get_width(self->m_selectedObj);
                        evt.h = lv_obj_get_height(self->m_selectedObj);
                        self->m_pendingMutex.lock();
                        self->m_pendingEvents.push_back(std::move(evt));
                        self->m_pendingMutex.unlock();
                    }
                }
            }
        }, LV_EVENT_RELEASED, this);

        m_handles[i] = h;
    }

    updateHandlePositions();
}

void LvglRenderer::destroyHandles() {
    for (int i = 0; i < 8; i++) {
        if (!m_handles[i]) continue;
        if (lv_obj_is_valid(m_handles[i])) {
            int* idx = static_cast<int*>(lv_obj_get_user_data(m_handles[i]));
            delete idx;
            lv_obj_del(m_handles[i]);
        }
        m_handles[i] = nullptr;
    }
    m_handleDragging = false;
    m_handleIndex = -1;
}

void LvglRenderer::updateHandlePositions() {
    if (!m_selectedObj) return;

    lv_obj_update_layout(m_selectedObj);

    lv_area_t coords;
    lv_obj_get_coords(m_selectedObj, &coords);
    lv_coord_t ax = coords.x1;
    lv_coord_t ay = coords.y1;
    lv_coord_t w = coords.x2 - coords.x1 + 1;
    lv_coord_t h = coords.y2 - coords.y1 + 1;

    int hs = HANDLE_SIZE;
    int half = hs / 2;

    int positions[8][2] = {
        {ax - half,        ay - half},
        {ax + w/2 - half,  ay - half},
        {ax + w - half,    ay - half},
        {ax - half,        ay + h/2 - half},
        {ax + w - half,    ay + h/2 - half},
        {ax - half,        ay + h - half},
        {ax + w/2 - half,  ay + h - half},
        {ax + w - half,    ay + h - half}
    };



    for (int i = 0; i < 8; i++) {
        if (m_handles[i]) {
            lv_obj_set_pos(m_handles[i], positions[i][0], positions[i][1]);
            lv_obj_move_foreground(m_handles[i]);
        }
    }
}

std::string LvglRenderer::findContainerAt(int x, int y) const {
    static const std::set<std::string> containerTypes = {
        "LVBoxLayout", "LHBoxLayout",
        "LGridLayout", "LStackedLayout",
        "LTabView", "LMenu", "LMenuPage", "LMenuCont", "LMenuSection",
        "LTileView", "LTile", "LList", "LWin",
        "LGroupBox"
    };

    std::string deepest;
    int deepestDepth = -1;

    for (const auto& [id, obj] : m_objects) {
        if (!obj || !lv_obj_is_valid(obj)) continue;

        auto typeIt = m_types.find(id);
        if (typeIt == m_types.end()) continue;
        const std::string& type = typeIt->second;
        if (containerTypes.find(type) == containerTypes.end()) continue;

        lv_obj_update_layout(obj);
        lv_area_t coords;
        lv_obj_get_coords(obj, &coords);

        int depth = 0;
        lv_obj_t* cur = obj;
        while (cur) { cur = lv_obj_get_parent(cur); depth++; }

        if (x >= coords.x1 && x <= coords.x2 && y >= coords.y1 && y <= coords.y2) {
            if (depth > deepestDepth) {
                deepestDepth = depth;
                deepest = id;
            }
        }
    }

    return deepest;
}

std::string LvglRenderer::objectType(const std::string& id) const {
    auto it = m_types.find(id);
    return it != m_types.end() ? it->second : "";
}

void LvglRenderer::screenToRelative(const std::string& parentId, int& x, int& y) const {
    auto it = m_objects.find(parentId);
    if (it == m_objects.end() || !it->second) return;

    lv_obj_update_layout(it->second);
    lv_area_t coords;
    lv_obj_get_coords(it->second, &coords);

    int32_t pad_left = lv_obj_get_style_space_left(it->second, LV_PART_MAIN);
    int32_t pad_top = lv_obj_get_style_space_top(it->second, LV_PART_MAIN);

    x -= coords.x1 + pad_left;
    y -= coords.y1 + pad_top;
}

void LvglRenderer::clearMultiSelection() {
    for (auto* obj : m_multiSelectedObjs) {
        if (obj && lv_obj_is_valid(obj)) {
            lv_obj_set_style_outline_width(obj, 0, 0);
        }
    }
    m_multiSelectedObjs.clear();
}

void LvglRenderer::selectMultiple(const std::vector<std::string>& ids) {
    deselectAll();
    for (size_t i = 0; i < ids.size(); i++) {
        auto it = m_objects.find(ids[i]);
        if (it != m_objects.end() && it->second) {
            lv_obj_t* obj = it->second;
            lv_obj_set_style_outline_width(obj, 2, 0);
            lv_obj_set_style_outline_color(obj, lv_color_hex(0x2196F3), 0);
            lv_obj_set_style_outline_opa(obj, LV_OPA_COVER, 0);
            lv_obj_set_style_outline_pad(obj, 2, 0);
            m_multiSelectedObjs.push_back(obj);
            if (i == 0) {
                m_selectedObj = obj;
                if (!isLayoutChild(obj)) {
                    createHandles(obj);
                }
            }
        }
    }
}

void LvglRenderer::addToSelection(const std::string& id) {
    auto it = m_objects.find(id);
    if (it == m_objects.end() || !it->second) return;

    for (auto* o : m_multiSelectedObjs) {
        if (o == it->second) return;
    }

    lv_obj_t* obj = it->second;
    lv_obj_set_style_outline_width(obj, 2, 0);
    lv_obj_set_style_outline_color(obj, lv_color_hex(0x2196F3), 0);
    lv_obj_set_style_outline_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_outline_pad(obj, 2, 0);
    m_multiSelectedObjs.push_back(obj);

    if (!m_selectedObj) {
        m_selectedObj = obj;
        if (!isLayoutChild(obj)) {
            createHandles(obj);
        }
    }
}

void LvglRenderer::removeFromSelection(const std::string& id) {
    auto it = m_objects.find(id);
    if (it == m_objects.end() || !it->second) return;

    lv_obj_t* obj = it->second;
    auto pos = std::find(m_multiSelectedObjs.begin(), m_multiSelectedObjs.end(), obj);
    if (pos != m_multiSelectedObjs.end()) {
        lv_obj_set_style_outline_width(obj, 0, 0);
        m_multiSelectedObjs.erase(pos);

        if (m_selectedObj == obj) {
            destroyHandles();
            m_selectedObj = nullptr;
            if (!m_multiSelectedObjs.empty()) {
                m_selectedObj = m_multiSelectedObjs[0];
                if (!isLayoutChild(m_selectedObj)) {
                    createHandles(m_selectedObj);
                }
            }
        }
    }
}

bool LvglRenderer::isSelected(const std::string& id) const {
    auto it = m_objects.find(id);
    if (it == m_objects.end() || !it->second) return false;
    lv_obj_t* obj = it->second;
    return std::find(m_multiSelectedObjs.begin(), m_multiSelectedObjs.end(), obj) != m_multiSelectedObjs.end();
}

} // namespace designer
