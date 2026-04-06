#pragma once
#include <QGraphicsScene>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QUndoStack>
#include <QTimer>
#include "model/LuiDocument.hpp"

namespace designer {

class WidgetItem;

class DesignerScene : public QGraphicsScene {
    Q_OBJECT

    LuiDocument* m_doc = nullptr;
    QUndoStack* m_undoStack = nullptr;
    WidgetItem* m_selectedItem = nullptr;
    QColor m_canvasBg;
    bool m_showGrid = true;
    int m_gridSize = 10;
    bool m_rebuildPending = false;

public:
    explicit DesignerScene(QObject* parent = nullptr);

    void setDocument(LuiDocument* doc);
    LuiDocument* document() const { return m_doc; }

    void setUndoStack(QUndoStack* stack) { m_undoStack = stack; }
    QUndoStack* undoStack() const { return m_undoStack; }

    void rebuild();
    void scheduleRebuild();
    void addWidgetFromDrop(const std::string& type, QPointF scenePos);
    void deleteSelected();
    void selectItem(WidgetItem* item);
    WidgetItem* selectedItem() const { return m_selectedItem; }

    void commitMoveResize(const std::string& nodeId,
                          QPointF oldPos, QPointF newPos,
                          QSizeF oldSize, QSizeF newSize);

    void setShowGrid(bool show) { m_showGrid = show; update(); }
    void setGridSize(int size) { m_gridSize = size; update(); }
    QPointF snapToGrid(QPointF pos) const;

    void emitWidgetsSelected();

signals:
    void widgetsSelected(const QList<WidgetItem*>& items);
    void documentModified();

protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;
    void dropEvent(QGraphicsSceneDragDropEvent* event) override;

private:
    void assignSceneRecursive(WidgetItem* item);
};

}
