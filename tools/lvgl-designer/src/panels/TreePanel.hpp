#pragma once
#include <QTreeWidget>
#include "model/LuiDocument.hpp"

namespace designer {

class TreePanel : public QTreeWidget {
    Q_OBJECT
    LuiDocument* m_doc = nullptr;

public:
    explicit TreePanel(QWidget* parent = nullptr) : QTreeWidget(parent) {
        setHeaderLabels({"ID", "Type"});
        setColumnWidth(0, 120);
        connect(this, &QTreeWidget::itemClicked, this, &TreePanel::onItemClicked);
    }

    void setDocument(LuiDocument* doc) {
        m_doc = doc;
        rebuild();
    }

    void rebuild() {
        clear();
        if (!m_doc) return;
        for (auto& child : m_doc->children()) {
            addNodeItem(child, nullptr);
        }
        expandAll();
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
    }

    void onItemClicked(QTreeWidgetItem* item, int) {
        if (!item) return;
        std::string id = item->data(0, Qt::UserRole).toString().toStdString();
        emit widgetSelected(id);
    }
};

}
