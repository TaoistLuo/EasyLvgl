#pragma once
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QPointF>
#include <QSizeF>
#include "model/ComponentSchema.hpp"

namespace designer {

struct WidgetNode {
    std::string type;
    std::string id;
    std::map<std::string, QVariant> props;
    std::vector<std::string> events;
    std::vector<WidgetNode> children;
    WidgetNode* parent = nullptr;

    QPointF pos() const {
        auto it = props.find("pos");
        if (it != props.end() && it->second.toList().size() == 2) {
            auto list = it->second.toList();
            return QPointF(list[0].toDouble(), list[1].toDouble());
        }
        return QPointF(0, 0);
    }

    QSizeF size() const {
        auto it = props.find("size");
        if (it != props.end()) {
            if (it->second.toList().size() == 2) {
                auto list = it->second.toList();
                return QSizeF(list[0].toDouble(), list[1].toDouble());
            }
            double s = it->second.toDouble();
            if (s > 0) return QSizeF(s, s);
        }
        return defaultSize();
    }

    void setPos(QPointF p) {
        QVariantList list;
        list.append(QVariant((int)p.x()));
        list.append(QVariant((int)p.y()));
        props["pos"] = QVariant(list);
    }

    bool usesSquareSize() const {
        return type == "LArc" || type == "LSpinner" || type == "LLed";
    }

    void setSize(QSizeF s) {
        if (usesSquareSize()) {
            props["size"] = QVariant((int)s.width());
        } else {
            QVariantList list;
            list.append(QVariant((int)s.width()));
            list.append(QVariant((int)s.height()));
            props["size"] = QVariant(list);
        }
    }

    QSizeF defaultSize() const {
        if (type == "LButton") return QSizeF(120, 50);
        if (type == "LLabel") return QSizeF(100, 30);
        if (type == "LSlider") return QSizeF(200, 10);
        if (type == "LBar") return QSizeF(200, 20);
        if (type == "LCheckbox") return QSizeF(140, 30);
        if (type == "LSwitch") return QSizeF(50, 25);
        if (type == "LDropdown") return QSizeF(150, 40);
        if (type == "LTextArea") return QSizeF(200, 80);
        if (type == "LArc") return QSizeF(100, 100);
        if (type == "LSpinner") return QSizeF(50, 50);
        if (type == "LLed") return QSizeF(30, 30);
        if (type == "LVStack") return QSizeF(300, 400);
        if (type == "LHStack") return QSizeF(400, 100);
        if (type == "LTable") return QSizeF(300, 200);
        if (type == "LChart") return QSizeF(300, 200);
        if (type == "LImage") return QSizeF(100, 100);
        return QSizeF(100, 40);
    }

    std::string propString(const std::string& key, const std::string& def = "") const {
        auto it = props.find(key);
        if (it != props.end()) return it->second.toString().toStdString();
        return def;
    }

    int propInt(const std::string& key, int def = 0) const {
        auto it = props.find(key);
        if (it != props.end()) return it->second.toInt();
        return def;
    }

    bool propBool(const std::string& key, bool def = false) const {
        auto it = props.find(key);
        if (it != props.end()) return it->second.toBool();
        return def;
    }

