/**************************************************************************************************
 * @file        DesignerView.cpp
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

#include "canvas/DesignerView.hpp"
#include <QWheelEvent>

namespace designer {

DesignerView::DesignerView(QWidget* parent)
    : QGraphicsView(parent) {
    setRenderHint(QPainter::Antialiasing);
    setAcceptDrops(true);
    setDragMode(QGraphicsView::RubberBandDrag);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    /* 画布外围：深灰（接近 Qt Creator），让 LVGL 屏幕区域（白）自然突出 */
    setBackgroundBrush(QColor(0x2b, 0x2b, 0x2b));
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
