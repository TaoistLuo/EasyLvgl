#include "canvas/SDL2Widget.hpp"
#include <QShowEvent>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMimeData>
#include <QPainter>
#include <QMetaObject>
#include <cstdlib>
#include <cstring>

#ifndef ALIGNED_ALLOC
#define ALIGNED_ALLOC
static void* aligned_alloc_wrapper(size_t alignment, size_t size) {
    void* ptr = nullptr;
    posix_memalign(&ptr, alignment, size);
    return ptr;
}
#define aligned_alloc aligned_alloc_wrapper
#endif

namespace designer {

static constexpr int PAN_MARGIN = 60;
static constexpr double ZOOM_MIN = 0.25;
static constexpr double ZOOM_MAX = 4.0;
static constexpr double ZOOM_STEP = 1.1;
static constexpr int ROOT_HANDLE_SIZE = 12;
static constexpr int ROOT_HANDLE_HIT = 7;
static constexpr int ROOT_MIN_SIZE = 20;

static void rootHandlePoints(const QRect& r, QPoint out[8]);

SDL2Widget::SDL2Widget(QWidget* parent)
    : QWidget(parent) {
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setMinimumSize(200, 150);
    setAcceptDrops(true);
}

SDL2Widget::~SDL2Widget() {
    cleanup();
}

void SDL2Widget::setLvglSize(int w, int h) {
    m_width = w;
    m_height = h;
    m_docSizeLocked = true;
    if (m_display) {
        lv_display_set_resolution(m_display, w, h);
        /* disp_size 由首个显示创建时按 hor_res 分档并全局复用，
         * set_resolution 不会重算，需重启主题让设计尺寸生效（否则与运行时不符）。 */
        lv_theme_default_init(m_display,
                              lv_palette_main(LV_PALETTE_BLUE),
                              lv_palette_main(LV_PALETTE_RED),
                              LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
        recreateTexture();
    }
    clampPan();
    update();
}

void SDL2Widget::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    if (!m_initialized) {
        m_initialized = true;
        QTimer::singleShot(50, this, [this]() {
            initSdl();
            initLvgl();
        });
    }
}

void SDL2Widget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);

    const int newW = event->size().width();
    const int newH = event->size().height();
    if (newW < 1 || newH < 1) return;

    clampPan();
    update();

    if (m_docSizeLocked) return;

    if (newW != m_width || newH != m_height) {
        m_width = newW;
        m_height = newH;
        if (m_display) {
            lv_display_set_resolution(m_display, m_width, m_height);
            recreateTexture();
        }
    }
}

void SDL2Widget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    /* 屏幕之外的区域：深灰 + 点阵网格（类似 Qt Creator 的表单设计器）。
     * 屏幕区域由 LVGL 自己渲染，不加任何装饰，保证与运行时一致。 */
    painter.fillRect(rect(), QColor(0x2b, 0x2b, 0x2b));
    {
        const QRect canvasRect(0, 0, m_width, m_height);
        painter.setPen(QPen(QColor(0x4a, 0x4a, 0x4a), 1));
        const int step = 10;
        for (int y = 0; y < height(); y += step) {
            for (int x = 0; x < width(); x += step) {
                if (canvasRect.contains(x, y)) continue;
                painter.drawPoint(x, y);
            }
        }
    }
    if (!m_fb) return;

    const uint32_t stride = lv_draw_buf_width_to_stride(m_width, LV_COLOR_FORMAT_ARGB8888);
    QImage image(m_fb, m_width, m_height, stride, QImage::Format_ARGB32_Premultiplied);
    painter.translate(m_panX, m_panY);
    painter.scale(m_zoom, m_zoom);
    painter.drawImage(0, 0, image);

    if (!m_rootSelected) return;

    painter.resetTransform();
    const QRect r = canvasRect();
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(0x21, 0x96, 0xF3), 2));
    painter.drawRect(r);

    painter.setBrush(QColor(0xFF, 0xFF, 0xFF));
    QPoint pts[8];
    rootHandlePoints(r, pts);
    for (int i = 0; i < 8; i++) {
        painter.drawRect(QRect(pts[i].x() - ROOT_HANDLE_SIZE / 2,
                               pts[i].y() - ROOT_HANDLE_SIZE / 2,
                               ROOT_HANDLE_SIZE, ROOT_HANDLE_SIZE));
    }
}

