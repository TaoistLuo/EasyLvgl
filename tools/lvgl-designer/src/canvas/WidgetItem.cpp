/**************************************************************************************************
 * @file        WidgetItem.cpp
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

#include "canvas/WidgetItem.hpp"
#include "canvas/DesignerScene.hpp"
#include "commands/UndoCommands.hpp"
#include <QGraphicsSceneHoverEvent>
#include <QStyleOptionGraphicsItem>

namespace designer {

WidgetItem::WidgetItem(const std::string& nodeId, const std::string& type,
                       QPointF pos, QSizeF size, QGraphicsItem* parent)
    : QGraphicsRectItem(parent), m_nodeId(nodeId), m_type(type) {
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
    setPos(pos);
    setRect(0, 0, size.width(), size.height());
}

QString WidgetItem::displayName() const {
    QString text = propStr("text");
    if (!text.isEmpty()) return text;
    return QString::fromStdString(m_type + " (" + m_nodeId + ")");
}

void WidgetItem::applyObjectProps() {
    setVisible(!propBool("hidden", false));
    int opa = propInt("opa", 255);
    setOpacity(opa / 255.0);
}

void WidgetItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    QRectF body = rect();

    int bgOpa = propInt("bg_opa", 0);
    if (bgOpa > 0) {
        QColor bg = propColor("bg_color", QColor(0, 0, 0));
        bg.setAlpha(bgOpa);
        int r = propInt("radius", 0);
        painter->setBrush(bg);
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(body, r, r);
    }

    paintWidget(painter, body);

    int bw = propInt("border_width", 0);
    if (bw > 0) {
        QColor bc = propColor("border_color", QColor(0, 0, 0));
        int bopa = propInt("border_opa", 255);
        bc.setAlpha(bopa);
        int r = propInt("radius", 0);
        painter->setPen(QPen(bc, bw));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(body.adjusted(bw/2.0, bw/2.0, -bw/2.0, -bw/2.0), r, r);
    }

    int ow = propInt("outline_width", 0);
    if (ow > 0) {
        QColor oc = propColor("outline_color", QColor(0, 0, 0));
        int opad = propInt("outline_pad", 0);
        painter->setPen(QPen(oc, ow));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(body.adjusted(-opad, -opad, opad, opad),
                                 propInt("radius", 0), propInt("radius", 0));
    }

    int sw = propInt("shadow_width", 0);
    if (sw > 0) {
        QColor sc = propColor("shadow_color", QColor(0, 0, 0));
        sc.setAlpha(propInt("shadow_opa", 255));
        int sox = propInt("shadow_ofs_x", 0);
        int soy = propInt("shadow_ofs_y", 0);
        painter->setPen(Qt::NoPen);
        painter->setBrush(sc);
        painter->drawRoundedRect(body.adjusted(sox-sw, soy-sw, sox+sw, soy+sw),
                                 propInt("radius", 0), propInt("radius", 0));
    }

    if (isSelected()) {
        painter->setPen(QPen(QColor(0x56, 0x9c, 0xd6), 2));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(body);
        drawHandles(painter);
    }
}

void WidgetItem::drawHandles(QPainter* painter) {
    painter->setBrush(QColor(0x56, 0x9c, 0xd6));
    painter->setPen(Qt::NoPen);
    QRectF r = rect();
    double hs = HANDLE_SIZE;
    auto drawH = [&](double x, double y) {
        painter->drawRect(QRectF(x - hs/2, y - hs/2, hs, hs));
    };
    drawH(r.left(), r.top());
    drawH(r.center().x(), r.top());
    drawH(r.right(), r.top());
    drawH(r.right(), r.center().y());
    drawH(r.right(), r.bottom());
    drawH(r.center().x(), r.bottom());
    drawH(r.left(), r.bottom());
    drawH(r.left(), r.center().y());
}

ResizeHandle WidgetItem::handleAt(QPointF p) const {
    if (!isSelected()) return ResizeHandle::None;
    QRectF r = rect();
    double hs = HANDLE_SIZE + 2;
    auto near = [&](double x, double y) {
        return QRectF(x - hs/2, y - hs/2, hs, hs).contains(p);
    };
    if (near(r.left(), r.top())) return ResizeHandle::TopLeft;
    if (near(r.center().x(), r.top())) return ResizeHandle::Top;
    if (near(r.right(), r.top())) return ResizeHandle::TopRight;
    if (near(r.right(), r.center().y())) return ResizeHandle::Right;
    if (near(r.right(), r.bottom())) return ResizeHandle::BottomRight;
    if (near(r.center().x(), r.bottom())) return ResizeHandle::Bottom;
    if (near(r.left(), r.bottom())) return ResizeHandle::BottomLeft;
    if (near(r.left(), r.center().y())) return ResizeHandle::Left;
    return ResizeHandle::None;
}

QCursor WidgetItem::cursorForHandle(ResizeHandle h) const {
    switch (h) {
        case ResizeHandle::TopLeft: case ResizeHandle::BottomRight: return Qt::SizeFDiagCursor;
        case ResizeHandle::TopRight: case ResizeHandle::BottomLeft: return Qt::SizeBDiagCursor;
        case ResizeHandle::Top: case ResizeHandle::Bottom: return Qt::SizeVerCursor;
        case ResizeHandle::Left: case ResizeHandle::Right: return Qt::SizeHorCursor;
        default: return Qt::ArrowCursor;
    }
}

void WidgetItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    m_activeHandle = handleAt(event->pos());
    m_dragStart = event->scenePos();
    m_dragStartRect = QRectF(pos(), rect().size());
    if (m_activeHandle != ResizeHandle::None) {
        m_resizing = true;
        event->accept();
    } else {
        m_resizing = false;
        QGraphicsRectItem::mousePressEvent(event);
    }
}

void WidgetItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    QPointF delta = event->scenePos() - m_dragStart;

    if (m_resizing) {
        QRectF r = m_dragStartRect;
        switch (m_activeHandle) {
            case ResizeHandle::Right:       r.setWidth(r.width() + delta.x()); break;
            case ResizeHandle::Bottom:      r.setHeight(r.height() + delta.y()); break;
            case ResizeHandle::BottomRight: r.setWidth(r.width() + delta.x()); r.setHeight(r.height() + delta.y()); break;
            case ResizeHandle::Left:        r.setLeft(r.left() + delta.x()); break;
            case ResizeHandle::Top:         r.setTop(r.top() + delta.y()); break;
            case ResizeHandle::TopLeft:     r.setTopLeft(r.topLeft() + delta); break;
            case ResizeHandle::TopRight:    r.setTop(r.top() + delta.y()); r.setWidth(r.width() + delta.x()); break;
            case ResizeHandle::BottomLeft:  r.setLeft(r.left() + delta.x()); r.setHeight(r.height() + delta.y()); break;
            default: break;
        }
        if (r.width() < 10) r.setWidth(10);
        if (r.height() < 10) r.setHeight(10);
        setPos(r.topLeft());
        setRect(0, 0, r.width(), r.height());
        event->accept();
    } else {
        QPointF newPos = m_dragStartRect.topLeft() + delta;
        if (m_scene) newPos = m_scene->snapToGrid(newPos);
        setPos(newPos);
        event->accept();
    }
}

void WidgetItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    bool moved = (pos() != m_dragStartRect.topLeft());
    bool resized = (rect().size() != m_dragStartRect.size());

    if (moved || resized) {
        if (m_scene) {
            m_scene->commitMoveResize(m_nodeId,
                m_dragStartRect.topLeft(), pos(),
                m_dragStartRect.size(), rect().size());
        }
    }
    m_resizing = false;
    m_activeHandle = ResizeHandle::None;
    QGraphicsRectItem::mouseReleaseEvent(event);
}

void WidgetItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
    auto h = handleAt(event->pos());
    if (h != ResizeHandle::None)
        setCursor(cursorForHandle(h));
    else
        setCursor(Qt::SizeAllCursor);
}

}
