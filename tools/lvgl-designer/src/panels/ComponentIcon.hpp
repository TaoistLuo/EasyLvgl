#pragma once

#include <QColor>
#include <QFont>
#include <QIcon>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QPolygonF>

namespace designer {

inline QIcon componentIcon(const QString& type) {
    constexpr int S = 20;
    QPixmap pm(S, S);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);

    const QColor ink(0xcf, 0xcf, 0xcf);
    QPen pen(ink, 1.2);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setCapStyle(Qt::RoundCap);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);

    auto box = [&](qreal x, qreal y, qreal w, qreal h, qreal r = 2) {
        p.drawRoundedRect(QRectF(x, y, w, h), r, r);
    };
    auto line = [&](qreal x1, qreal y1, qreal x2, qreal y2) {
        p.drawLine(QPointF(x1, y1), QPointF(x2, y2));
    };
    auto fillBox = [&](qreal x, qreal y, qreal w, qreal h, qreal r = 1) {
        p.setPen(Qt::NoPen);
        p.setBrush(ink);
        p.drawRoundedRect(QRectF(x, y, w, h), r, r);
        p.setPen(pen);
        p.setBrush(Qt::NoBrush);
    };

    if (type == "LVBoxLayout") {
        box(3, 2, 14, 16);
        line(3, 7, 17, 7);
        line(3, 12, 17, 12);
    } else if (type == "LHBoxLayout") {
        box(2, 3, 16, 14);
        line(7, 3, 7, 17);
        line(12, 3, 12, 17);
    } else if (type == "LGridLayout") {
        box(2, 3, 16, 14);
        line(8, 3, 8, 17);
        line(2, 10, 18, 10);
    } else if (type == "LStackedLayout") {
        box(5, 2, 12, 12);
        line(3, 6, 3, 18);
        line(3, 18, 15, 18);
        line(3, 6, 5, 2);
    } else if (type == "LButton") {
        box(2, 5, 16, 10, 3);
        line(7, 10, 13, 10);
    } else if (type == "LImageButton") {
        box(2, 5, 16, 10, 3);
        QPolygonF mountain;
        mountain << QPointF(5, 13) << QPointF(8, 9) << QPointF(10.5, 11.5)
                 << QPointF(12.5, 9.5) << QPointF(15, 13);
        p.drawPolyline(mountain);
    } else if (type == "LCheckbox") {
        box(3, 5, 10, 10, 1);
        line(5, 10, 7.5, 12.5);
        line(7.5, 12.5, 12, 7.5);
        line(15, 8, 18, 8);
        line(15, 12, 18, 12);
    } else if (type == "LRadioButton") {
        p.drawEllipse(QRectF(3, 5, 10, 10));
        fillBox(7, 9, 2, 2, 1);
        line(15, 8, 18, 8);
        line(15, 12, 18, 12);
    } else if (type == "LSwitch") {
        box(2, 6, 16, 8, 4);
        fillBox(11, 7, 6, 6, 3);
    } else if (type == "LButtonMatrix") {
        for (int r = 0; r < 2; ++r) {
            for (int c = 0; c < 3; ++c) box(2.5 + c * 5.5, 5 + r * 6, 4.5, 4.5, 1);
        }
    } else if (type == "LGroupBox") {
        box(2, 4, 16, 13);
        line(2, 7, 18, 7);
        line(5, 10, 15, 10);
        line(5, 13, 12, 13);
    } else if (type == "LList") {
        box(2, 3, 16, 14);
        line(2, 7.5, 18, 7.5);
        line(2, 12.5, 18, 12.5);
    } else if (type == "LTabView") {
        box(2, 5, 16, 12);
        box(3, 2.5, 5, 4, 1);
        box(9, 2.5, 5, 4, 1);
        line(3, 10, 17, 10);
        line(3, 13, 13, 13);
    } else if (type == "LMenu" || type == "LMenuPage") {
        box(3, 2, 14, 16);
        line(6, 6, 14, 6);
        line(6, 10, 14, 10);
        line(6, 14, 11, 14);
    } else if (type == "LMenuCont") {
        box(2, 3, 16, 14);
        box(5, 6, 10, 8, 1);
    } else if (type == "LMenuSection") {
        box(2, 3, 16, 14);
        line(2, 8, 18, 8);
        line(2, 12.5, 18, 12.5);
    } else if (type == "LTileView") {
        box(2, 3, 7, 6, 1);
        box(11, 3, 7, 6, 1);
        box(2, 11, 7, 6, 1);
        box(11, 11, 7, 6, 1);
    } else if (type == "LTile") {
        box(3, 3, 14, 14, 2);
        line(6, 7, 14, 7);
        line(6, 11, 12, 11);
    } else if (type == "LWin") {
        box(2, 3, 16, 14);
        line(2, 7, 18, 7);
        line(6, 9.5, 9, 12.5);
        line(9, 9.5, 6, 12.5);
    } else if (type == "LCanvas") {
        box(3, 3, 14, 14);
        line(6, 14, 14, 6);
        line(6, 14, 6, 16);
        line(6, 16, 8, 14);
    } else if (type == "LLineEdit") {
        box(2, 6, 16, 8, 1);
        line(6, 8, 6, 12);
        line(8, 10, 14, 10);
    } else if (type == "LTextEdit") {
        box(2, 3, 16, 14, 1);
        line(5, 7, 15, 7);
        line(5, 10, 15, 10);
        line(5, 13, 11, 13);
    } else if (type == "LComboBox") {
        box(2, 5, 16, 10, 1);
        QPolygonF arrow;
        arrow << QPointF(12, 8.5) << QPointF(15, 8.5) << QPointF(13.5, 11);
        p.setBrush(ink);
        p.drawPolygon(arrow);
        p.setBrush(Qt::NoBrush);
    } else if (type == "LSlider") {
        line(3, 10, 17, 10);
        fillBox(11, 6, 5, 8, 2.5);
    } else if (type == "LSpinbox") {
        box(2, 5, 16, 10, 1);
        line(11, 10, 17.5, 10);
        line(13, 7, 14.5, 8.5);
        line(14.5, 8.5, 16, 7);
        line(13, 13, 14.5, 11.5);
        line(14.5, 11.5, 16, 13);
    } else if (type == "LRoller") {
        box(4, 2, 12, 16, 2);
        line(4, 7, 16, 7);
        line(4, 13, 16, 13);
        QPolygonF up;
        up << QPointF(8, 4.5) << QPointF(10, 2.5) << QPointF(12, 4.5);
        p.drawPolyline(up);
        QPolygonF down;
        down << QPointF(8, 15.5) << QPointF(10, 17.5) << QPointF(12, 15.5);
        p.drawPolyline(down);
    } else if (type == "LKeyboard") {
        box(1, 5, 18, 11, 1);
        for (int r = 0; r < 2; ++r) {
            for (int c = 0; c < 4; ++c) box(3 + c * 4, 7 + r * 3.5, 3, 2, 0.5);
        }
        box(5, 13.5, 10, 1.8, 0.5);
    } else if (type == "LLabel" || type == "LSpan") {
        QFont f = p.font();
        f.setPixelSize(13);
        f.setBold(true);
        p.setFont(f);
        p.drawText(QRectF(0, 0, S, S), Qt::AlignCenter, type == "LLabel" ? "A" : "T");
    } else if (type == "LImage") {
        box(2, 3, 16, 14, 1);
        QPolygonF mountain;
        mountain << QPointF(4, 14) << QPointF(8, 8) << QPointF(11, 11.5)
                 << QPointF(14, 7) << QPointF(17, 14);
        p.drawPolyline(mountain);
        fillBox(12.5, 5, 2, 2, 1);
    } else if (type == "LArclabel") {
        p.drawArc(QRectF(3, 3, 14, 14), 30 * 16, 120 * 16);
        QFont f = p.font();
        f.setPixelSize(9);
        p.setFont(f);
        p.drawText(QRectF(6, 5, 8, 8), Qt::AlignCenter, "A");
    } else if (type == "LBarcode") {
        for (int i = 0; i < 5; ++i) line(4 + i * 3, 4, 4 + i * 3, 16);
    } else if (type == "LQrcode") {
        box(2, 2, 7, 7, 1);
        box(11, 2, 7, 7, 1);
        box(2, 11, 7, 7, 1);
        fillBox(4, 4, 3, 3, 0.5);
        fillBox(13, 4, 3, 3, 0.5);
        fillBox(4, 13, 3, 3, 0.5);
        fillBox(12, 12, 2, 2, 0.5);
        fillBox(15, 15, 2, 2, 0.5);
    } else if (type == "LBar") {
        box(2, 6, 16, 8, 3);
        fillBox(3, 7, 9, 6, 2);
    } else if (type == "LArc") {
        p.drawArc(QRectF(3, 3, 14, 14), 40 * 16, 280 * 16);
    } else if (type == "LSpinner") {
        p.drawArc(QRectF(3, 3, 14, 14), 30 * 16, 270 * 16);
    } else if (type == "LLed") {
        p.drawEllipse(QRectF(5, 5, 10, 10));
        fillBox(7.5, 7.5, 5, 5, 2.5);
    } else if (type == "LLine") {
        line(3, 16, 17, 4);
    } else if (type == "LScale") {
        line(2, 12, 18, 12);
        for (int i = 0; i < 5; ++i) line(3 + i * 3.5, 12, 3 + i * 3.5, i % 2 ? 9 : 7);
    } else if (type == "LTable") {
        box(2, 3, 16, 14, 1);
        line(2, 7, 18, 7);
        line(2, 11.5, 18, 11.5);
        line(8.5, 7, 8.5, 17);
    } else if (type == "LChart") {
        box(2, 3, 16, 14, 1);
        fillBox(5, 10, 2.5, 5, 0.5);
        fillBox(9, 7, 2.5, 8, 0.5);
        fillBox(13, 9, 2.5, 6, 0.5);
    } else if (type == "LCalendar") {
        box(2, 4, 16, 13, 1);
        line(2, 7.5, 18, 7.5);
        for (int r = 0; r < 2; ++r) {
            for (int c = 0; c < 3; ++c) fillBox(4 + c * 4.5, 9.5 + r * 3.5, 2, 2, 0.3);
        }
    } else if (type == "LMsgBox") {
        box(2, 4, 16, 13, 2);
        line(2, 7.5, 18, 7.5);
        line(5, 10.5, 15, 10.5);
        line(5, 13.5, 11, 13.5);
    } else if (type == "LAnimImage") {
        box(2, 3, 16, 14, 1);
        QPolygonF mountain;
        mountain << QPointF(4, 14) << QPointF(8, 8) << QPointF(11, 11.5)
                 << QPointF(14, 7) << QPointF(17, 14);
        p.drawPolyline(mountain);
        QPolygonF play;
        play << QPointF(7, 6) << QPointF(11, 8) << QPointF(7, 10);
        p.setBrush(ink);
        p.drawPolygon(play);
        p.setBrush(Qt::NoBrush);
    } else if (type == "LLottie") {
        p.drawEllipse(QRectF(3, 3, 14, 14));
        QPolygonF play;
        play << QPointF(8, 6) << QPointF(14, 10) << QPointF(8, 14);
        p.setBrush(ink);
        p.drawPolygon(play);
        p.setBrush(Qt::NoBrush);
    } else {
        box(3, 3, 14, 14, 2);
        line(6, 8, 14, 8);
        line(6, 12, 11, 12);
    }

    p.end();
    return QIcon(pm);
}

} // namespace designer