    void initDefaultProps() {
        QVariantList pair0;
        pair0.append(QVariant(0));
        pair0.append(QVariant(0));
        QVariantList pair100;
        pair100.append(QVariant(0));
        pair100.append(QVariant(100));

        if (type == "LButton") {
            props.emplace("text", QVariant(QString("Button")));
            props.emplace("disabled", QVariant(false));
            props.emplace("theme", QVariant(QString("primary")));
        } else if (type == "LLabel") {
            props.emplace("text", QVariant(QString("Label")));
            props.emplace("color", QVariant(QString("0xFFFFFF")));
            props.emplace("font_size", QVariant(14));
            props.emplace("long_mode", QVariant(0));
        } else if (type == "LSlider") {
            props.emplace("range", QVariant(pair100));
            props.emplace("value", QVariant(50));
            props.emplace("left_value", QVariant(0));
            props.emplace("mode", QVariant(0));
        } else if (type == "LCheckbox") {
            props.emplace("text", QVariant(QString("Checkbox")));
            props.emplace("checked", QVariant(false));
        } else if (type == "LSwitch") {
            props.emplace("checked", QVariant(false));
            props.emplace("enabled", QVariant(true));
        } else if (type == "LDropdown") {
            props.emplace("options", QVariant(QString("Option 1\\nOption 2\\nOption 3")));
            props.emplace("selected", QVariant(0));
            props.emplace("direction", QVariant(0));
            props.emplace("enabled", QVariant(true));
        } else if (type == "LTextArea") {
            props.emplace("text", QVariant(QString("")));
            props.emplace("placeholder", QVariant(QString("Enter text...")));
            props.emplace("one_line", QVariant(false));
            props.emplace("password_mode", QVariant(false));
            props.emplace("max_length", QVariant(0));
            props.emplace("enabled", QVariant(true));
        } else if (type == "LImage") {
            props.emplace("src", QVariant(QString("")));
            props.emplace("rotation", QVariant(0));
            props.emplace("scale", QVariant(256));
            props.emplace("opacity", QVariant(255));
        } else if (type == "LArc") {
            props.emplace("range", QVariant(pair100));
            props.emplace("value", QVariant(0));
            props.emplace("rotation", QVariant(0));
            props.emplace("mode", QVariant(0));
            QVariantList angles;
            angles.append(QVariant(0));
            angles.append(QVariant(360));
            props.emplace("bg_angles", QVariant(angles));
        } else if (type == "LBar") {
            props.emplace("range", QVariant(pair100));
            props.emplace("value", QVariant(0));
            props.emplace("start_value", QVariant(0));
            props.emplace("mode", QVariant(0));
        } else if (type == "LSpinner") {
            props.emplace("color", QVariant(QString("0x2196F3")));
        } else if (type == "LLed") {
            props.emplace("color", QVariant(QString("0x00FF00")));
            props.emplace("brightness", QVariant(255));
            props.emplace("on", QVariant(true));
        } else if (type == "LLine") {
            props.emplace("width", QVariant(2));
            props.emplace("color", QVariant(QString("0xFFFFFF")));
            props.emplace("y_invert", QVariant(false));
        } else if (type == "LRoller") {
            props.emplace("options", QVariant(QString("Item 1\\nItem 2\\nItem 3")));
            props.emplace("selected", QVariant(0));
            props.emplace("visible_row_count", QVariant(3));
        } else if (type == "LSpinbox") {
            props.emplace("range", QVariant(pair100));
            props.emplace("value", QVariant(0));
            props.emplace("step", QVariant(1));
            QVariantList df;
            df.append(QVariant(3));
            df.append(QVariant(0));
            props.emplace("digit_format", QVariant(df));
            props.emplace("rollover", QVariant(false));
        } else if (type == "LButtonMatrix") {
            props.emplace("one_checked", QVariant(false));
        } else if (type == "LKeyboard") {
            props.emplace("mode", QVariant(0));
            props.emplace("popovers", QVariant(false));
        } else if (type == "LTable") {
            props.emplace("row_count", QVariant(3));
            props.emplace("column_count", QVariant(3));
        } else if (type == "LChart") {
            props.emplace("type", QVariant(0));
            props.emplace("point_count", QVariant(10));
            props.emplace("update_mode", QVariant(0));
        } else if (type == "LScale") {
            props.emplace("mode", QVariant(0));
            props.emplace("range", QVariant(pair100));
            props.emplace("total_tick_count", QVariant(11));
            props.emplace("major_tick_every", QVariant(5));
            props.emplace("rotation", QVariant(0));
            props.emplace("label_show", QVariant(true));
        } else if (type == "LVStack" || type == "LHStack") {
            props.emplace("spacing", QVariant(10));
            props.emplace("padding", QVariant(10));
            props.emplace("align_items", QVariant(0));
        } else if (type == "LSpan") {
            props.emplace("mode", QVariant(0));
            props.emplace("overflow", QVariant(0));
            props.emplace("indent", QVariant(0));
        } else if (type == "LLottie") {
            props.emplace("src", QVariant(QString("")));
            props.emplace("loop", QVariant(true));
            props.emplace("speed", QVariant(1));
        } else if (type == "LMsgBox") {
            props.emplace("title", QVariant(QString("Title")));
            props.emplace("text", QVariant(QString("Message")));
        } else if (type == "LAnimImage") {
            props.emplace("duration", QVariant(1000));
            props.emplace("repeat_count", QVariant(0));
        } else if (type == "LTabView") {
            props.emplace("tab_bar_position", QVariant(0));
            props.emplace("tab_bar_size", QVariant(40));
        } else if (type == "LMenu") {
            props.emplace("mode_header", QVariant(0));
            props.emplace("mode_root_back_button", QVariant(0));
        }

        setSize(defaultSize());
    }
};

class LuiDocument {
    std::string m_version = "1.0";
    std::string m_name = "Unnamed";
    int m_width = 800;
    int m_height = 480;
    std::vector<WidgetNode> m_children;
    QString m_filePath;
    bool m_modified = false;

public:
    LuiDocument() = default;

    bool load(const QString& path) {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) return false;

        QJsonParseError err;
        auto jdoc = QJsonDocument::fromJson(file.readAll(), &err);
        if (err.error != QJsonParseError::NoError) return false;

        auto root = jdoc.object();
        m_version = root["version"].toString("1.0").toStdString();
        m_name = root["name"].toString("Unnamed").toStdString();

        if (root.contains("size") && root["size"].isArray()) {
            auto arr = root["size"].toArray();
            m_width = arr[0].toInt(800);
            m_height = arr[1].toInt(480);
        }

        m_children.clear();
        if (root.contains("children") && root["children"].isArray()) {
            for (auto val : root["children"].toArray()) {
                auto node = parseNode(val.toObject());
                m_children.push_back(std::move(node));
            }
        }

