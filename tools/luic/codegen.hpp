#pragma once
#include "ir.hpp"
#include "component_registry.hpp"
#include <sstream>
#include <string>
#include <set>

namespace luic {

class CodeGenerator {
    const ComponentRegistry& m_registry;
    std::vector<std::string> m_pendingRefs;
    /* 本次生成用到的嵌入资源（"#别名"），用于决定是否 include 资源头文件 */
    std::set<std::string> m_usedResources;
    std::string m_assetsInclude = "assets/lvgl_assets.h";

    std::string escape_string(const std::string& s) {
        std::string out;
        out.reserve(s.size());
        for (char c : s) {
            switch (c) {
                case '\n': out += "\\n"; break;
                case '\r': out += "\\r"; break;
                case '\t': out += "\\t"; break;
                case '"':  out += "\\\""; break;
                case '\\': out += "\\\\"; break;
                default:   out += c; break;
            }
        }
        return out;
    }

    static bool is_src_prop(const prop_def_t& def, const std::string& name) {
        if (def.method.rfind("set_src", 0) == 0) return true;
        if (name == "src") return true;
        return name.size() > 4 && name.compare(name.size() - 4, 4, "_src") == 0;
    }

    /* "#logo" 表示编译期嵌入的图片资源，生成 &logo；其它值仍按普通字符串 */
    std::string src_value(const std::string& s) {
        if (!s.empty() && s[0] == '#') {
            std::string name = s.substr(1);
            if (!name.empty()) {
                m_usedResources.insert(name);
                return "&" + name;
            }
        }
        return "\"" + escape_string(s) + "\"";
    }

    std::string format_value(const prop_def_t& def, const property_t& prop) {
        if (def.type == "string") {
            if (is_src_prop(def, prop.name)) return src_value(prop.str_val);
            return "\"" + escape_string(prop.str_val) + "\"";
        }
        if (def.type == "int")      return std::to_string(prop.int_val);
        if (def.type == "hex")      return prop.str_val;
        if (def.type == "int_pair") return std::to_string(prop.int_val) + ", " + std::to_string(prop.int_val2);
        if (def.type == "int_triple") return std::to_string(prop.int_val) + ", " + std::to_string(prop.int_val2) + ", " + std::to_string(prop.int_val3);
        if (def.type == "bool")     return prop.bool_val ? "true" : "false";
        return prop.raw;
    }

    bool is_bool_action_method(const prop_def_t& def) const {
        return def.type == "bool" && (def.method == "on" || def.method == "off");
    }

    static int node_prop_int(const widget_node_t& n, const std::string& name, int def) {
        for (auto& p : n.props) {
            if (p.name == name) return p.int_val;
        }
        return def;
    }

    static std::vector<std::string> split(const std::string& s, char sep) {
        std::vector<std::string> out;
        std::string cur;
        for (char c : s) {
            if (c == sep) { out.push_back(cur); cur.clear(); }
            else cur += c;
        }
        out.push_back(cur);
        return out;
    }

