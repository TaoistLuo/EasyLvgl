#include "canvas/DesignerView.hpp"
#include <QWheelEvent>

namespace designer {

DesignerView::DesignerView(QWidget* parent)
    : QGraphicsView(parent) {
    setRenderHint(QPainter::Antialiasing);
    setAcceptDrops(true);
    setDragMode(QGraphicsView::RubberBandDrag);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setBackgroundBrush(QColor(0xb8, 0xb8, 0xb8));
    setFrameShape(QFrame::NoFrame);
}

void DesignerView::zoomIn() {
    m_zoomFactor *= 1.2;
    setTransform(QTransform::fromScale(m_zoomFactor, m_zoomFactor));
}

void DesignerView::zoomOut() {
    m_zoomFactor /= 1.2;
    setTransform(QTransform::fromScale(m_zoomFactor, m_zoomFactor));
}

void DesignerView::zoomReset() {
    m_zoomFactor = 1.0;
    resetTransform();
}

void DesignerView::wheelEvent(QWheelEvent* event) {
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->angleDelta().y() > 0) zoomIn();
        else zoomOut();
        event->accept();
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

}
