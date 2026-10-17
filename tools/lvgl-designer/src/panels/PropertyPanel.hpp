#pragma once
#include <QWidget>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QCheckBox>
#include <QLabel>
#include <QScrollArea>
#include <QGroupBox>
#include <QPushButton>
#include <QColorDialog>
#include <QUndoStack>
#include "model/LuiDocument.hpp"
#include "model/ComponentSchema.hpp"
#include "commands/UndoCommands.hpp"

namespace designer {

class PropertyPanel : public QScrollArea {
    Q_OBJECT
    QWidget* m_content;
    QVBoxLayout* m_topLayout;
    std::string m_currentId;
    LuiDocument* m_doc = nullptr;
    QUndoStack* m_undoStack = nullptr;
    RebuildCallback m_rebuild;

public:
    explicit PropertyPanel(QWidget* parent = nullptr) : QScrollArea(parent) {
        m_content = new QWidget();
        m_topLayout = new QVBoxLayout(m_content);
        m_topLayout->setContentsMargins(4, 4, 4, 4);
        m_topLayout->setSpacing(4);
        setWidget(m_content);
        setWidgetResizable(true);
        m_topLayout->addWidget(new QLabel("Select a widget"));
        setContextMenuPolicy(Qt::NoContextMenu);
    }

    void setDocument(LuiDocument* doc) { m_doc = doc; }
    void setUndoStack(QUndoStack* stack) { m_undoStack = stack; }
    void setRebuildCallback(RebuildCallback rebuild) { m_rebuild = std::move(rebuild); }
    std::string currentId() const { return m_currentId; }