QPointF SDL2Widget::toLvgl(const QPoint& widgetPos) const {
    QPointF p((widgetPos.x() - m_panX) / m_zoom,
              (widgetPos.y() - m_panY) / m_zoom);

    const qreal maxX = m_width - 1;
    const qreal maxY = m_height - 1;
    if (p.x() < 0) p.setX(0); else if (p.x() > maxX) p.setX(maxX);
    if (p.y() < 0) p.setY(0); else if (p.y() > maxY) p.setY(maxY);
    return p;
}

void SDL2Widget::clampPan() {
    const int cw = qRound(m_width * m_zoom);
    const int ch = qRound(m_height * m_zoom);

    if (cw <= width()) m_panX = (width() - cw) / 2;
    else m_panX = qBound(width() - cw - PAN_MARGIN, m_panX, PAN_MARGIN);

    if (ch <= height()) m_panY = (height() - ch) / 2;
    else m_panY = qBound(height() - ch - PAN_MARGIN, m_panY, PAN_MARGIN);
}

QRect SDL2Widget::canvasRect() const {
    return QRect(m_panX, m_panY, qRound(m_width * m_zoom), qRound(m_height * m_zoom));
}

static void rootHandlePoints(const QRect& r, QPoint out[8]) {
    out[0] = {r.left(), r.top()};
    out[1] = {r.center().x(), r.top()};
    out[2] = {r.right(), r.top()};
    out[3] = {r.left(), r.center().y()};
    out[4] = {r.right(), r.center().y()};
    out[5] = {r.left(), r.bottom()};
    out[6] = {r.center().x(), r.bottom()};
    out[7] = {r.right(), r.bottom()};
}

int SDL2Widget::rootHandleAt(const QPoint& widgetPos) const {
    QPoint pts[8];
    rootHandlePoints(canvasRect(), pts);
    const int tol = ROOT_HANDLE_SIZE / 2 + ROOT_HANDLE_HIT;
    for (int i = 0; i < 8; i++) {
        if (qAbs(widgetPos.x() - pts[i].x()) <= tol && qAbs(widgetPos.y() - pts[i].y()) <= tol) return i;
    }
    return -1;
}

void SDL2Widget::setRootSelected(bool on) {
    if (m_rootSelected == on) return;
    m_rootSelected = on;
    update();
}

void SDL2Widget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_rootSelected) {
        const int edge = rootHandleAt(event->pos());
        if (edge >= 0) {
            m_rootDragEdge = edge;
            m_rootDragStart = event->pos();
            m_rootStartW = m_width;
            m_rootStartH = m_height;
            m_mousePressed = false;
            QWidget::mousePressEvent(event);
            return;
        }
    }

    if (event->button() == Qt::MiddleButton) {
        m_panning = true;
        m_panStart = event->pos();
        m_panStartX = m_panX;
        m_panStartY = m_panY;
        setCursor(Qt::ClosedHandCursor);
    } else if (event->button() == Qt::LeftButton) {
        if (!canvasRect().contains(event->pos())) {
            m_mousePressed = false;
            QWidget::mousePressEvent(event);
            return;
        }
        const QPointF p = toLvgl(event->pos());
        m_mouseX = qRound(p.x());
        m_mouseY = qRound(p.y());
        m_mousePressed = true;
        emit widgetClicked(m_mouseX, m_mouseY, event->modifiers());
    } else if (event->button() == Qt::RightButton) {
        const QPointF p = toLvgl(event->pos());
        m_mouseX = qRound(p.x());
        m_mouseY = qRound(p.y());
        m_mousePressed = false;
    }
    QWidget::mousePressEvent(event);
}

