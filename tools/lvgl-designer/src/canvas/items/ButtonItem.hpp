#pragma once
#include "canvas/WidgetItem.hpp"

namespace designer {

class ButtonItem : public WidgetItem {
public:
    using WidgetItem::WidgetItem;
    void paintWidget(QPainter* painter, const QRectF& body) override {
        QString theme = propStr("theme", "primary");
        QColor bg(0x0e, 0x63, 0x9c);
        if (theme == "success") bg = QColor(0x4c, 0xaf, 0x50);
        else if (theme == "danger") bg = QColor(0xf4, 0x43, 0x36);
        else if (theme == "warning") bg = QColor(0xff, 0x98, 0x00);

        painter->setBrush(bg);
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(body, 6, 6);

        painter->setPen(Qt::white);
        painter->setFont(QFont("Sans", 11));
        painter->drawText(body, Qt::AlignCenter, displayName());
    }
};

class LabelItem : public WidgetItem {
public:
    using WidgetItem::WidgetItem;
    void paintWidget(QPainter* painter, const QRectF& body) override {
        QColor color = propColor("color", QColor(0xcc, 0xcc, 0xcc));
        int fontSize = propInt("font_size", 14);
        painter->setPen(color);
        painter->setFont(QFont("Sans", fontSize));
        painter->drawText(body, Qt::AlignLeft | Qt::AlignVCenter, displayName());
    }
};

class SliderItem : public WidgetItem {
public:
    using WidgetItem::WidgetItem;
    void paintWidget(QPainter* painter, const QRectF& body) override {
        painter->setBrush(QColor(0x44, 0x44, 0x44));
        painter->setPen(Qt::NoPen);
        double rr = body.height() / 2;
        painter->drawRoundedRect(body, rr, rr);

        int val = propInt("value", 50);
        int maxVal = 100;
        auto it = m_props.find("range");
        if (it != m_props.end() && it->second.toList().size() == 2)
            maxVal = it->second.toList()[1].toInt();
        if (maxVal <= 0) maxVal = 100;

        double ratio = qBound(0.0, (double)val / maxVal, 1.0);
        QRectF fill(body.left(), body.top(), body.width() * ratio, body.height());
        painter->setBrush(QColor(0x0e, 0x63, 0x9c));
        painter->drawRoundedRect(fill, rr, rr);
    }
};

class BarItem : public WidgetItem {
public:
    using WidgetItem::WidgetItem;
    void paintWidget(QPainter* painter, const QRectF& body) override {
        painter->setBrush(QColor(0x44, 0x44, 0x44));
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(body, 4, 4);

        int val = propInt("value", 0);
        int maxVal = 100;
        auto it = m_props.find("range");
        if (it != m_props.end() && it->second.toList().size() == 2)
            maxVal = it->second.toList()[1].toInt();
        if (maxVal <= 0) maxVal = 100;

        double ratio = qBound(0.0, (double)val / maxVal, 1.0);
        QRectF fill(body.left(), body.top(), body.width() * ratio, body.height());
        painter->setBrush(QColor(0x4c, 0xaf, 0x50));
        painter->drawRoundedRect(fill, 4, 4);
    }
};

class CheckboxItem : public WidgetItem {
public:
    using WidgetItem::WidgetItem;
    void paintWidget(QPainter* painter, const QRectF& body) override {
        bool checked = propBool("checked", false);
        double boxSize = qMin(body.height() - 6, 18.0);
        QRectF box(body.left() + 3, body.center().y() - boxSize / 2.0, boxSize, boxSize);

        painter->setPen(QPen(QColor(0x88, 0x88, 0x88), 1.5));
        painter->setBrush(checked ? QColor(0x0e, 0x63, 0x9c) : QColor(0x3c, 0x3c, 0x3c));
        painter->drawRoundedRect(box, 3, 3);

        if (checked) {
            painter->setPen(QPen(Qt::white, 2));
            painter->drawLine(QPointF(box.left()+4, box.center().y()),
                              QPointF(box.center().x(), box.bottom()-4));
            painter->drawLine(QPointF(box.center().x(), box.bottom()-4),
                              QPointF(box.right()-3, box.top()+4));
        }

        painter->setPen(QColor(0xcc, 0xcc, 0xcc));
        painter->setFont(QFont("Sans", 11));
        QRectF textRect(box.right() + 8, body.top(), body.width() - boxSize - 11, body.height());
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, displayName());
    }
};

class RadioButtonItem : public WidgetItem {
public:
    using WidgetItem::WidgetItem;
    void paintWidget(QPainter* painter, const QRectF& body) override {
        bool checked = propBool("checked", false);
        double outerSize = qMin(body.height() - 6, 18.0);
        QRectF outer(body.left() + 3, body.center().y() - outerSize / 2.0, outerSize, outerSize);

        painter->setPen(QPen(QColor(0x88, 0x88, 0x88), 1.8));
        painter->setBrush(QColor(0x3c, 0x3c, 0x3c));
        painter->drawEllipse(outer);

        if (checked) {
            double innerSize = qMax(outerSize * 0.52, 7.0);
            QRectF inner(outer.center().x() - innerSize / 2.0,
                         outer.center().y() - innerSize / 2.0,
                         innerSize,
                         innerSize);
            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor(0x0e, 0x63, 0x9c));
            painter->drawEllipse(inner);
        }

        painter->setPen(QColor(0xcc, 0xcc, 0xcc));
        painter->setFont(QFont("Sans", 11));
        QRectF textRect(outer.right() + 8, body.top(), body.width() - outerSize - 11, body.height());
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, displayName());
    }
};

