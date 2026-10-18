/**************************************************************************************************
 * @file        PropertyPanel.hpp
 * @brief       EasyLVGL 设计器
 * @details     EasyLVGL —— 让 LVGL 开发像 Qt 一样简单：C++ 封装 + 可视化设计器 + 代码生成。
 *
 * @author      taoist.luo
 * @version     1.0.0
 * @date        2026-10-18
 *
 * Copyright (c) 2026 taoist.luo (https://github.com/TaoistLuo/EasyLVGL)
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *************************************************************************************************/

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
#include <QComboBox>
#include <QCompleter>
#include <QToolButton>
#include <QSignalBlocker>
#include <map>
#include <vector>
#include <set>
#include <QUndoStack>
#include <functional>
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
    std::function<void(int, int)> m_onRootSize;
    /** 工程组件解析（"@Name" 实例的参数来自被引用组件的 exports） */
    std::function<const LuiDocument*(const std::string&)> m_componentResolver;
    /** 查询对象当前实际生效的样式值；返回无效 QVariant 表示不是样式属性 */
    std::function<QVariant(const std::string&, const std::string&)> m_styleValue;
    /** 字段刷新器：只改显示值、不重建控件，用于重建后同步"真实生效值" */
    std::map<std::string, std::function<void(const QVariant&)>> m_fieldSetters;
    std::map<std::string, QVariant> m_fieldDefaults;

