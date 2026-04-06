#include "canvas/DesignerScene.hpp"
#include "canvas/WidgetItem.hpp"
#include "canvas/items/ItemFactory.hpp"
#include "commands/UndoCommands.hpp"
#include <QPainter>
#include <cmath>

namespace designer {

namespace {

QStringList collectSelectedIds(const QList<QGraphicsItem*>& items) {
    QStringList ids;
    for (auto* gi : items) {
        auto* wi = dynamic_cast<WidgetItem*>(gi);
        if (wi) ids.append(QString::fromStdString(wi->nodeId()));
    }
    return ids;
}

}

DesignerScene::DesignerScene(QObject* parent)
    : QGraphicsScene(parent), m_canvasBg(Qt::white) {
    connect(this, &QGraphicsScene::selectionChanged, this, &DesignerScene::emitWidgetsSelected);
}

void DesignerScene::setDocument(LuiDocument* doc) {
    m_doc = doc;
    rebuild();
}

void DesignerScene::rebuild() {
    const QStringList selectedIds = collectSelectedIds(selectedItems());

    blockSignals(true);
    m_selectedItem = nullptr;
    m_rebuildPending = false;

    QList<QGraphicsItem*> allItems = items();
    for (auto* gi : allItems) {
        removeItem(gi);
        delete gi;
    }

    if (!m_doc) {
        blockSignals(false);
        return;
    }

    setSceneRect(0, 0, m_doc->width(), m_doc->height());

    for (auto& node : m_doc->children()) {
        auto* item = ItemFactory::create(node);
        if (item) {
            assignSceneRecursive(item);
            addItem(item);
        }
    }

    for (auto* gi : items()) {
        auto* wi = dynamic_cast<WidgetItem*>(gi);
        if (wi && selectedIds.contains(QString::fromStdString(wi->nodeId()))) {
            wi->setSelected(true);
        }
    }

    blockSignals(false);
    emitWidgetsSelected();
}

void DesignerScene::scheduleRebuild() {
    if (m_rebuildPending) return;
    m_rebuildPending = true;
    QTimer::singleShot(0, this, &DesignerScene::rebuild);
}

void DesignerScene::addWidgetFromDrop(const std::string& type, QPointF scenePos) {
    if (!m_doc) return;

    WidgetNode node;
    node.type = type;
    std::string prefix = type;
    if (prefix.size() > 1 && prefix[0] == 'L')
        prefix = prefix.substr(1);
    for (auto& c : prefix) c = std::tolower(c);
    node.id = prefix + "_" + std::to_string(m_doc->nextAutoId());

    QPointF snapped = snapToGrid(scenePos);
    node.initDefaultProps();

    auto* hitItem = dynamic_cast<WidgetItem*>(itemAt(scenePos, QTransform()));
    if (hitItem && hitItem->isContainer()) {
        QPointF localPos = snapped - hitItem->pos();
        node.setPos(localPos);

        auto* parentNode = m_doc->findById(hitItem->nodeId());
        if (parentNode) {
            parentNode->children.push_back(std::move(node));
            m_doc->markModified();
            scheduleRebuild();
            emit documentModified();
            return;
        }
    }

    node.setPos(snapped);

    if (m_undoStack) {
        m_undoStack->push(new AddWidgetCommand(this, std::move(node)));
    } else {
        m_doc->addChild(std::move(node));
        m_doc->markModified();
        scheduleRebuild();
    }

    emit documentModified();
}

void DesignerScene::deleteSelected() {
    if (!m_selectedItem || !m_doc) return;

    std::string id = m_selectedItem->nodeId();
    m_selectedItem = nullptr;
    clearSelection();

    if (m_undoStack) {
        m_undoStack->push(new DeleteWidgetCommand(this, id));
    } else {
        m_doc->removeById(id);
    }

    scheduleRebuild();
    emit documentModified();
}

void DesignerScene::selectItem(WidgetItem* item) {
    clearSelection();
    m_selectedItem = item;
    if (m_selectedItem) m_selectedItem->setSelected(true);
}

void DesignerScene::commitMoveResize(const std::string& nodeId,
                                      QPointF oldPos, QPointF newPos,
                                      QSizeF oldSize, QSizeF newSize) {
    if (!m_doc) return;

    if (m_undoStack) {
        m_undoStack->push(new MoveResizeCommand(this, nodeId,
                                                 oldPos, newPos, oldSize, newSize));
    } else {
        auto* node = m_doc->findById(nodeId);
        if (node) {
            node->setPos(newPos);
            node->setSize(newSize);
            m_doc->markModified();
        }
        emit documentModified();
    }
}

QPointF DesignerScene::snapToGrid(QPointF pos) const {
    if (!m_showGrid || m_gridSize <= 1) return pos;
    int x = qRound(pos.x() / m_gridSize) * m_gridSize;
    int y = qRound(pos.y() / m_gridSize) * m_gridSize;
    return QPointF((double)x, (double)y);
}

void DesignerScene::drawBackground(QPainter* painter, const QRectF& rect) {
    painter->fillRect(rect, m_canvasBg);

    if (m_showGrid && m_doc) {
        painter->setPen(QPen(QColor(60, 60, 60), 0.5));
        double left = rect.left() - fmod(rect.left(), m_gridSize);
        double top = rect.top() - fmod(rect.top(), m_gridSize);

        for (double x = left; x < rect.right(); x += m_gridSize)
            painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
        for (double y = top; y < rect.bottom(); y += m_gridSize)
            painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));
    }
}

void DesignerScene::dragEnterEvent(QGraphicsSceneDragDropEvent* event) {
    if (event->mimeData()->hasFormat("application/x-lvgl-widget"))
        event->acceptProposedAction();
    else
        event->ignore();
}

void DesignerScene::dragMoveEvent(QGraphicsSceneDragDropEvent* event) {
    if (event->mimeData()->hasFormat("application/x-lvgl-widget"))
        event->acceptProposedAction();
    else
        event->ignore();
}

void DesignerScene::dropEvent(QGraphicsSceneDragDropEvent* event) {
    if (!event->mimeData()->hasFormat("application/x-lvgl-widget")) {
        event->ignore();
        return;
    }

    std::string type = event->mimeData()->data("application/x-lvgl-widget").toStdString();
    addWidgetFromDrop(type, event->scenePos());
    event->acceptProposedAction();
}

void DesignerScene::emitWidgetsSelected() {
    QList<WidgetItem*> result;
    for (auto* gi : selectedItems()) {
        auto* wi = dynamic_cast<WidgetItem*>(gi);
        if (wi) result.append(wi);
    }
    m_selectedItem = result.isEmpty() ? nullptr : result.first();
    emit widgetsSelected(result);
}

void DesignerScene::assignSceneRecursive(WidgetItem* item) {
    if (!item) return;
    item->setScene(this);

    const auto childItems = item->childItems();
    for (auto* child : childItems) {
        auto* widgetChild = dynamic_cast<WidgetItem*>(child);
        if (widgetChild) assignSceneRecursive(widgetChild);
    }
}

}
