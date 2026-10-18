#pragma once
#include <string>
#include <vector>
#include <map>

namespace luic {

struct property_t {
    std::string name;
    std::string type;   // "string", "int", "hex", "int_pair", "int_triple", "bool"
    std::string raw;    // raw JSON value as string
    int int_val = 0;
    int int_val2 = 0;   // for int_pair
    int int_val3 = 0;   // for int_triple
    std::string str_val;
    bool bool_val = false;
};

struct widget_node_t {
    std::string type;   // "LButton", "LLabel", etc.
    std::string id;
    std::vector<property_t> props;
    std::vector<std::string> events;
    std::vector<widget_node_t> children;
};

struct lui_document_t {
    std::string version;
    std::string name;
    /* "screen"（默认）或 "component"：组件没有屏幕语义，生成独立根容器 */
    std::string kind = "screen";
    int width = 800;
    int height = 480;
    bool hadSize = false;          /* JSON 里是否显式写了 size（组件不允许） */
    std::string filePath;          /* 源文件路径，用于工程模式推导输出名/依赖 */
    /* 导出参数：对外名 -> "<节点id>.<属性名>"，仅组件可用 */
    std::map<std::string, std::string> exports;
    /* 图片资源：别名 -> 路径（相对 .lui 所在目录）。控件里用 "#别名" 引用，
     * 生成代码时变成编译期嵌入的 lv_image_dsc_t 指针。 */
    std::map<std::string, std::string> resources;
    std::vector<widget_node_t> children;

    bool is_component() const { return kind == "component"; }

    void collect_all_nodes(const std::vector<widget_node_t>& nodes,
                           std::vector<const widget_node_t*>& out) const {
        for (auto& n : nodes) {
            out.push_back(&n);
            collect_all_nodes(n.children, out);
        }
    }

    std::vector<const widget_node_t*> all_nodes() const {
        std::vector<const widget_node_t*> result;
        collect_all_nodes(children, result);
        return result;
    }
};

} // namespace luic