        m_filePath = path;
        m_modified = false;
        return true;
    }

    bool save() { return saveTo(m_filePath); }

    bool saveTo(const QString& path) {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly)) return false;

        QJsonObject root;
        root["version"] = QString::fromStdString(m_version);
        root["name"] = QString::fromStdString(m_name);
        root["size"] = QJsonArray{m_width, m_height};

        QJsonArray children;
        for (auto& child : m_children) {
            children.append(serializeNode(child));
        }
        root["children"] = children;

        QJsonDocument jdoc(root);
        file.write(jdoc.toJson(QJsonDocument::Indented));
        m_filePath = path;
        m_modified = false;
        return true;
    }

    const std::string& name() const { return m_name; }
    void setName(const std::string& n) { m_name = n; m_modified = true; }
    int width() const { return m_width; }
    int height() const { return m_height; }
    void setScreenSize(int w, int h) { m_width = w; m_height = h; m_modified = true; }
    const QString& filePath() const { return m_filePath; }
    bool isModified() const { return m_modified; }
    void markModified() { m_modified = true; }

    std::vector<WidgetNode>& children() { return m_children; }
    const std::vector<WidgetNode>& children() const { return m_children; }

    WidgetNode* findById(const std::string& id) {
        return findByIdIn(m_children, id);
    }

    void addChild(WidgetNode node) {
        m_children.push_back(std::move(node));
        m_modified = true;
    }

    bool removeById(const std::string& id) {
        return removeByIdFrom(m_children, id);
    }

    int nextAutoId() {
        static int counter = 0;
        return counter++;
    }

private:
    WidgetNode parseNode(const QJsonObject& obj) {
        WidgetNode node;
        node.type = obj["type"].toString().toStdString();
        node.id = obj["id"].toString().toStdString();

        if (obj.contains("props") && obj["props"].isObject()) {
            auto propsObj = obj["props"].toObject();
            for (auto it = propsObj.begin(); it != propsObj.end(); ++it) {
                node.props[it.key().toStdString()] = jsonToVariant(it.value());
            }
        }

        if (obj.contains("events") && obj["events"].isArray()) {
            for (auto val : obj["events"].toArray()) {
                node.events.push_back(val.toString().toStdString());
            }
        }

        if (obj.contains("children") && obj["children"].isArray()) {
            for (auto val : obj["children"].toArray()) {
                auto child = parseNode(val.toObject());
                child.parent = &node;
                node.children.push_back(std::move(child));
            }
        }

        return node;
    }

    QJsonObject serializeNode(const WidgetNode& node) {
        QJsonObject obj;
        obj["type"] = QString::fromStdString(node.type);
        obj["id"] = QString::fromStdString(node.id);

        if (!node.props.empty()) {
            QJsonObject propsObj;
            for (auto& [key, val] : node.props) {
                propsObj[QString::fromStdString(key)] = variantToJson(val);
            }
            obj["props"] = propsObj;
        }

        if (!node.events.empty()) {
            QJsonArray evts;
            for (auto& e : node.events) evts.append(QString::fromStdString(e));
            obj["events"] = evts;
        }

        if (!node.children.empty()) {
            QJsonArray ch;
            for (auto& child : node.children) ch.append(serializeNode(child));
            obj["children"] = ch;
        }

        return obj;
    }

    QVariant jsonToVariant(const QJsonValue& val) {
        if (val.isString()) return QVariant(val.toString());
        if (val.isBool()) return QVariant(val.toBool());
        if (val.isDouble()) return QVariant(val.toInt());
        if (val.isArray()) {
            QVariantList list;
            for (int i = 0; i < val.toArray().size(); i++)
                list.append(jsonToVariant(val.toArray().at(i)));
            return QVariant(list);
        }
        return QVariant();
    }

    QJsonValue variantToJson(const QVariant& val) {
        if (val.type() == QVariant::String) return QJsonValue(val.toString());
        if (val.type() == QVariant::Bool) return QJsonValue(val.toBool());
        if (val.type() == QVariant::Int) return QJsonValue(val.toInt());
        if (val.type() == QVariant::Double) return QJsonValue(val.toDouble());
        if (val.type() == QVariant::List) {
            QJsonArray arr;
            for (int i = 0; i < val.toList().size(); i++)
                arr.append(variantToJson(val.toList().at(i)));
            return arr;
        }
        return QJsonValue();
    }

    WidgetNode* findByIdIn(std::vector<WidgetNode>& nodes, const std::string& id) {
        for (auto& n : nodes) {
            if (n.id == id) return &n;
            auto* found = findByIdIn(n.children, id);
            if (found) return found;
        }
        return nullptr;
    }

    bool removeByIdFrom(std::vector<WidgetNode>& nodes, const std::string& id) {
        for (auto it = nodes.begin(); it != nodes.end(); ++it) {
            if (it->id == id) { nodes.erase(it); return true; }
            if (removeByIdFrom(it->children, id)) return true;
        }
        return false;
    }
};

}