    void showWidget(WidgetNode* node) {
        clearLayout();

        if (!node || !m_doc) {
            m_currentId.clear();
            m_topLayout->addWidget(new QLabel("Select a widget"));
            return;
        }

        m_currentId = node->id;
        WidgetNode* parent = m_doc->findParentOf(node->id);
        const bool is_stacked_child = parent && parent->type == "LStackedLayout";

        auto* idGroup = new QGroupBox("Identity");
        auto* idLayout = new QFormLayout(idGroup);
        idLayout->setContentsMargins(6, 6, 6, 6);

        auto* typeField = new QLineEdit(QString::fromStdString(node->type));
        typeField->setReadOnly(true);
        idLayout->addRow("Type", typeField);

        auto* idField = new QLineEdit(QString::fromStdString(node->id));
        connect(idField, &QLineEdit::editingFinished, [this, idField]() {
            if (!m_doc) return;
            auto* n = m_doc->findById(m_currentId);
            if (n) {
                n->id = idField->text().toStdString();
                m_currentId = n->id;
                m_doc->markModified();
                emit documentModified();
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
                if (is_stacked_child && (prop.name == "x" || prop.name == "y" ||
                                         prop.name == "width" || prop.name == "height")) {
                    continue;
                }

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
                } else if (prop.type == "hex") {
                    auto* container = new QWidget();
                    auto* hbox = new QHBoxLayout(container);
                    hbox->setContentsMargins(0, 0, 0, 0);
                    hbox->setSpacing(4);

                    auto* swatch = new QPushButton();
                    swatch->setFixedSize(32, 22);
                    auto* hexField = new QLineEdit(val.toString());

                    QString hexStr = val.toString();
                    hexStr.remove("0x").remove("0X");
                    QColor initialColor = QColor(hexStr.toInt(nullptr, 16));
                    if (!initialColor.isValid()) initialColor = QColor(0, 0, 0);
                    swatch->setStyleSheet(QString("background-color: %1; border: 1px solid #555; border-radius: 2px;").arg(initialColor.name()));

                    connect(swatch, &QPushButton::clicked, [this, hexField, swatch]() {
                        QString curHex = hexField->text();
                        curHex.remove("0x").remove("0X");
                        QColor curColor = QColor(curHex.toInt(nullptr, 16));
                        if (!curColor.isValid()) curColor = QColor(0, 0, 1);

                        QColor chosen = QColorDialog::getColor(curColor, nullptr, "Select Color");
                        if (chosen.isValid()) {
                            QString newHex = "0x" + QString("%1%2%3")
                                .arg(chosen.red(), 2, 16, QChar('0'))
                                .arg(chosen.green(), 2, 16, QChar('0'))
                                .arg(chosen.blue(), 2, 16, QChar('0'))
                                .toUpper();
                            hexField->setText(newHex);
                            swatch->setStyleSheet(QString("background-color: %1; border: 1px solid #555; border-radius: 2px;").arg(chosen.name()));
                            emit hexField->editingFinished();
                        }
                    });

                    connect(hexField, &QLineEdit::editingFinished, [this, hexField, swatch, key]() {
                        QString colorHex = hexField->text();
                        colorHex.remove("0x").remove("0X");
                        QColor c = QColor(colorHex.toInt(nullptr, 16));
                        if (c.isValid()) {
                            swatch->setStyleSheet(QString("background-color: %1; border: 1px solid #555; border-radius: 2px;").arg(c.name()));
                        }
                        writeProp(key, QVariant(hexField->text()));
                    });
                    glayout->addRow(qkey, container);
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
                } else if (prop.type == "string" &&
                           (key == "options" || key == "map" || key == "data" ||
                            key == "items" || key == "tabs" || key == "series" ||
                            key == "column_widths")) {
                    auto* field = new QPlainTextEdit(val.toString());
                    field->setTabChangesFocus(true);
                    field->setMinimumHeight(72);
                    connect(field, &QPlainTextEdit::textChanged, [this, field, key]() {
                        writeProp(key, QVariant(field->toPlainText()));
                    });
                    glayout->addRow(qkey, field);
                } else {
                    auto* field = new QLineEdit(val.toString());
                    connect(field, &QLineEdit::editingFinished, [this, field, key, prop]() {
                        QVariant newVal = (prop.type == "int") ? QVariant(field->text().toInt()) : QVariant(field->text());
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

signals:
    void documentModified();

private:
    QVariant readProp(WidgetNode* node, const std::string& key, const QVariant& def) {
        if (!node) return def;
        if (key == "x") {
            return QVariant((int)node->pos().x());
        } else if (key == "y") {
            return QVariant((int)node->pos().y());
        } else if (key == "width") {
            return QVariant((int)node->size().width());
        } else if (key == "height") {
            return QVariant((int)node->size().height());
        }
        auto it = node->props.find(key);
        if (it != node->props.end()) return it->second;
        return def;
    }

    void writeProp(const std::string& key, const QVariant& newVal) {
        if (!m_doc) return;
        auto* node = m_doc->findById(m_currentId);
        if (!node) return;

        QVariant oldVal = readProp(node, key, QVariant());

        if (key == "x" || key == "y") {
            QPointF p = node->pos();
            if (key == "x") p.setX(newVal.toInt());
            else p.setY(newVal.toInt());
            QVariantList oldPos, newPos;
            oldPos.append(QVariant((int)node->pos().x()));
            oldPos.append(QVariant((int)node->pos().y()));
            newPos.append(QVariant((int)p.x()));
            newPos.append(QVariant((int)p.y()));
            if (m_undoStack) {
                m_undoStack->push(new PropertyChangeCommand(m_doc, m_rebuild, m_currentId, "pos", QVariant(oldPos), QVariant(newPos)));
            } else {
                node->setPos(p);
                if (m_rebuild) m_rebuild();
            }
            m_doc->markModified();
            emit documentModified();
            return;
        }
        if (key == "width" || key == "height") {
            QSizeF s = node->size();
            if (key == "width") s.setWidth(newVal.toInt());
            else s.setHeight(newVal.toInt());
            QVariant oldSize, newSize;
            if (node->usesSquareSize()) {
                oldSize = node->props.count("size") ? node->props["size"] : QVariant((int)node->size().width());
                newSize = QVariant((int)s.width());
            } else {
                QVariantList oldSl;
                oldSl.append(QVariant((int)node->size().width()));
                oldSl.append(QVariant((int)node->size().height()));
                oldSize = QVariant(oldSl);
                QVariantList ns;
                ns.append(QVariant((int)s.width()));
                ns.append(QVariant((int)s.height()));
                newSize = QVariant(ns);
            }
            if (m_undoStack) {
                m_undoStack->push(new PropertyChangeCommand(m_doc, m_rebuild, m_currentId, "size", oldSize, newSize));
            } else {
                node->setSize(s);
                if (m_rebuild) m_rebuild();
            }
            m_doc->markModified();
            emit documentModified();
            return;
        }

        if (m_undoStack) {
            m_undoStack->push(new PropertyChangeCommand(m_doc, m_rebuild, m_currentId, key, oldVal, newVal));
        } else {
            node->props[key] = newVal;
            if (m_rebuild) m_rebuild();
        }
        m_doc->markModified();
        emit documentModified();
    }

    void clearLayout() {
        QLayoutItem* item;
        while ((item = m_topLayout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                item->widget()->deleteLater();
            } else {
                delete item;
            }
        }
    }
};

}
