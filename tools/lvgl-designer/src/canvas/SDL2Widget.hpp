#pragma once

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QDragEnterEvent>
#include <QWheelEvent>
#include <lvgl.h>
#include <string>
#include <functional>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

namespace designer {

class SDL2Widget : public QWidget {
    Q_OBJECT

public:
    explicit SDL2Widget(QWidget* parent = nullptr);
    ~SDL2Widget();

    lv_display_t* display() const { return m_display; }
    void setLvglSize(int w, int h);
    void setRootSelected(bool on);

signals:
    void lvglReady();
    void rootResized(int w, int h);
    void widgetDropped(const QString& widgetType, int x, int y);
    void wheelScrolled(int delta, int x, int y);
    void widgetClicked(int x, int y, Qt::KeyboardModifiers modifiers);
    void ctrlStateChanged(bool ctrl);

protected:
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void initSdl();
    void initLvgl();
    void recreateTexture();
    void cleanup();
    QPointF toLvgl(const QPoint& widgetPos) const;
    void clampPan();
    QRect canvasRect() const;
    int rootHandleAt(const QPoint& widgetPos) const;

    static void flushCb(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map);
    static void mouseReadCb(lv_indev_t* indev, lv_indev_data_t* data);
    static void keyReadCb(lv_indev_t* indev, lv_indev_data_t* data);

    SDL_Window* m_sdlWindow = nullptr;
    SDL_Renderer* m_sdlRenderer = nullptr;
    SDL_Texture* m_sdlTexture = nullptr;

    lv_display_t* m_display = nullptr;
    lv_indev_t* m_mouseIndev = nullptr;
    lv_indev_t* m_keyIndev = nullptr;

    uint8_t* m_fb = nullptr;
    int m_width = 800;
    int m_height = 480;
    bool m_docSizeLocked = false;

    int32_t m_mouseX = 0;
    int32_t m_mouseY = 0;
    bool m_mousePressed = false;
    uint32_t m_lastKey = 0;
    bool m_keyPressed = false;

    double m_zoom = 1.0;
    int m_panX = 0;
    int m_panY = 0;
    bool m_panning = false;
    QPoint m_panStart;
    int m_panStartX = 0;
    int m_panStartY = 0;

    bool m_rootSelected = false;
    int m_rootDragEdge = -1;
    QPoint m_rootDragStart;
    int m_rootStartW = 0;
    int m_rootStartH = 0;

    QTimer* m_tickTimer = nullptr;
    QElapsedTimer m_elapsed;
    bool m_initialized = false;
};

}