public:
    explicit PropertyPanel(QWidget* parent = nullptr) : QScrollArea(parent) {
        m_content = new QWidget();
        m_topLayout = new QVBoxLayout(m_content);
        m_topLayout->setContentsMargins(4, 4, 4, 4);
        m_topLayout->setSpacing(4);
        setWidget(m_content);
        setWidgetResizable(true);
        m_topLayout->addWidget(new QLabel("请选择组件"));
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
            m_topLayout->addWidget(new QLabel("请选择组件"));
            return;
        }

        m_currentId = node->id;
        WidgetNode* parent = m_doc->findParentOf(node->id);
        const bool is_stacked_child = parent && parent->type == "LStackedLayout";

        auto* idGroup = new QGroupBox("标识");
        auto* idLayout = new QFormLayout(idGroup);
        idLayout->setContentsMargins(6, 6, 6, 6);

        auto* typeField = new QLineEdit(QString::fromStdString(node->type));
        typeField->setReadOnly(true);
        idLayout->addRow("类型", typeField);

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

        const bool isInstance = !node->type.empty() && node->type[0] == '@';
        auto groups = isInstance ? instanceGroups(node) : ComponentSchema::propsFor(node->type);
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
                // 优先显示 LVGL 里实际生效的值，其次 schema 默认值
                QVariant val = valueFor(node, key, prop.defaultVal);
                m_fieldDefaults[key] = prop.defaultVal;

                const bool srcLike = key == "src" || key.rfind("src_", 0) == 0 ||
                                     (key.size() > 4 && key.compare(key.size() - 4, 4, "_src") == 0);

                if (auto* opts = enumOptionsFor(node->type, key)) {
                    /* 枚举属性用下拉框选择，不让用户记 LVGL 的数字取值 */
                    auto* combo = new QComboBox();
                    int cur = val.toInt();
                    int curIdx = -1;
                    for (size_t i = 0; i < opts->size(); ++i) {
                        combo->addItem(QString::fromUtf8((*opts)[i].label), (*opts)[i].value);
                        combo->setItemData((int)i, QString::fromUtf8((*opts)[i].symbol), Qt::ToolTipRole);
                        if ((*opts)[i].value == cur) curIdx = (int)i;
                    }
                    if (curIdx < 0) {
                        combo->addItem(QString("自定义 (%1)").arg(cur), cur);
                        curIdx = combo->count() - 1;
                    }
                    combo->setCurrentIndex(curIdx);
                    connect(combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                            [this, combo, key](int idx) {
                                if (idx < 0) return;
                                writeProp(key, QVariant(combo->itemData(idx).toInt()));
                            });
                    glayout->addRow(qkey, combo);
                } else if (auto* sopts = stringOptionsFor(node->type, key)) {
                    auto* combo = new QComboBox();
                    QString curStr = val.toString();
                    int curIdx = 0;
                    for (size_t i = 0; i < sopts->size(); ++i) {
                        combo->addItem(QString::fromUtf8((*sopts)[i].label),
                                       QString::fromUtf8((*sopts)[i].value));
                        if (curStr == QString::fromUtf8((*sopts)[i].value)) curIdx = (int)i;
                    }
                    combo->setCurrentIndex(curIdx);
                    connect(combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                            [this, combo, key](int idx) {
                                if (idx < 0) return;
                                writeProp(key, combo->itemData(idx).toString());
                            });
                    glayout->addRow(qkey, combo);
                } else if (key == "textarea" || key == "active_page") {
                    /* 引用其它对象的属性：列出候选对象的 ID，避免手输 ID 输错后静默失效 */
                    auto* combo = new QComboBox();
                    combo->addItem("（未指定）", QString(""));
                    std::set<std::string> want = key == "textarea"
                        ? std::set<std::string>{"LLineEdit", "LTextEdit"}
                        : std::set<std::string>{"LMenuPage"};
                    std::function<void(const std::vector<WidgetNode>&)> addRefs =
                        [&](const std::vector<WidgetNode>& nodes) {
                            for (auto& n : nodes) {
                                if (want.count(n.type))
                                    combo->addItem(QString::fromStdString(n.id + "（" + n.type + "）"),
                                                   QString::fromStdString(n.id));
                                addRefs(n.children);
                            }
                        };
                    addRefs(m_doc->children());
                    QString curRef = val.toString();
                    int curIdx = 0;
                    for (int i = 1; i < combo->count(); ++i)
                        if (combo->itemData(i).toString() == curRef) curIdx = i;
                    if (curIdx == 0 && !curRef.isEmpty()) {
                        combo->addItem(curRef + "（不存在）", curRef);
                        curIdx = combo->count() - 1;
                    }
                    combo->setCurrentIndex(curIdx);
                    connect(combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                            [this, combo, key](int idx) {
                                if (idx < 0) return;
                                writeProp(key, combo->itemData(idx).toString());
                            });
                    glayout->addRow(qkey, combo);
                } else if (prop.type == "bool") {
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
                    hbox->addWidget(swatch);
                    hbox->addWidget(hexField);
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
                            key == "spans" || key == "points" || key == "column_widths")) {
                    auto* field = new QPlainTextEdit(val.toString());
                    field->setTabChangesFocus(true);
                    field->setMinimumHeight(72);
                    connect(field, &QPlainTextEdit::textChanged, [this, field, key]() {
                        writeProp(key, QVariant(field->toPlainText()));
                    });
                    glayout->addRow(qkey, field);
                } else if (srcLike && m_doc) {
                    /* 图片资源：可编辑下拉，右侧小三角展开候选（类似 Qt 的属性编辑器） */
                    auto* combo = new QComboBox();
                    combo->setEditable(true);
                    combo->addItem("");                       // 未设置
                    for (auto& kv : m_doc->allResources())
                        combo->addItem(QString::fromStdString("#" + kv.first));
                    combo->setCurrentText(val.toString());
                    connect(combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
                            this, [this, combo, key](int i) {
                                if (i < 0) return;
                                writeProp(key, combo->itemText(i));
                            });
                    if (auto* le = combo->lineEdit()) {
                        connect(le, &QLineEdit::editingFinished, this, [this, combo, key] {
                            writeProp(key, combo->currentText());
                        });
                    }
                    glayout->addRow(qkey, combo);
                } else {
                    auto* field = new QLineEdit(val.toString());
                    /* 资源是工程级的：要用 allResources()（文件 + 工程合并） */
                    if (srcLike && m_doc && !m_doc->allResources().empty()) {
                        QStringList names;
                        for (auto& kv : m_doc->allResources())
                            names << QString::fromStdString("#" + kv.first);
                        auto* completer = new QCompleter(names, field);
                        completer->setCaseSensitivity(Qt::CaseInsensitive);
                        completer->setCompletionMode(QCompleter::PopupCompletion);
                        completer->setFilterMode(Qt::MatchStartsWith);
                        field->setCompleter(completer);
                        /* 输入 "#" 时立即弹出（QCompleter 默认只在有匹配前缀后弹） */
                        connect(field, &QLineEdit::textEdited, field,
                                [completer](const QString& t) {
                            if (t == "#" || t.endsWith("/")) {
                                completer->setCompletionPrefix(t);
                                completer->complete();
                            }
                        });
                    }
                    connect(field, &QLineEdit::editingFinished, [this, field, key, prop]() {
                        QVariant newVal = (prop.type == "int") ? QVariant(field->text().toInt()) : QVariant(field->text());
                        writeProp(key, newVal);
                    });
                    glayout->addRow(qkey, field);
                }
            }
            addExportButtons(glayout, node);
            registerFields(glayout);
            m_topLayout->addWidget(gbox);
        }

        m_topLayout->addStretch();
    }

    /** 重建后同步显示值（例如新加组件、撤销/重做之后） */
    void refreshValues() {
        if (!m_doc || m_currentId.empty()) return;
        // 正在编辑某个字段时不要回写，否则光标会跳
        QWidget* fw = window()->focusWidget();
        if (fw && m_content->isAncestorOf(fw)) return;
        auto* node = m_doc->findById(m_currentId);
        if (!node) return;
        for (auto& kv : m_fieldSetters) {
            auto d = m_fieldDefaults.find(kv.first);
            QVariant v = valueFor(node, kv.first,
                                  d == m_fieldDefaults.end() ? QVariant() : d->second);
            if (v.isValid()) kv.second(v);
        }
    }

    void clearSelection() {
        m_currentId.clear();
        clearLayout();
        m_topLayout->addWidget(new QLabel("请选择组件"));
    }

    void setRootResizeCallback(std::function<void(int, int)> cb) { m_onRootSize = std::move(cb); }
    void setStyleValueProvider(std::function<QVariant(const std::string&, const std::string&)> cb) {
        m_styleValue = std::move(cb);
    }
    void setComponentResolver(std::function<const LuiDocument*(const std::string&)> cb) {
        m_componentResolver = std::move(cb);
    }

    void showRoot(const std::string& rootId, int w, int h) {
        clearLayout();
        m_currentId = rootId;

        auto* group = new QGroupBox("根");
        auto* layout = new QFormLayout(group);
        layout->setContentsMargins(6, 6, 6, 6);

        auto* typeField = new QLineEdit("LScreen");
        typeField->setReadOnly(true);
        layout->addRow("类型", typeField);

        auto* wField = new QLineEdit(QString::number(w));
        auto* hField = new QLineEdit(QString::number(h));
        auto apply = [this, wField, hField]() {
            if (!m_onRootSize) return;
            m_onRootSize(wField->text().toInt(), hField->text().toInt());
        };
        connect(wField, &QLineEdit::editingFinished, apply);
        connect(hField, &QLineEdit::editingFinished, apply);
        layout->addRow("宽度", wField);
        layout->addRow("高度", hField);

        m_topLayout->addWidget(group);
        m_topLayout->addStretch();
    }

