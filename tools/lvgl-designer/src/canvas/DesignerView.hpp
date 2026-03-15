#pragma once
#include <QGraphicsView>

namespace designer {

class DesignerView : public QGraphicsView {
    Q_OBJECT
    double m_zoomFactor = 1.0;

public:
    explicit DesignerView(QWidget* parent = nullptr);
    void zoomIn();
    void zoomOut();
    void zoomReset();

protected:
    void wheelEvent(QWheelEvent* event) override;
};

}