void SDL2Widget::mouseReleaseEvent(QMouseEvent* event) {
    if (m_rootDragEdge >= 0 && event->button() == Qt::LeftButton) {
        m_rootDragEdge = -1;
        emit rootResized(m_width, m_height);
        QWidget::mouseReleaseEvent(event);
        return;
    }

    if (event->button() == Qt::MiddleButton) {
        m_panning = false;
        unsetCursor();
    } else if (event->button() == Qt::LeftButton) {
        const QPointF p = toLvgl(event->pos());
        m_mouseX = qRound(p.x());
        m_mouseY = qRound(p.y());
        m_mousePressed = false;
    }
    QWidget::mouseReleaseEvent(event);
}

void SDL2Widget::mouseMoveEvent(QMouseEvent* event) {
    if (m_rootDragEdge >= 0) {
        const int dx = qRound((event->x() - m_rootDragStart.x()) / m_zoom);
        const int dy = qRound((event->y() - m_rootDragStart.y()) / m_zoom);
        const int e = m_rootDragEdge;

        int nw = m_rootStartW;
        int nh = m_rootStartH;
        if (e == 0 || e == 3 || e == 5) nw = m_rootStartW - dx;
        else if (e == 2 || e == 4 || e == 7) nw = m_rootStartW + dx;
        if (e == 0 || e == 1 || e == 2) nh = m_rootStartH - dy;
        else if (e == 5 || e == 6 || e == 7) nh = m_rootStartH + dy;
        nw = qMax(ROOT_MIN_SIZE, nw);
        nh = qMax(ROOT_MIN_SIZE, nh);

        if (nw != m_width || nh != m_height) {
            m_width = nw;
            m_height = nh;
            if (m_display) {
                lv_display_set_resolution(m_display, nw, nh);
                recreateTexture();
            }
            clampPan();
            update();
        }
        QWidget::mouseMoveEvent(event);
        return;
    }

    if (m_panning) {
        m_panX = m_panStartX + (event->x() - m_panStart.x());
        m_panY = m_panStartY + (event->y() - m_panStart.y());
        clampPan();
        update();
        QWidget::mouseMoveEvent(event);
        return;
    }

    const QPointF p = toLvgl(event->pos());
    m_mouseX = qRound(p.x());
    m_mouseY = qRound(p.y());
    QWidget::mouseMoveEvent(event);
}

void SDL2Widget::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Control) {
        emit ctrlStateChanged(true);
    }
    m_lastKey = event->key();
    m_keyPressed = true;
    QWidget::keyPressEvent(event);
}

void SDL2Widget::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Control) {
        emit ctrlStateChanged(false);
    }
    m_keyPressed = false;
    QWidget::keyReleaseEvent(event);
}

void SDL2Widget::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasFormat("application/x-lvgl-widget")) {
        event->acceptProposedAction();
    }
}

void SDL2Widget::dragMoveEvent(QDragMoveEvent* event) {
    if (event->mimeData()->hasFormat("application/x-lvgl-widget")) {
        event->acceptProposedAction();
    }
}

void SDL2Widget::dropEvent(QDropEvent* event) {
    if (event->mimeData()->hasFormat("application/x-lvgl-widget")) {
        const QByteArray data = event->mimeData()->data("application/x-lvgl-widget");
        const QPointF p = toLvgl(event->pos());
        emit widgetDropped(QString::fromUtf8(data), qRound(p.x()), qRound(p.y()));
        event->acceptProposedAction();
    }
}

void SDL2Widget::wheelEvent(QWheelEvent* event) {
    if (event->modifiers() & Qt::ControlModifier) {
        const double factor = event->angleDelta().y() > 0 ? ZOOM_STEP : 1.0 / ZOOM_STEP;
        const double newZoom = qBound(ZOOM_MIN, m_zoom * factor, ZOOM_MAX);
        if (newZoom != m_zoom) {
            const QPointF anchor = toLvgl(event->pos());
            m_zoom = newZoom;
            m_panX = event->x() - qRound(anchor.x() * m_zoom);
            m_panY = event->y() - qRound(anchor.y() * m_zoom);
            clampPan();
            update();
        }
        event->accept();
        return;
    }

    if (m_mousePressed) return;
    const QPointF p = toLvgl(event->pos());
    emit wheelScrolled(event->angleDelta().y(), qRound(p.x()), qRound(p.y()));
    QWidget::wheelEvent(event);
}

