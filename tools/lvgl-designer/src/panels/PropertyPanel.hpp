#pragma once
#include <QWidget>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QScrollArea>
#include <QGroupBox>
#include "canvas/WidgetItem.hpp"
#include "canvas/DesignerScene.hpp"
#include "commands/UndoCommands.hpp"

namespace designer {

class PropertyPanel : public QScrollArea {
    Q_OBJECT
    QWidget* m_content;
    QVBoxLayout* m_topLayout;
    std::string m_currentId;
    DesignerScene* m_scene = nullptr;

public:
    explicit PropertyPanel(QWidget* parent = nullptr) : QScrollArea(parent) {
        m_content = new QWidget();
        m_topLayout = new QVBoxLayout(m_content);
        m_topLayout->setContentsMargins(4, 4, 4, 4);
        m_topLayout->setSpacing(4);
        setWidget(m_content);
        setWidgetResizable(true);
    }

    void setScene(DesignerScene* scene) { m_scene = scene; }

    void showWidget(WidgetItem* item) {
        clearLayout();

        if (!item || !m_scene || !m_scene->document()) {
            m_currentId.clear();
            m_topLayout->addWidget(new QLabel("Select a widget"));
            return;
        }

        m_currentId = item->nodeId();
        auto* node = m_scene->document()->findById(m_currentId);
        if (!node) {
            m_topLayout->addWidget(new QLabel("Node not found"));
            return;
        }

        auto* idGroup = new QGroupBox("Identity");
        auto* idLayout = new QFormLayout(idGroup);
        idLayout->setContentsMargins(6, 6, 6, 6);

        auto* typeField = new QLineEdit(QString::fromStdString(node->type));
        typeField->setReadOnly(true);
        idLayout->addRow("Type", typeField);

        auto* idField = new QLineEdit(QString::fromStdString(node->id));
        connect(idField, &QLineEdit::editingFinished, [this, idField]() {
            if (!m_scene || !m_scene->document()) return;
            auto* n = m_scene->document()->findById(m_currentId);
            if (n) {
                n->id = idField->text().toStdString();
                m_currentId = n->id;
                m_scene->document()->markModified();
                m_scene->scheduleRebuild();
                emit m_scene->documentModified();
            }
        });
        idLayout->addRow("ID", idField);
        m_topLayout->addWidget(idGroup);

        auto groups = ComponentSchema::propsFor(node->type);
        for (auto& group : groups) {
            auto* gbox = new QGroupBox(QString::fromStdString(group.groupName));
            auto* glayout = new QFormLayout(gbox);
            glayout->setContentsMargins(6, 6, 6, 6);

            for (auto& prop : group.props) {
                QString qkey = QString::fromStdString(prop.name);
                std::string key = prop.name;

                QVariant val = readProp(node, key, prop.defaultVal);

                if (prop.type == "bool") {
                    auto* cb = new QCheckBox();
                    cb->setChecked(val.toBool());
                    connect(cb, &QCheckBox::toggled, [this, key](bool checked) {
                        writeProp(key, QVariant(checked));
                    });
                    glayout->addRow(qkey, cb);
                } else if (prop.type == "int_pair" || prop.type == "int_triple") {
                    auto list = val.toList();
                    QString text;
                    for (int i = 0; i < list.size(); i++) {
                        if (i > 0) text += ", ";
                        text += list[i].toString();
                    }
                    auto* field = new QLineEdit(text);
                    connect(field, &QLineEdit::editingFinished, [this, field, key]() {
                        auto parts = field->text().split(",");
                        QVariantList newList;
                        for (int i = 0; i < parts.size(); i++)
                            newList.append(QVariant(parts[i].trimmed().toInt()));
                        writeProp(key, QVariant(newList));
                    });
                    glayout->addRow(qkey, field);
                } else {
                    auto* field = new QLineEdit(val.toString());
                    connect(field, &QLineEdit::editingFinished, [this, field, key, prop]() {
                        QVariant newVal;
                        if (prop.type == "int") {
                            newVal = QVariant(field->text().toInt());
                        } else {
                            newVal = QVariant(field->text());
                        }
                        writeProp(key, newVal);
                    });
                    glayout->addRow(qkey, field);
                }
            }

            m_topLayout->addWidget(gbox);
        }

        m_topLayout->addStretch();
    }

    void clearSelection() {
        m_currentId.clear();
        clearLayout();
        m_topLayout->addWidget(new QLabel("Select a widget"));
    }

private:
    QVariant readProp(WidgetNode* node, const std::string& key, const QVariant& def) {
        if (key == "x") {
            auto it = node->props.find("pos");
            if (it != node->props.end() && it->second.toList().size() >= 2)
                return it->second.toList()[0];
            return def;
        }
        if (key == "y") {
            auto it = node->props.find("pos");
            if (it != node->props.end() && it->second.toList().size() >= 2)
                return it->second.toList()[1];
            return def;
        }
        if (key == "width") {
            auto it = node->props.find("size");
            if (it != node->props.end()) {
                if (it->second.toList().size() >= 2)
                    return it->second.toList()[0];
                return it->second;
            }
            return def;
        }
        if (key == "height") {
            auto it = node->props.find("size");
            if (it != node->props.end()) {
                if (it->second.toList().size() >= 2)
                    return it->second.toList()[1];
                return it->second;
            }
            return def;
        }
        auto it = node->props.find(key);
        return (it != node->props.end()) ? it->second : def;
    }

    void writeProp(const std::string& key, const QVariant& newVal) {
        if (!m_scene || !m_scene->document()) return;
        auto* n = m_scene->document()->findById(m_currentId);
        if (!n) return;

        if (key == "x" || key == "y") {
            auto it = n->props.find("pos");
            QVariantList pos;
            if (it != n->props.end() && it->second.toList().size() >= 2) {
                pos = it->second.toList();
            } else {
                pos.append(QVariant(0));
                pos.append(QVariant(0));
            }
            if (key == "x") pos[0] = newVal;
            else pos[1] = newVal;
            n->props["pos"] = QVariant(pos);
        } else if (key == "width" || key == "height") {
            auto it = n->props.find("size");
            bool square = n->usesSquareSize();
            if (square) {
                n->props["size"] = newVal;
            } else {
                QVariantList sz;
                if (it != n->props.end() && it->second.toList().size() >= 2) {
                    sz = it->second.toList();
                } else {
                    sz.append(QVariant(0));
                    sz.append(QVariant(0));
                }
                if (key == "width") sz[0] = newVal;
                else sz[1] = newVal;
                n->props["size"] = QVariant(sz);
            }
        } else {
            n->props[key] = newVal;
        }

        n->props.erase("_dirty");
        m_scene->document()->markModified();

        if (m_scene->undoStack()) {
            m_scene->undoStack()->push(
                new PropertyChangeCommand(m_scene, m_currentId, key, QVariant(), newVal));
        }

        m_scene->scheduleRebuild();
        emit m_scene->documentModified();
    }

    void clearLayout() {
        QLayoutItem* item;
        while ((item = m_topLayout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                delete item->widget();
            }
            delete item;
        }
    }
};

}
