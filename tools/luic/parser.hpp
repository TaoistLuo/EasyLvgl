#pragma once
#include "ir.hpp"
#include "third_party/nlohmann/json.hpp"
#include <fstream>
#include <string>
#include <optional>

namespace luic {

using json = nlohmann::json;

class Parser {
public:
    std::optional<lui_document_t> parse(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            fprintf(stderr, "luic: cannot open file: %s\n", filepath.c_str());
            return std::nullopt;
        }

        json j;
        try {
            file >> j;
        } catch (const json::parse_error& e) {
            fprintf(stderr, "luic: JSON parse error in %s: %s\n",
                    filepath.c_str(), e.what());
            return std::nullopt;
        }

        lui_document_t doc;
        doc.version = j.value("version", "1.0");
        doc.name = j.value("name", "Unnamed");

        if (j.contains("size") && j["size"].is_array() && j["size"].size() == 2) {
            doc.width = j["size"][0].get<int>();
            doc.height = j["size"][1].get<int>();
        }

        if (j.contains("children") && j["children"].is_array()) {
            for (auto& child : j["children"]) {
                auto node = parse_node(child);
                if (node) doc.children.push_back(std::move(*node));
            }
        }

        return doc;
    }

private:
    std::optional<widget_node_t> parse_node(const json& j) {
        if (!j.contains("type") || !j["type"].is_string()) {
            fprintf(stderr, "luic: node missing 'type' field\n");
            return std::nullopt;
        }

        widget_node_t node;
        node.type = j["type"].get<std::string>();
        node.id = j.value("id", "");

        if (node.id.empty()) {
            static int auto_id = 0;
            node.id = "widget_" + std::to_string(auto_id++);
        }

        if (j.contains("props") && j["props"].is_object()) {
            for (auto& [key, val] : j["props"].items()) {
                property_t prop;
                prop.name = key;
                prop.raw = val.dump();

                if (val.is_string()) {
                    prop.type = "string";
                    prop.str_val = val.get<std::string>();
                    if (prop.str_val.substr(0, 2) == "0x" || prop.str_val.substr(0, 2) == "0X") {
                        prop.type = "hex";
                    }
                } else if (val.is_number_integer()) {
                    prop.type = "int";
                    prop.int_val = val.get<int>();
                } else if (val.is_boolean()) {
                    prop.type = "bool";
                    prop.bool_val = val.get<bool>();
                } else if (val.is_array() && val.size() == 2) {
                    if (val[0].is_number_integer() && val[1].is_number_integer()) {
                        prop.type = "int_pair";
                        prop.int_val = val[0].get<int>();
                        prop.int_val2 = val[1].get<int>();
                    } else {
                        prop.type = "array";
                    }
                } else if (val.is_array() && val.size() == 3) {
                    if (val[0].is_number_integer() && val[1].is_number_integer() && val[2].is_number_integer()) {
                        prop.type = "int_triple";
                        prop.int_val = val[0].get<int>();
                        prop.int_val2 = val[1].get<int>();
                        prop.int_val3 = val[2].get<int>();
                    } else {
                        prop.type = "array";
                    }
                }

                node.props.push_back(std::move(prop));
            }
        }

        if (j.contains("events") && j["events"].is_array()) {
            for (auto& evt : j["events"]) {
                if (evt.is_string()) {
                    node.events.push_back(evt.get<std::string>());
                }
            }
        }

        if (j.contains("children") && j["children"].is_array()) {
            for (auto& child : j["children"]) {
                auto child_node = parse_node(child);
                if (child_node) node.children.push_back(std::move(*child_node));
            }
        }

        return node;
    }
};

} // namespace luic
