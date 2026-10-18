#pragma once
#include "ir.hpp"
#include "component_registry.hpp"
#include <algorithm>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <functional>

namespace luic {

class Validator {
    const ComponentRegistry& m_registry;
    /* 工程模式下的全部文档（名字 -> 文档），用于解析 "@Name" 实例 */
    const std::map<std::string, const lui_document_t*>* m_components = nullptr;

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
    void setComponents(const std::map<std::string, const lui_document_t*>* comps) {
        m_components = comps;
    }

    explicit Validator(const ComponentRegistry& reg) : m_registry(reg) {}

    std::vector<std::string> validate(const lui_document_t& doc) {
        std::vector<std::string> errors;

        if (doc.name.empty()) {
            errors.push_back("document 'name' is required");
        }

        if (doc.children.empty()) {
            errors.push_back("document has no children");
        }

        /* ---- kind ---- */
        if (doc.kind != "screen" && doc.kind != "component") {
            errors.push_back("invalid kind '" + doc.kind + "' (expected 'screen' or 'component')");
        }
        if (doc.is_component() && doc.hadSize) {
            errors.push_back("component '" + doc.name + "' must not declare 'size'");
        }
        if (!doc.is_component() && !doc.exports.empty()) {
            errors.push_back("only components can declare 'exports' (doc '" + doc.name + "')");
        }

        /* ---- exports: "<id>.<prop>" 必须指向存在的节点与属性 ---- */
        for (auto& kv : doc.exports) {
            std::string path = kv.second;
            auto dot = path.find('.');
            if (dot == std::string::npos) {
                errors.push_back("export '" + kv.first + "': expected '<id>.<property>', got '" + path + "'");
                continue;
            }
            std::string nodeId = path.substr(0, dot);
            std::string propName = path.substr(dot + 1);
            if (nodeId.rfind("m_", 0) == 0) nodeId = nodeId.substr(2);   // 兼容 m_title 写法
            auto* target = find_node(doc, nodeId);
            if (!target) {
                errors.push_back("export '" + kv.first + "': node '" + nodeId + "' not found");
                continue;
            }
            auto* cdef = m_registry.find(target->type);
            if (!cdef) continue;
            if (!cdef->find_prop(propName) && !style_props().count(propName)) {
                errors.push_back("export '" + kv.first + "': '" + target->type +
                                 "' has no property '" + propName + "'");
            }
        }

        for (auto& child : doc.children) {
            validate_node(child, errors);
        }

        /* 图片资源引用必须先在 resources 里声明，否则生成代码里会出现未定义符号 */
        for (auto* n : doc.all_nodes()) {
            for (auto& p : n->props) {
                bool src_like = p.name == "src" ||
                                (p.name.size() > 4 && p.name.compare(p.name.size() - 4, 4, "_src") == 0) ||
                                p.name.rfind("src_", 0) == 0;
                if (!src_like || p.str_val.empty() || p.str_val[0] != '#') continue;
                std::string name = p.str_val.substr(1);
                if (!doc.resources.count(name))
                    errors.push_back(n->id + ": 图片资源 '#" + name +
                                     "' 未在 resources 中声明");
            }
        }

        return errors;
    }

private:
    bool is_valid_theme(const std::string& value) const {
        static const char* allowed[] = {"primary", "danger", "success", "warning"};
        return std::any_of(std::begin(allowed), std::end(allowed),
            [&](const char* item) { return value == item; });
    }

    static const widget_node_t* find_node(const lui_document_t& doc, const std::string& id) {
        const widget_node_t* found = nullptr;
        std::function<void(const std::vector<widget_node_t>&)> walk =
            [&](const std::vector<widget_node_t>& nodes) {
                for (auto& n : nodes) {
                    if (n.id == id) found = &n;
                    walk(n.children);
                }
            };
        walk(doc.children);
        return found;
    }

    /* "@MyCard" 实例：父文件里声明一个组件实例 */
    void validate_instance(const widget_node_t& node, std::vector<std::string>& errors) {
        std::string compName = node.type.substr(1);
        if (!m_components) {
            errors.push_back(node.id + ": component instance '@" + compName +
                             "' requires project mode (luic --project ...)");
            return;
        }
        auto it = m_components->find(compName);
        if (it == m_components->end()) {
            errors.push_back(node.id + ": unknown component '@" + compName + "'");
            return;
        }
        const lui_document_t* cdoc = it->second;
        if (!cdoc->is_component()) {
            errors.push_back(node.id + ": '@" + compName + "' is not a component (kind: screen)");
            return;
        }
        if (!node.events.empty()) {
            errors.push_back(node.id + ": component instances do not support 'events' yet");
        }
        for (auto& prop : node.props) {
            if (prop.name == "pos" || prop.name == "size" || prop.name == "align" ||
                prop.name == "hidden" || prop.name == "clickable") continue;
            if (style_props().count(prop.name)) continue;
            if (!cdoc->exports.count(prop.name)) {
                errors.push_back(node.id + ": '@" + compName + "' has no exported parameter '" +
                                 prop.name + "'");
            }
        }
        for (auto& child : node.children) {
            validate_node(child, errors);
        }
    }

    void validate_node(const widget_node_t& node, std::vector<std::string>& errors) {
        if (!node.type.empty() && node.type[0] == '@') {
            validate_instance(node, errors);
            return;
        }
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