signals:
    void documentModified();

private:
    /* 实例的属性面板：只有"对象"（位置尺寸）+ 被引用组件导出的参数 */
    std::vector<PropGroup> instanceGroups(WidgetNode* node) {
        std::vector<PropGroup> out;
        for (auto& g : ComponentSchema::propsFor("LButton")) {
            if (g.groupName == "对象") out.push_back(g);
        }
        const LuiDocument* cdoc = m_componentResolver ? m_componentResolver(node->type.substr(1)) : nullptr;
        if (!cdoc || cdoc->exports().empty()) return out;

        PropGroup params;
        params.groupName = "组件参数";
        for (auto& kv : cdoc->exports()) {
            std::string path = kv.second;
            auto dot = path.find('.');
            if (dot == std::string::npos) continue;
            std::string targetId = path.substr(0, dot);
            std::string propName = path.substr(dot + 1);
            if (targetId.rfind("m_", 0) == 0) targetId = targetId.substr(2);
            if (targetId == "root") continue;          // 根上的样式导出暂不在面板编辑

            const WidgetNode* target = nullptr;
            std::function<void(const std::vector<WidgetNode>&)> walk =
                [&](const std::vector<WidgetNode>& nodes) {
                    for (auto& n : nodes) {
                        if (n.id == targetId) target = &n;
                        walk(n.children);
                    }
                };
            walk(cdoc->children());
            if (!target) continue;

            PropSchema ps;
            ps.name = kv.first;
            ps.type = "int";
            ps.defaultVal = QVariant(0);
            for (auto& g : ComponentSchema::propsFor(target->type)) {
                for (auto& p : g.props) {
                    if (p.name != propName) continue;
                    ps.type = p.type;
                    auto it = target->props.find(propName);
                    ps.defaultVal = (it != target->props.end()) ? it->second : p.defaultVal;
                }
            }
            params.props.push_back(ps);
        }
        if (!params.props.empty()) out.push_back(std::move(params));
        return out;
    }

    /* 组件作者视角：每个属性行右侧一个"设为导出"按钮 */
    void addExportButtons(QFormLayout* glayout, WidgetNode* node) {
        if (!m_doc || !m_doc->isComponent()) return;
        for (int i = 0; i < glayout->rowCount(); ++i) {
            QLayoutItem* litem = glayout->itemAt(i, QFormLayout::LabelRole);
            QLayoutItem* fitem = glayout->itemAt(i, QFormLayout::FieldRole);
            if (!litem || !fitem || !fitem->widget()) continue;
            auto* lbl = qobject_cast<QLabel*>(litem->widget());
            if (!lbl) continue;
            std::string prop = lbl->text().toStdString();
            if (prop == "x" || prop == "y" || prop == "width" || prop == "height") continue;
            if (fitem->widget()->objectName() == "exportRow") continue;

            const std::string path = node->id + "." + prop;
            const std::string alias = m_doc->exportNameFor(path);
            QWidget* field = fitem->widget();
            auto* holder = new QWidget();
            holder->setObjectName("exportRow");
            auto* h = new QHBoxLayout(holder);
            h->setContentsMargins(0, 0, 0, 0);
            h->setSpacing(2);
            field->setParent(holder);
            h->addWidget(field, 1);

            auto* btn = new QToolButton(holder);
            btn->setAutoRaise(true);
            btn->setCheckable(true);
            btn->setChecked(!alias.empty());
            btn->setText(alias.empty() ? "⇪" : "★");
            btn->setToolTip(alias.empty() ? "设为导出参数"
                                          : QString("已导出为 %1（点击取消）")
                                                .arg(QString::fromStdString(alias)));
            h->addWidget(btn);
            connect(btn, &QToolButton::clicked, this, [this, prop, path, btn](bool on) {
                if (!m_doc || m_currentId.empty()) return;
                if (on) {
                    std::string a = prop;
                    int n = 2;
                    while (m_doc->exports().count(a)) a = prop + "_" + std::to_string(n++);
                    m_doc->setExport(a, path);
                } else {
                    m_doc->removeExport(m_doc->exportNameFor(path));
                }
                emit documentModified();
                showWidget(m_doc->findById(m_currentId));
            });
            glayout->setWidget(i, QFormLayout::FieldRole, holder);
        }
    }

    QVariant valueFor(WidgetNode* node, const std::string& key, const QVariant& def) {
        if (!node) return def;
        auto it = node->props.find(key);
        if (it != node->props.end()) return it->second;
        if (key == "x") return QVariant((int)node->pos().x());
        if (key == "y") return QVariant((int)node->pos().y());
        if (key == "width") return QVariant((int)node->size().width());
        if (key == "height") return QVariant((int)node->size().height());
        if (m_styleValue) {
            QVariant v = m_styleValue(node->id, key);
            if (v.isValid()) return v;
        }
        return def;
    }

    static std::function<void(const QVariant&)> makeSetter(QWidget* w) {
        if (auto* cb = qobject_cast<QCheckBox*>(w)) {
            return [cb](const QVariant& v) { QSignalBlocker b(cb); cb->setChecked(v.toBool()); };
        }
        if (auto* combo = qobject_cast<QComboBox*>(w)) {
            return [combo](const QVariant& v) {
                QSignalBlocker b(combo);
                int idx = combo->findData(v);
                if (idx < 0) idx = combo->findData(v.toInt());
                if (idx < 0) idx = combo->findData(v.toString());
                if (idx < 0) {
                    combo->addItem(QString("自定义 (%1)").arg(v.toString()), v);
                    idx = combo->count() - 1;
                }
                combo->setCurrentIndex(idx);
            };
        }
        if (auto* pt = qobject_cast<QPlainTextEdit*>(w)) {
            return [pt](const QVariant& v) { QSignalBlocker b(pt); pt->setPlainText(v.toString()); };
        }
        if (auto* le = qobject_cast<QLineEdit*>(w)) {
            return [le](const QVariant& v) {
                QSignalBlocker b(le);
                if (v.type() == QVariant::List) {
                    QString s;
                    auto list = v.toList();
                    for (int i = 0; i < list.size(); ++i) {
                        if (i) s += ", ";
                        s += list[i].toString();
                    }
                    le->setText(s);
                } else {
                    le->setText(v.toString());
                }
            };
        }
        /* 包了一层的行（导出按钮 / 颜色行）：继续往里找控件 */
        if (auto* cb = w->findChild<QCheckBox*>()) {
            return [cb](const QVariant& v) { QSignalBlocker b(cb); cb->setChecked(v.toBool()); };
        }
        if (auto* combo = w->findChild<QComboBox*>()) {
            return [combo](const QVariant& v) {
                QSignalBlocker b(combo);
                int idx = combo->findData(v);
                if (idx < 0) idx = combo->findData(v.toInt());
                if (idx < 0) idx = combo->findData(v.toString());
                if (idx < 0) {
                    combo->addItem(QString("自定义 (%1)").arg(v.toString()), v);
                    idx = combo->count() - 1;
                }
                combo->setCurrentIndex(idx);
            };
        }
        if (auto* pt = w->findChild<QPlainTextEdit*>()) {
            return [pt](const QVariant& v) { QSignalBlocker b(pt); pt->setPlainText(v.toString()); };
        }
        if (auto* le = w->findChild<QLineEdit*>()) {
            QPushButton* swatch = w->findChild<QPushButton*>();
            return [le, swatch](const QVariant& v) {
                QSignalBlocker b(le);
                QString s = v.toString();
                le->setText(s);
                if (swatch) {
                    QString hex = s;
                    hex.remove("0x").remove("0X");
                    QColor c(hex.toInt(nullptr, 16));
                    if (!c.isValid()) c = QColor(0, 0, 0);
                    swatch->setStyleSheet(QString("background-color: %1; border: 1px solid #555; "
                                                  "border-radius: 2px;").arg(c.name()));
                }
            };
        }
        return nullptr;
    }

    void registerFields(QFormLayout* glayout) {
        for (int i = 0; i < glayout->rowCount(); ++i) {
            QLayoutItem* litem = glayout->itemAt(i, QFormLayout::LabelRole);
            QLayoutItem* fitem = glayout->itemAt(i, QFormLayout::FieldRole);
            if (!litem || !fitem) continue;
            auto* lbl = qobject_cast<QLabel*>(litem->widget());
            if (!lbl || !fitem->widget()) continue;
            auto setter = makeSetter(fitem->widget());
            if (setter) m_fieldSetters[lbl->text().toStdString()] = std::move(setter);
        }
    }

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
        m_fieldSetters.clear();
        m_fieldDefaults.clear();
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
