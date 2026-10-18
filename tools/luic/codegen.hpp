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
    /* 工程模式：名字 -> 文档，用于展开 "@Name" 实例与校验 */
    const std::map<std::string, const lui_document_t*>* m_components = nullptr;
    std::set<std::string> m_usedComponents;   // 需要 include 的组件头

    static bool is_instance(const widget_node_t& n) {
        return !n.type.empty() && n.type[0] == '@';
    }
    static std::string instance_class(const widget_node_t& n) {
        return n.type.substr(1) + "_UI";
    }
    static std::string ui_header_of(const lui_document_t& d) {
        std::string base = d.filePath.empty() ? d.name : d.filePath;
        auto slash = base.find_last_of("/\\");
        if (slash != std::string::npos) base = base.substr(slash + 1);
        auto dot = base.rfind('.');
        if (dot != std::string::npos) base = base.substr(0, dot);
        return base + "_ui.hpp";
    }
    /* 样式属性 -> "st.set_xxx(value);" */
    std::string style_setter_call(const property_t& prop) {
        std::string k = style_kind(prop.name);
        if (k.empty()) return "";
        std::string val;
        if (k == "color") val = prop.str_val.empty() ? prop.raw : prop.str_val;
        else if (k == "bool") val = prop.bool_val ? "true" : "false";
        else if (k == "src") val = src_value(prop.str_val);
        else val = std::to_string(prop.int_val);
        return "st.set_" + prop.name + "(" + val + ");";
    }
    static std::string style_type_of(const std::string& name) {
        const auto& m = style_props();
        auto it = m.find(name);
        if (it == m.end()) return "";
        const std::string& k = it->second;
        if (k == "color") return "hex";
        if (k == "bool") return "bool";
        if (k == "src") return "string";
        return "int";
    }
    /* 导出目标：返回 {目标节点 id 或 "root", 属性名, 属性类型} */
    static bool resolve_export(const lui_document_t& doc, const std::string& path,
                               std::string& nodeId, std::string& propName) {
        auto dot = path.find('.');
        if (dot == std::string::npos) return false;
        nodeId = path.substr(0, dot);
        propName = path.substr(dot + 1);
        if (nodeId.rfind("m_", 0) == 0) nodeId = nodeId.substr(2);
        return true;
    }

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

    /* .lui 里多行文本用字面量 "\\n" 表示换行（设计器就是这么写的），
     * 生成 C++ 字符串前先还原成真正的换行，否则 LVGL 会把它当作字面量 */
    static std::string unescape_newlines(std::string s) {
        size_t pos = 0;
        while ((pos = s.find("\\n", pos)) != std::string::npos) {
            s.replace(pos, 2, "\n");
            pos += 1;
        }
        return s;
    }

    std::string format_value(const prop_def_t& def, const property_t& prop) {
        if (def.type == "string") {
            if (is_src_prop(def, prop.name)) return src_value(prop.str_val);
            return "\"" + escape_string(unescape_newlines(prop.str_val)) + "\"";
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
        if (is_instance(node)) {
            gen_instance(out, node, parent_expr, indent);
            return;
        }
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
            /* 下拉框的 symbol 为空时会传给图片 API（空串当图片源）→ 绘制阶段崩溃，
             * 空 symbol 表示"用主题默认箭头"，跳过 */
            if (prop.name == "symbol" && prop.str_val.empty()) continue;
            /* 下拉框 text 为空串时会覆盖"显示已选项"的行为（LVGL 一旦有 text 就不显示选中项） */
            if (node.type == "LComboBox" && prop.name == "text" && prop.str_val.empty()) continue;

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
                /* 组件自身的同名属性优先（如 LLabel.color、LLine.width），
                 * 否则会额外生成一条语义完全不同的样式调用（set_color=线色） */
                if (comp->find_prop(prop.name)) continue;
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
    void setComponents(const std::map<std::string, const lui_document_t*>* comps) {
        m_components = comps;
    }

    std::string generate(const lui_document_t& doc) {
        m_usedResources.clear();
        m_usedComponents.clear();
        m_pendingRefs.clear();
        std::ostringstream out;
        auto all = doc.all_nodes();

        const bool isComp = doc.is_component();

        out << "// Auto-generated by luic from " << doc.name << ".lui\n";
        out << "// DO NOT EDIT — changes will be overwritten\n";
        out << "#pragma once\n";
        out << "#include \"lvgl++/lvgl_widgets.hpp\"\n";
        out << "//@@ASSETS_INCLUDE@@\n";
        out << "//@@COMPONENT_INCLUDES@@\n";
        out << "namespace lvgl {\n\n";
        out << "class " << doc.name << "_UI {\n";

        // protected members
        out << "protected:\n";
        if (isComp) {
            out << "    LObj* m_root = nullptr;   // 组件根容器\n";
        }
        for (auto* n : all) {
            if (is_instance(*n)) {
                out << "    " << instance_class(*n) << "* m_" << n->id << " = nullptr;\n";
                m_usedComponents.insert(n->type.substr(1));
            } else {
                out << "    " << n->type << "* m_" << n->id << " = nullptr;\n";
            }
        }

        // setup()
        out << "\npublic:\n";
        if (!isComp) {
            out << "    static constexpr int kScreenWidth = " << doc.width << ";\n";
            out << "    static constexpr int kScreenHeight = " << doc.height << ";\n";
        }
        out << "    void setup(lv_obj_t* parent = nullptr) {\n";
        if (isComp) {
            out << "        // 组件根容器：透明、无内边距、尺寸随内容\n";
            out << "        m_root = new LObj(parent);\n";
            out << "        m_root->set_size(LV_SIZE_CONTENT, LV_SIZE_CONTENT);\n\n";
        } else {
            out << "        lv_obj_t* root = parent ? parent : lv_scr_act();\n\n";
        }
        const std::string parent_expr = isComp ? "m_root->native()" : "root";
        for (auto& child : doc.children) {
            gen_node(out, child, parent_expr, 8);
            out << "\n";
        }
        if (!m_pendingRefs.empty()) {
            out << "        // cross-references\n";
            for (auto& r : m_pendingRefs) out << "        " << r << "\n";
            out << "\n";
            m_pendingRefs.clear();
        }
        out << "    }\n";

        // 组件的实例 API + 导出参数
        if (isComp) {
            out << "\n    // ---- 实例 API ----\n";
            out << "    " << doc.name << "_UI& set_pos(int x, int y) { m_root->set_pos(x, y); return *this; }\n";
            out << "    " << doc.name << "_UI& set_size(int w, int h) { m_root->set_size(w, h); return *this; }\n";
            out << "    " << doc.name << "_UI& set_align(lv_align_t a) { m_root->set_align(a); return *this; }\n";
            out << "    lv_obj_t* root() const { return m_root->native(); }\n";
            gen_export_setters(out, doc);
        }

        // bind_events()
        out << "\n    void bind_events() {\n";
        for (auto* n : all) {
            if (is_instance(*n)) continue;
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
            if (is_instance(*n)) continue;
            for (auto& evt_name : n->events) {
                out << "\n    virtual void on_" << n->id
                    << "_" << evt_name << "(lv_event_t* e) {}";
            }
        }
        out << "\n";

        // destructor：先删子节点（逆序），组件的根最后删
        out << "\n    virtual ~" << doc.name << "_UI() {\n";
        for (auto it = all.rbegin(); it != all.rend(); ++it) {
            out << "        delete m_" << (*it)->id << ";\n";
        }
        if (isComp) out << "        delete m_root;\n";
        out << "    }\n";

        out << "};\n\n";
        out << "} // namespace lvgl\n";

        std::string result = out.str();
        /* 嵌入图片资源头（只有真的用到 "#别名" 时才 include） */
        {
            const std::string marker = "//@@ASSETS_INCLUDE@@\n";
            std::string inc = m_usedResources.empty() ? std::string("\n")
                : "#include \"" + m_assetsInclude + "\"\n";
            auto pos = result.find(marker);
            if (pos != std::string::npos) result.replace(pos, marker.size(), inc);
        }
        /* 组件头（按名字排序保证稳定输出） */
        {
            const std::string marker = "//@@COMPONENT_INCLUDES@@\n";
            std::string inc;
            if (m_components) {
                for (auto& name : m_usedComponents) {
                    auto dit = m_components->find(name);
                    if (dit != m_components->end())
                        inc += "#include \"" + ui_header_of(*dit->second) + "\"\n";
                }
            }
            auto pos = result.find(marker);
            if (pos != std::string::npos) result.replace(pos, marker.size(), inc);
        }
        return result;
    }

    /* 为组件的每个导出项生成一个链式 setter */
    void gen_export_setters(std::ostringstream& out, const lui_document_t& doc) {
        if (doc.exports.empty()) return;
        out << "\n    // ---- 导出参数 ----\n";
        for (auto& kv : doc.exports) {
            const std::string& alias = kv.first;
            std::string nodeId, propName;
            if (!resolve_export(doc, kv.second, nodeId, propName)) continue;

            const widget_node_t* target = nullptr;
            if (nodeId != "root") {
                for (auto* n : doc.all_nodes())
                    if (n->id == nodeId) target = n;
            }
            if (!target && nodeId != "root") continue;

            /* 目标属性类型 */
            std::string ptype;
            std::string call;
            if (target) {
                auto* cdef = m_registry.find(target->type);
                auto* pdef = cdef ? cdef->find_prop(propName) : nullptr;
                if (pdef) {
                    ptype = pdef->type;
                    call = "m_" + target->id + "->" + pdef->method + "(";
                    if (!pdef->prefix_arg.empty()) call = "m_" + target->id + "->" +
                        pdef->method + "(" + pdef->prefix_arg + ", ";
                    call += "@@ARGS@@);";
                }
            }
            if (ptype.empty()) {
                ptype = style_type_of(propName);
                if (ptype.empty()) continue;
                std::string objExpr = (nodeId == "root") ? "m_root->native()"
                                                         : ("m_" + nodeId + "->native()");
                call = "{ LStyle st; st.set_" + propName + "(@@ARGS@@); st.apply_to(" +
                       objExpr + "); }";
            }

            std::string argDecl, argUse;
            if (ptype == "int") { argDecl = "int v"; argUse = "v"; }
            else if (ptype == "bool") { argDecl = "bool v"; argUse = "v"; }
            else if (ptype == "hex") { argDecl = "uint32_t v"; argUse = "v"; }
            else if (ptype == "string") { argDecl = "const char* v"; argUse = "v"; }
            else if (ptype == "int_pair") { argDecl = "int v1, int v2"; argUse = "v1, v2"; }
            else if (ptype == "int_triple") { argDecl = "int v1, int v2, int v3"; argUse = "v1, v2, v3"; }
            else { argDecl = "int v"; argUse = "v"; }

            std::string body = call;
            auto at = body.find("@@ARGS@@");
            if (at != std::string::npos) body.replace(at, 8, argUse);

            out << "    " << doc.name << "_UI& set_" << alias << "(" << argDecl
                << ") { " << body << " return *this; }\n";
        }
    }

    /* 组件实例："@MyCard" */
    void gen_instance(std::ostringstream& out, const widget_node_t& node,
                      const std::string& parent_expr, int indent) {
        std::string pad(indent, ' ');
        std::string cls = instance_class(node);
        std::string compName = node.type.substr(1);
        m_usedComponents.insert(compName);

        out << pad << "m_" << node.id << " = new " << cls << "();\n";
        out << pad << "m_" << node.id << "->setup(" << parent_expr << ");\n";

        const lui_document_t* cdoc = nullptr;
        if (m_components) {
            auto it = m_components->find(compName);
            if (it != m_components->end()) cdoc = it->second;
        }

        for (auto& p : node.props) {
            if (p.name == "pos" && p.type == "int_pair") {
                out << pad << "m_" << node.id << "->set_pos(" << p.int_val << ", " << p.int_val2 << ");\n";
            } else if (p.name == "size" && p.type == "int_pair") {
                out << pad << "m_" << node.id << "->set_size(" << p.int_val << ", " << p.int_val2 << ");\n";
            } else if (p.name == "align" && p.type == "int") {
                out << pad << "lv_obj_set_align(m_" << node.id << "->root(), (lv_align_t)" << p.int_val << ");\n";
            } else if (p.name == "hidden" && p.type == "bool") {
                out << pad << "lv_obj_" << (p.bool_val ? "add" : "remove")
                    << "_flag(m_" << node.id << "->root(), LV_OBJ_FLAG_HIDDEN);\n";
            } else if (p.name == "clickable" && p.type == "bool") {
                out << pad << "lv_obj_" << (p.bool_val ? "add" : "remove")
                    << "_flag(m_" << node.id << "->root(), LV_OBJ_FLAG_CLICKABLE);\n";
            } else if (!style_kind(p.name).empty()) {
                out << pad << "{ LStyle st; " << style_setter_call(p)
                    << " st.apply_to(m_" << node.id << "->root()); }\n";
            } else if (cdoc && cdoc->exports.count(p.name)) {
                /* 导出参数：按目标属性类型格式化实参 */
                std::string nodeId, propName, ptype;
                resolve_export(*cdoc, cdoc->exports.at(p.name), nodeId, propName);
                const widget_node_t* target = nullptr;
                if (nodeId != "root")
                    for (auto* n : cdoc->all_nodes()) if (n->id == nodeId) target = n;
                if (target) {
                    auto* cdef = m_registry.find(target->type);
                    auto* pdef = cdef ? cdef->find_prop(propName) : nullptr;
                    if (pdef) ptype = pdef->type;
                }
                if (ptype.empty()) ptype = style_type_of(propName);

                std::string arg;
                if (ptype == "hex") arg = p.str_val.empty() ? p.raw : p.str_val;
                else if (ptype == "bool") arg = p.bool_val ? "true" : "false";
                else if (ptype == "string") arg = "\"" + escape_string(p.str_val) + "\"";
                else if (ptype == "int_pair")
                    arg = std::to_string(p.int_val) + ", " + std::to_string(p.int_val2);
                else arg = std::to_string(p.int_val);
                out << pad << "m_" << node.id << "->set_" << p.name << "(" << arg << ");\n";
            }
        }

        for (auto& child : node.children) {
            gen_node(out, child, "m_" + node.id + "->root()", indent);
            out << "\n";
        }
    }
};

} // namespace luic
