#include <QApplication>
#include <QMouseEvent>
#include <cstdio>
#include "canvas/SDL2Widget.hpp"

class Probe : public designer::SDL2Widget {
public:
    using designer::SDL2Widget::SDL2Widget;

    void press(int x, int y) {
        QMouseEvent e(QEvent::MouseButtonPress, QPointF(x, y), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        mousePressEvent(&e);
    }
    void move(int x, int y) {
        QMouseEvent e(QEvent::MouseMove, QPointF(x, y), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
        mouseMoveEvent(&e);
    }
    void release(int x, int y) {
        QMouseEvent e(QEvent::MouseButtonRelease, QPointF(x, y), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
        mouseReleaseEvent(&e);
    }
    void selectRoot(bool on) { setRootSelected(on); }
};

static int fails = 0;
static void check(bool c, const char* name) {
    printf("%s %s\n", c ? "  ok  " : "  FAIL", name);
    if (!c) fails++;
}

struct Fixture {
    Probe w;
    int gotW = -1, gotH = -1;

    Fixture(int vw, int vh, int dw, int dh) {
        w.resize(vw, vh);
        w.setLvglSize(dw, dh);
        w.selectRoot(true);
        QObject::connect(&w, &designer::SDL2Widget::rootResized, &w, [this](int a, int b) {
            gotW = a; gotH = b;
        });
    }
};

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    Fixture f(800, 600, 400, 320);

    // 画布 400x320 在 800x600 中，若居中则原点 (200,140)，否则 (0,0)
    int ox = 200, oy = 140;
    int brX = ox + 400, brY = oy + 320;
    f.w.press(brX, brY);
    f.w.move(brX + 100, brY + 80);
    f.w.release(brX + 100, brY + 80);
    if (!(f.gotW == 500 && f.gotH == 400)) {
        ox = 0; oy = 0; brX = 400; brY = 320;
        f.gotW = f.gotH = -1;
        f.w.press(brX, brY);
        f.w.move(brX + 100, brY + 80);
        f.w.release(brX + 100, brY + 80);
    }
    printf("canvas origin = (%d,%d); BR 拖动 +100,+80 -> %dx%d\n", ox, oy, f.gotW, f.gotH);
    check(f.gotW == 500 && f.gotH == 400, "BR 手柄拖动: 根 400x320 -> 500x400 (可超出显示分辨率, 未被钳制)");

    // 画布中心不应命中手柄
    Fixture g(800, 600, 400, 320);
    g.w.press(ox + 200, oy + 160);
    g.w.move(ox + 300, oy + 240);
    g.w.release(ox + 300, oy + 240);
    check(g.gotW == -1, "画布中心拖动不触发根尺寸变更");

    // TL 手柄向上左拖 => 原点固定 => 尺寸变大
    Fixture h(800, 600, 400, 320);
    h.w.press(ox, oy);
    h.w.move(ox - 60, oy - 40);
    h.w.release(ox - 60, oy - 40);
    printf("TL 拖动 -60,-40 -> %dx%d\n", h.gotW, h.gotH);
    check(h.gotW == 460 && h.gotH == 360, "TL 手柄拖动: 根 400x320 -> 460x360");

    // MR 只改宽度
    Fixture k(800, 600, 400, 320);
    k.w.press(ox + 400, oy + 160);
    k.w.move(ox + 450, oy + 300);
    k.w.release(ox + 450, oy + 300);
    printf("MR 拖动 +50,+140 -> %dx%d\n", k.gotW, k.gotH);
    check(k.gotW == 450 && k.gotH == 320, "MR 手柄只改宽度, 高度不变");

    printf("\n%s (%d failures)\n", fails ? "FAILED" : "PASSED", fails);
    return fails ? 1 : 0;
}
