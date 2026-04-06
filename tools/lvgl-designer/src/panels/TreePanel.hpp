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
        setHeaderLabels({"ID", "Type"});
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