    void emit_expand(std::ostringstream& out, const std::string& pad,
                     const std::string& id, const prop_def_t& def,
                     const property_t& prop) {
        if (def.expand == "argv") {
            auto rows = split(prop.str_val, '\n');
            out << pad << "static const char* _m_" << id << "[] = {";
            bool first_row = true;
            for (auto& row : rows) {
                if (!first_row) out << "\"\\n\", ";
                for (auto& item : split(row, ' ')) {
                    if (item.empty()) continue;
                    out << "\"" << escape_string(item) << "\", ";
                }
                first_row = false;
            }
            out << "NULL};\n";
            out << pad << "m_" << id << "->" << def.method << "(_m_" << id << ");\n";
        } else if (def.expand == "lines") {
            for (auto& line : split(prop.str_val, '\n')) {
                if (line.empty()) continue;
                out << pad << "m_" << id << "->" << def.method
                    << "(\"" << escape_string(line) << "\");\n";
            }
        } else if (def.expand == "cells") {
            auto rows = split(prop.str_val, '\n');
            for (size_t r = 0; r < rows.size(); r++) {
                auto cells = split(rows[r], '|');
                for (size_t c = 0; c < cells.size(); c++) {
                    if (cells[c].empty()) continue;
                    out << pad << "m_" << id << "->set_cell_value(" << r << ", "
                        << c << ", \"" << escape_string(cells[c]) << "\");\n";
                }
            }
        } else if (def.expand == "int_2arg") {
            auto vals = split(prop.str_val, ',');
            for (size_t i = 0; i < vals.size(); i++) {
                std::string v = vals[i];
                while (!v.empty() && v.front() == ' ') v.erase(v.begin());
                if (v.empty()) continue;
                out << pad << "m_" << id << "->" << def.method
                    << "(" << i << ", " << v << ");\n";
            }
        } else if (def.expand == "list_items") {
            for (auto& line : split(prop.str_val, '\n')) {
                if (line.empty()) continue;
                if (line[0] == '#')
                    out << pad << "m_" << id << "->add_text(\""
                        << escape_string(line.substr(1)) << "\");\n";
                else
                    out << pad << "m_" << id << "->add_button(NULL, \""
                        << escape_string(line) << "\");\n";
            }
        } else if (def.expand == "series") {
            for (auto& ser : split(prop.str_val, ';')) {
                if (ser.empty()) continue;
                out << pad << "{ lv_chart_series_t* _s = m_" << id
                    << "->add_series(0xFF0000, LV_CHART_AXIS_PRIMARY_Y);\n";
                for (auto& v : split(ser, ',')) {
                    std::string val = v;
                    while (!val.empty() && val.front() == ' ') val.erase(val.begin());
                    if (val.empty()) continue;
                    out << pad << "  m_" << id << "->set_next_value(_s, " << val << ");\n";
                }
            out << pad << "}\n";
            }
        } else if (def.expand == "ref") {
            if (!prop.str_val.empty()) m_pendingRefs.push_back("m_" + id + "->" + def.method + "(m_" + prop.str_val + "->native());");
        } else if (def.expand == "spans") {
            for (auto& line : split(prop.str_val, '\n')) {
                if (line.empty()) continue;
                out << pad << "{ lv_span_t* _sp = m_" << id << "->new_span();\n";
                out << pad << "  lv_span_set_text(_sp, \"" << escape_string(line) << "\"); }\n";
            }
        } else if (def.expand == "points") {
            auto pts = split(prop.str_val, ';');
            size_t n = 0;
            out << pad << "static const lv_point_precise_t _pts_" << id << "[] = {";
            for (auto& p : pts) {
                std::string t = p;
                while (!t.empty() && (t.front() == ' ' || t.front() == '\n')) t.erase(t.begin());
                while (!t.empty() && (t.back() == ' ' || t.back() == '\r')) t.pop_back();
                if (t.empty()) continue;
                auto xy = split(t, ',');
                if (xy.size() < 2) continue;
                std::string xs = xy[0], ys = xy[1];
                while (!xs.empty() && xs.front() == ' ') xs.erase(xs.begin());
                while (!ys.empty() && ys.front() == ' ') ys.erase(ys.begin());
                out << "{" << xs << ", " << ys << "}, ";
                n++;
            }
            out << "};\n";
            out << pad << "m_" << id << "->set_points(_pts_" << id << ", " << n << ");\n";
        }
    }

