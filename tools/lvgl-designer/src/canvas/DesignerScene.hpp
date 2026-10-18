/**************************************************************************************************
 * @file        DesignerScene.hpp
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
