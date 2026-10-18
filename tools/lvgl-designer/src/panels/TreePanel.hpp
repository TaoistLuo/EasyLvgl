/**************************************************************************************************
 * @file        TreePanel.hpp
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
#include <QTreeWidget>
#include <QSignalBlocker>
#include "model/LuiDocument.hpp"

namespace designer {

class TreePanel : public QTreeWidget {
    Q_OBJECT
    LuiDocument* m_doc = nullptr;

public:
    explicit TreePanel(QWidget* parent = nullptr) : QTreeWidget(parent) {
        setHeaderLabels({"ID", "类型"});
        setColumnWidth(0, 120);
        connect(this, &QTreeWidget::itemClicked, this, &TreePanel::onItemClicked);
    }

    void setDocument(LuiDocument* doc) {
        m_doc = doc;
        rebuild();
    }

    void rebuild() {
        QSignalBlocker blocker(this);
        setUpdatesEnabled(false);
        while (topLevelItemCount() > 0) {
            delete takeTopLevelItem(0);
        }
        if (m_doc) {
            for (auto& child : m_doc->children()) {
                addNodeItem(child, nullptr);
            }
        }
        setUpdatesEnabled(true);
    }

    void selectById(const std::string& id) {
        QSignalBlocker blocker(this);
        auto matches = findItems(QString::fromStdString(id), Qt::MatchExactly | Qt::MatchRecursive, 0);
        if (matches.isEmpty()) {
            clearSelection();
            setCurrentItem(nullptr);
            return;
        }
        clearSelection();
        setCurrentItem(matches.first());
        matches.first()->setSelected(true);
        scrollToItem(matches.first());
    }

    void clearCurrentSelection() {
        QSignalBlocker blocker(this);
        clearSelection();
        setCurrentItem(nullptr);
    }

signals:
    void widgetSelected(const std::string& id);

private:
    void addNodeItem(const WidgetNode& node, QTreeWidgetItem* parent) {
        auto* item = parent
            ? new QTreeWidgetItem(parent)
            : new QTreeWidgetItem(this);
        item->setText(0, QString::fromStdString(node.id));
        item->setText(1, QString::fromStdString(node.type));
        item->setData(0, Qt::UserRole, QString::fromStdString(node.id));

        for (auto& child : node.children) {
            addNodeItem(child, item);
        }

        if (parent) {
            parent->setExpanded(true);
        }
    }

    void onItemClicked(QTreeWidgetItem* item, int) {
        if (!item) return;
        std::string id = item->data(0, Qt::UserRole).toString().toStdString();
        emit widgetSelected(id);
    }
};

}