    void gen_node(std::ostringstream& out, const widget_node_t& node,
                  const std::string& parent_expr, int indent) {
        std::string pad(indent, ' ');
        auto* comp = m_registry.find(node.type);
        if (!comp) return;

        out << pad << "m_" << node.id << " = new " << node.type
            << "(" << parent_expr;

        std::vector<std::string> ctor_names;
        if (!comp->ctor_prop.empty()) ctor_names.push_back(comp->ctor_prop);
        for (auto& cn : comp->ctor_props) ctor_names.push_back(cn);
        for (auto& cn : ctor_names) {
            for (auto& p : node.props) {
                if (p.name != cn) continue;
                auto* pd = comp->find_prop(cn);
                out << ", " << (pd ? format_value(*pd, p) : p.raw);
                break;
            }
        }
        out << ");\n";

        auto is_ctor = [&](const std::string& n) {
            for (auto& cn : ctor_names) if (cn == n) return true;
            return false;
        };

        std::vector<const property_t*> deferred;
        bool has_chain = false;
        for (auto& prop : node.props) {
            if (is_ctor(prop.name)) continue;
            auto* pdef = comp->find_prop(prop.name);
            if (!pdef) continue;
            /* 空图片路径会让 lv_image_set_src() 直接崩，空值表示"未设置图片" */
            if (is_src_prop(*pdef, prop.name) && prop.str_val.empty()) continue;

            if (pdef->after_children) {
                deferred.push_back(&prop);
                continue;
            }

            if (!pdef->expand.empty()) {
                if (has_chain) { out << ";\n"; has_chain = false; }
                out << "\n";
                emit_expand(out, pad, node.id, *pdef, prop);
                continue;
            }

            if (pdef->is_theme) {
                if (!has_chain) {
                    out << pad << "m_" << node.id << "->" << prop.str_val << "()";
                    has_chain = true;
                } else {
                    out << "." << prop.str_val << "()";
                }
            } else if (is_bool_action_method(*pdef)) {
                std::string method = prop.bool_val ? pdef->method : (pdef->method == "on" ? "off" : pdef->method);
                if (!has_chain) {
                    out << pad << "m_" << node.id << "->" << method << "()";
                    has_chain = true;
                } else {
                    out << "\n" << pad << "      ." << method << "()";
                }
            } else {
                std::string val = format_value(*pdef, prop);
                if (!has_chain) {
                    out << pad << "m_" << node.id << "->" << pdef->method << "(";
                    has_chain = true;
                } else {
                    out << "\n" << pad << "      ." << pdef->method << "(";
                }
                if (!pdef->prefix_arg.empty()) out << pdef->prefix_arg << ", ";
                out << val << ")";
            }
        }
        if (has_chain) out << ";\n";

        // === 通用样式属性：生成 LStyle 设置块（对所有组件类型都生效）===
        {
            std::string style_block;
            for (auto& prop : node.props) {
                std::string k = style_kind(prop.name);
                if (k.empty()) continue;
                if (k == "src" && prop.str_val.empty()) continue;
                std::string val;
                if (k == "color") {
                    val = prop.str_val.empty() ? prop.raw : prop.str_val;
                } else if (k == "bool") {
                    val = prop.bool_val ? "true" : "false";
                } else if (k == "src") {
                    val = src_value(prop.str_val);
                } else {
                    val = std::to_string(prop.int_val);
                }
                style_block += pad + "    st.set_" + prop.name + "(" + val + ");\n";
            }
            if (!style_block.empty()) {
                out << pad << "{\n"
                    << pad << "    LStyle st;\n"
                    << style_block
                    << pad << "    st.apply_to(m_" << node.id << "->native());\n"
                    << pad << "}\n";
            }
        }

        for (auto& child : node.children) {
            out << "\n";
            gen_node(out, child, "m_" + node.id + "->native()", indent);
        }

        if (node.type == "LGridLayout") {
            int cols = node_prop_int(node, "columns", 2);
            if (cols < 1) cols = 1;
            for (size_t i = 0; i < node.children.size(); i++) {
                const auto& child = node.children[i];
                int col = node_prop_int(child, "grid_col", (int)(i % cols));
                int row = node_prop_int(child, "grid_row", (int)(i / cols));
                int cspan = node_prop_int(child, "grid_col_span", 1);
                int rspan = node_prop_int(child, "grid_row_span", 1);
                out << pad << "lv_obj_set_grid_cell(m_" << child.id
                    << "->native(), LV_GRID_ALIGN_STRETCH, " << col << ", " << cspan
                    << ", LV_GRID_ALIGN_STRETCH, " << row << ", " << rspan << ");\n";
            }
        }

        for (auto* prop : deferred) {
            auto* pdef = comp->find_prop(prop->name);
            if (!pdef) continue;
            if (!pdef->expand.empty()) {
                emit_expand(out, pad, node.id, *pdef, *prop);
                continue;
            }
            out << "\n" << pad << "m_" << node.id << "->" << pdef->method << "(";
            if (!pdef->prefix_arg.empty()) out << pdef->prefix_arg << ", ";
            out << format_value(*pdef, *prop) << ");\n";
        }
    }

public:
    explicit CodeGenerator(const ComponentRegistry& reg) : m_registry(reg) {}

