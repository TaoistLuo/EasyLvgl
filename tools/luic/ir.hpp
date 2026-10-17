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
    int width = 800;
    int height = 480;
    std::vector<widget_node_t> children;

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
