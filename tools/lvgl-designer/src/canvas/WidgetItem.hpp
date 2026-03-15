#pragma once
#include <QGraphicsRectItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <QVariant>
#include <string>
#include <map>

namespace designer {

class DesignerScene;

enum class ResizeHandle { None, TopLeft, Top, TopRight, Right, BottomRight, Bottom, BottomLeft, Left };

class WidgetItem : public QGraphicsRectItem {
protected:
    std::string m_nodeId;
    std::string m_type;
    DesignerScene* m_scene = nullptr;
    bool m_resizing = false;
    ResizeHandle m_activeHandle = ResizeHandle::None;
    QPointF m_dragStart;
    QRectF m_dragStartRect;
    static constexpr double HANDLE_SIZE = 6.0;
    bool m_isContainer = false;

    std::map<std::string, QVariant> m_props;

public:
    WidgetItem(const std::string& nodeId, const std::string& type,
               QPointF pos, QSizeF size, QGraphicsItem* parent = nullptr);
    virtual ~WidgetItem() = default;

    const std::string& nodeId() const { return m_nodeId; }
    const std::string& widgetType() const { return m_type; }
    void setScene(DesignerScene* scene) { m_scene = scene; }
    DesignerScene* designerScene() const { return m_scene; }

    void setAllProps(const std::map<std::string, QVariant>& props) { m_props = props; }
    void setIsContainer(bool c) { m_isContainer = c; }
    bool isContainer() const { return m_isContainer; }

    QString propStr(const std::string& key, const QString& def = QString()) const {
        auto it = m_props.find(key);
        return (it != m_props.end()) ? it->second.toString() : def;
    }
    int propInt(const std::string& key, int def = 0) const {
        auto it = m_props.find(key);
        return (it != m_props.end()) ? it->second.toInt() : def;
    }
    bool propBool(const std::string& key, bool def = false) const {
        auto it = m_props.find(key);
        return (it != m_props.end()) ? it->second.toBool() : def;
    }
    uint32_t propHex(const std::string& key, uint32_t def = 0) const {
        auto it = m_props.find(key);
        if (it == m_props.end()) return def;
        QString s = it->second.toString();
        s.remove("0x").remove("0X");
        bool ok = false;
        uint32_t v = s.toUInt(&ok, 16);
        return ok ? v : it->second.toUInt();
    }
    QColor propColor(const std::string& key, QColor def = QColor(0,0,0)) const {
        uint32_t hex = propHex(key, (def.red() << 16) | (def.green() << 8) | def.blue());
        return QColor((hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF);
    }

    virtual void paintWidget(QPainter* painter, const QRectF& body) = 0;
    virtual QString displayName() const;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void applyObjectProps();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;

private:
    ResizeHandle handleAt(QPointF localPos) const;
    QCursor cursorForHandle(ResizeHandle h) const;
    void drawHandles(QPainter* painter);
};

}
