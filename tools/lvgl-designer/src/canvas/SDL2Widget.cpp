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
    if (m_display) {
        lv_display_set_resolution(m_display, w, h);
        recreateTexture();
        update();
    }
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
    if (!m_fb) return;

    const uint32_t stride = lv_draw_buf_width_to_stride(m_width, LV_COLOR_FORMAT_ARGB8888);
    QImage image(m_fb, m_width, m_height, stride, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0xb8, 0xb8, 0xb8));
    painter.drawImage(0, 0, image);
}

void SDL2Widget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_mouseX = event->x();
        m_mouseY = event->y();
        m_mousePressed = true;
        emit widgetClicked(m_mouseX, m_mouseY, event->modifiers());
    } else if (event->button() == Qt::RightButton) {
        m_mouseX = event->x();
        m_mouseY = event->y();
        m_mousePressed = false;
    }
    QWidget::mousePressEvent(event);
}

void SDL2Widget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_mouseX = event->x();
        m_mouseY = event->y();
        m_mousePressed = false;
    }
    QWidget::mouseReleaseEvent(event);
}

void SDL2Widget::mouseMoveEvent(QMouseEvent* event) {
    m_mouseX = event->x();
    m_mouseY = event->y();
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
        emit widgetDropped(QString::fromUtf8(data), event->pos().x(), event->pos().y());
        event->acceptProposedAction();
    }
}

void SDL2Widget::wheelEvent(QWheelEvent* event) {
    if (m_mousePressed) return;
    emit wheelScrolled(event->angleDelta().y(), event->x(), event->y());
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