void SDL2Widget::initSdl() {
    // LVGL is rendered into m_fb and painted by Qt in paintEvent().
    // Do not attach SDL to the Qt native window here: Qt backing-store and SDL/Xcb
    // window ownership conflict and can crash in QBackingStore on repaint.
}

void SDL2Widget::initLvgl() {
    if (!lv_is_initialized()) {
        lv_init();
    }

    m_display = lv_display_create(m_width, m_height);
    lv_display_set_color_format(m_display, LV_COLOR_FORMAT_ARGB8888);
    lv_display_set_user_data(m_display, this);
    lv_display_set_flush_cb(m_display, flushCb);

    const uint32_t stride = lv_draw_buf_width_to_stride(m_width, LV_COLOR_FORMAT_ARGB8888);
    const uint32_t bufSize = stride * m_height;
    m_fb = static_cast<uint8_t*>(aligned_alloc(64, bufSize));
    if (m_fb) memset(m_fb, 0xFF, bufSize);
    lv_display_set_buffers(m_display, m_fb, nullptr, bufSize, LV_DISPLAY_RENDER_MODE_DIRECT);

    m_mouseIndev = lv_indev_create();
    lv_indev_set_type(m_mouseIndev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(m_mouseIndev, mouseReadCb);
    lv_indev_set_user_data(m_mouseIndev, this);

    m_keyIndev = lv_indev_create();
    lv_indev_set_type(m_keyIndev, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(m_keyIndev, keyReadCb);
    lv_indev_set_user_data(m_keyIndev, this);

    m_elapsed.start();
    m_tickTimer = new QTimer(this);
    connect(m_tickTimer, &QTimer::timeout, this, [this]() {
        lv_tick_inc(5);
        lv_timer_handler();
    });
    m_tickTimer->start(5);

    emit lvglReady();
}

void SDL2Widget::recreateTexture() {
    if (m_fb) {
        free(m_fb);
        m_fb = nullptr;
    }

    const uint32_t stride = lv_draw_buf_width_to_stride(m_width, LV_COLOR_FORMAT_ARGB8888);
    const uint32_t bufSize = stride * m_height;
    m_fb = static_cast<uint8_t*>(aligned_alloc(64, bufSize));
    if (m_fb) memset(m_fb, 0xFF, bufSize);

    if (m_display && m_fb) {
        lv_display_set_buffers(m_display, m_fb, nullptr, bufSize, LV_DISPLAY_RENDER_MODE_DIRECT);
    }

}

void SDL2Widget::cleanup() {
    if (m_tickTimer) {
        m_tickTimer->stop();
        delete m_tickTimer;
        m_tickTimer = nullptr;
    }

    if (m_mouseIndev) {
        lv_indev_delete(m_mouseIndev);
        m_mouseIndev = nullptr;
    }
    if (m_keyIndev) {
        lv_indev_delete(m_keyIndev);
        m_keyIndev = nullptr;
    }
    if (m_display) {
        lv_display_delete(m_display);
        m_display = nullptr;
    }

    if (m_fb) {
        free(m_fb);
        m_fb = nullptr;
    }
}

void SDL2Widget::flushCb(lv_display_t* disp, const lv_area_t*, uint8_t*) {
    auto* self = static_cast<SDL2Widget*>(lv_display_get_user_data(disp));
    if (!self) {
        lv_display_flush_ready(disp);
        return;
    }

    if (lv_display_flush_is_last(disp)) {
        QMetaObject::invokeMethod(self, "update", Qt::QueuedConnection);
    }
    lv_display_flush_ready(disp);
}

void SDL2Widget::mouseReadCb(lv_indev_t* indev, lv_indev_data_t* data) {
    auto* self = static_cast<SDL2Widget*>(lv_indev_get_user_data(indev));
    if (!self) return;
    data->point.x = self->m_mouseX;
    data->point.y = self->m_mouseY;
    data->state = self->m_mousePressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

void SDL2Widget::keyReadCb(lv_indev_t* indev, lv_indev_data_t* data) {
    auto* self = static_cast<SDL2Widget*>(lv_indev_get_user_data(indev));
    if (!self) return;
    data->key = self->m_lastKey;
    data->state = self->m_keyPressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

}