    void setAssetsInclude(const std::string& inc) { m_assetsInclude = inc; }

    std::string generate(const lui_document_t& doc) {
        m_usedResources.clear();
        std::ostringstream out;
        auto all = doc.all_nodes();

        out << "// Auto-generated by luic from " << doc.name << ".lui\n";
        out << "// DO NOT EDIT — changes will be overwritten\n";
        out << "#pragma once\n";
        out << "#include \"lvgl++/lvgl_widgets.hpp\"\n";
        out << "//@@ASSETS_INCLUDE@@\n";
        out << "namespace lvgl {\n\n";
        out << "class " << doc.name << "_UI {\n";

        // protected members
        out << "protected:\n";
        for (auto* n : all) {
            out << "    " << n->type << "* m_" << n->id << " = nullptr;\n";
        }

        // setup()
        out << "\npublic:\n";
        out << "    static constexpr int kScreenWidth = " << doc.width << ";\n";
        out << "    static constexpr int kScreenHeight = " << doc.height << ";\n";
        out << "    void setup(lv_obj_t* parent = nullptr) {\n";
        out << "        lv_obj_t* root = parent ? parent : lv_scr_act();\n\n";
        for (auto& child : doc.children) {
            gen_node(out, child, "root", 8);
            out << "\n";
        }
        if (!m_pendingRefs.empty()) {
            out << "        // cross-references\n";
            for (auto& r : m_pendingRefs) out << "        " << r << "\n";
            out << "\n";
            m_pendingRefs.clear();
        }
        out << "    }\n";

        // bind_events()
        out << "\n    void bind_events() {\n";
        for (auto* n : all) {
            auto* comp = m_registry.find(n->type);
            if (!comp) continue;
            for (auto& evt_name : n->events) {
                auto* edef = comp->find_event(evt_name);
                if (!edef) continue;
                out << "        m_" << n->id << "->" << edef->method
                    << "([this](lv_event_t* e) { on_" << n->id
                    << "_" << evt_name << "(e); });\n";
            }
        }
        out << "    }\n";

        // virtual event slots
        for (auto* n : all) {
            for (auto& evt_name : n->events) {
                out << "\n    virtual void on_" << n->id
                    << "_" << evt_name << "(lv_event_t* e) {}";
            }
        }
        out << "\n";

        // destructor
        out << "\n    virtual ~" << doc.name << "_UI() {\n";
        for (auto it = all.rbegin(); it != all.rend(); ++it) {
            out << "        delete m_" << (*it)->id << ";\n";
        }
        out << "    }\n";

        out << "};\n\n";
        out << "} // namespace lvgl\n";

        /* 嵌入图片资源：只有在真的用到 "#别名" 时才 include 资源头文件 */
        std::string result = out.str();
        const std::string marker = "//@@ASSETS_INCLUDE@@\n";
        /* 没有资源时保留一个空行，避免生成结果与旧版本产生无意义差异 */
        std::string inc = m_usedResources.empty() ? std::string("\n")
            : "#include \"" + m_assetsInclude + "\"\n";
        auto pos = result.find(marker);
        if (pos != std::string::npos) result.replace(pos, marker.size(), inc);
        return result;
    }
};

} // namespace luic