class SwitchItem : public WidgetItem {
public:
    using WidgetItem::WidgetItem;
    void paintWidget(QPainter* painter, const QRectF& body) override {
        bool on = propBool("checked", false);
        QColor bg = on ? QColor(0x4c, 0xaf, 0x50) : QColor(0x66, 0x66, 0x66);
        painter->setBrush(bg);
        painter->setPen(Qt::NoPen);
        double rr = body.height() / 2;
        painter->drawRoundedRect(body, rr, rr);

        double knobR = body.height() - 4;
        double knobX = on ? body.right() - knobR - 2 : body.left() + 2;
        painter->setBrush(Qt::white);
        painter->drawEllipse(QRectF(knobX, body.top() + 2, knobR, knobR));
    }
};

class DropdownItem : public WidgetItem {
public:
    using WidgetItem::WidgetItem;
    void paintWidget(QPainter* painter, const QRectF& body) override {
        painter->setBrush(QColor(0x3c, 0x3c, 0x3c));
        painter->setPen(QPen(QColor(0x55, 0x55, 0x55), 1));
        painter->drawRoundedRect(body, 4, 4);

        painter->setPen(QColor(0xcc, 0xcc, 0xcc));
        painter->setFont(QFont("Sans", 11));
        QRectF textRect(body.left() + 8, body.top(), body.width() - 32, body.height());
        QString opts = propStr("options", "Select...");
        opts.replace("\\n", "\n");
        QString first = opts.split("\n").first();
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, first);

        painter->setPen(QPen(QColor(0xaa, 0xaa, 0xaa), 1.5));
        double ax = body.right() - 16;
        double ay = body.center().y() - 3;
        painter->drawLine(QPointF(ax - 4, ay), QPointF(ax, ay + 6));
        painter->drawLine(QPointF(ax, ay + 6), QPointF(ax + 4, ay));
    }
};

class TextAreaItem : public WidgetItem {
public:
    using WidgetItem::WidgetItem;
    void paintWidget(QPainter* painter, const QRectF& body) override {
        painter->setBrush(QColor(0x3c, 0x3c, 0x3c));
        painter->setPen(QPen(QColor(0x55, 0x55, 0x55), 1));
        painter->drawRoundedRect(body, 4, 4);

        QString text = propStr("text");
        if (text.isEmpty()) text = propStr("placeholder", "Enter text...");
        painter->setPen(propStr("text").isEmpty() ? QColor(0x66, 0x66, 0x66) : QColor(0xcc, 0xcc, 0xcc));
        painter->setFont(QFont("Sans", 11));
        painter->drawText(body.adjusted(6, 4, -4, -4), Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, text);
    }
};

class ArcItem : public WidgetItem {
public:
    using WidgetItem::WidgetItem;
    void paintWidget(QPainter* painter, const QRectF& body) override {
        painter->setPen(QPen(QColor(0x44, 0x44, 0x44), 6));
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(body.adjusted(4, 4, -4, -4));

        int val = propInt("value", 0);
        int maxVal = 100;
        auto it = m_props.find("range");
        if (it != m_props.end() && it->second.toList().size() == 2)
            maxVal = it->second.toList()[1].toInt();
        if (maxVal <= 0) maxVal = 100;
        int span = (int)(360.0 * val / maxVal);
        painter->setPen(QPen(QColor(0x0e, 0x63, 0x9c), 6));
        painter->drawArc(body.adjusted(4, 4, -4, -4).toRect(), 90 * 16, -span * 16);
    }
};

class SpinnerItem : public WidgetItem {
public:
    using WidgetItem::WidgetItem;
    void paintWidget(QPainter* painter, const QRectF& body) override {
        painter->setPen(QPen(QColor(0x44, 0x44, 0x44), 4));
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(body.adjusted(4, 4, -4, -4));
        painter->setPen(QPen(QColor(0x0e, 0x63, 0x9c), 4));
        painter->drawArc(body.adjusted(4, 4, -4, -4).toRect(), 90 * 16, -90 * 16);
    }
};

class LedItem : public WidgetItem {
public:
    using WidgetItem::WidgetItem;
    void paintWidget(QPainter* painter, const QRectF& body) override {
        QColor c = propColor("color", QColor(0x00, 0xff, 0x00));
        if (!propBool("on", true)) c = c.darker(200);
        painter->setBrush(c);
        painter->setPen(QPen(c.lighter(150), 1));
        painter->drawEllipse(body.adjusted(2, 2, -2, -2));
    }
};

class ContainerItem : public WidgetItem {
public:
    using WidgetItem::WidgetItem;
    void paintWidget(QPainter* painter, const QRectF& body) override {
        painter->setBrush(QColor(0x30, 0x30, 0x30));
        painter->setPen(QPen(QColor(0x55, 0x55, 0x55), 1, Qt::DashLine));
        painter->drawRoundedRect(body, 4, 4);

        painter->setPen(QColor(0x88, 0x88, 0x88));
        painter->setFont(QFont("Sans", 9));
        painter->drawText(body.adjusted(4, 2, 0, 0), Qt::AlignLeft | Qt::AlignTop,
                          QString::fromStdString(m_type));
    }
};

class GenericItem : public WidgetItem {
public:
    using WidgetItem::WidgetItem;
    void paintWidget(QPainter* painter, const QRectF& body) override {
        painter->setBrush(QColor(0x3a, 0x3a, 0x3a));
        painter->setPen(QPen(QColor(0x66, 0x66, 0x66), 1));
        painter->drawRoundedRect(body, 4, 4);

        painter->setPen(QColor(0xaa, 0xaa, 0xaa));
        painter->setFont(QFont("Sans", 10));
        painter->drawText(body, Qt::AlignCenter, displayName());
    }
};

}
