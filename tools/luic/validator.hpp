#pragma once
#include "ir.hpp"
#include "component_registry.hpp"
#include <algorithm>
#include <string>
#include <vector>
#include <set>

namespace luic {

class Validator {
    const ComponentRegistry& m_registry;

    static const std::set<std::string>& base_props() {
        static const std::set<std::string> s = {
            "x", "y", "width", "height", "align",
            "hidden", "clickable", "scrollable", "scroll_dir", "scrollbar_mode",
            "layout", "ext_click_area",
            "bg_color", "bg_opa", "bg_grad_color", "bg_grad_dir",
            "bg_main_stop", "bg_grad_stop",
            "bg_image_src", "bg_image_opa", "bg_image_recolor", "bg_image_recolor_opa", "bg_image_tiled",
            "border_color", "border_width", "border_opa", "border_side", "border_post",
            "outline_color", "outline_width", "outline_opa", "outline_pad",
            "shadow_color", "shadow_width", "shadow_ofs_x", "shadow_ofs_y", "shadow_spread", "shadow_opa",
            "pad_top", "pad_bottom", "pad_left", "pad_right", "pad_row", "pad_column",
            "margin_top", "margin_bottom", "margin_left", "margin_right",
            "min_width", "max_width", "min_height", "max_height",
            "radius", "clip_corner", "opa", "blend_mode", "base_dir",
            "text_color", "text_opa", "text_font_size", "text_letter_space",
            "text_line_space", "text_align", "text_decor",
            "transform_rotation", "transform_scale_x", "transform_scale_y",
            "transform_pivot_x", "transform_pivot_y",
            "translate_x", "translate_y", "transform_skew_x", "transform_skew_y",
            "line_color", "line_width", "line_opa", "line_dash_width", "line_dash_gap", "line_rounded",
            "arc_color", "arc_width", "arc_opa", "arc_rounded",
            "image_opa", "image_recolor", "image_recolor_opa",
        };
        return s;
    }

public:
    explicit Validator(const ComponentRegistry& reg) : m_registry(reg) {}

    std::vector<std::string> validate(const lui_document_t& doc) {
        std::vector<std::string> errors;

        if (doc.name.empty()) {
            errors.push_back("document 'name' is required");
        }

        if (doc.children.empty()) {
            errors.push_back("document has no children");
        }

        for (auto& child : doc.children) {
            validate_node(child, errors);
        }

        return errors;
    }

private:
    bool is_valid_theme(const std::string& value) const {
        static const char* allowed[] = {"primary", "danger", "success", "warning"};
        return std::any_of(std::begin(allowed), std::end(allowed),
            [&](const char* item) { return value == item; });
    }

    void validate_node(const widget_node_t& node, std::vector<std::string>& errors) {
        auto* comp = m_registry.find(node.type);
        if (!comp) {
            errors.push_back("unknown component type: " + node.type);
            return;
        }

        if (node.id.empty()) {
            errors.push_back(node.type + ": 'id' is required");
        }

        for (auto& prop : node.props) {
            if (base_props().count(prop.name)) continue;

            auto* def = comp->find_prop(prop.name);
            if (!def) {
                errors.push_back(node.type + "." + node.id +
                    ": unknown property '" + prop.name + "'");
                continue;
            }

            if (!prop.type.empty() && prop.type != def->type) {
                errors.push_back(node.type + "." + node.id +
                    ": property '" + prop.name + "' expects type '" + def->type +
                    "' but got '" + prop.type + "'");
                continue;
            }

            if (def->is_theme && !is_valid_theme(prop.str_val)) {
                errors.push_back(node.type + "." + node.id +
                    ": invalid theme '" + prop.str_val + "'");
            }
        }

        for (auto& evt : node.events) {
            if (!comp->find_event(evt)) {
                errors.push_back(node.type + "." + node.id +
                    ": unknown event '" + evt + "'");
            }
        }

        if (!node.children.empty() && !comp->is_container) {
            errors.push_back(node.type + "." + node.id +
                ": is not a container, cannot have children");
        }

        for (auto& child : node.children) {
            validate_node(child, errors);
        }
    }
};

} // namespace luic
