#ifndef QUICKITEMOVERLAY_H
#define QUICKITEMOVERLAY_H

#include <QDeclarativeItem>

namespace GammaRay {

class QuickItemOverlay : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap geometryData READ geometryData WRITE setGeometryData)
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom)
    Q_PROPERTY(QRectF imageRect READ imageRect WRITE setImageRect)

public:
    QuickItemOverlay(QDeclarativeItem *parent = 0);
    virtual ~QuickItemOverlay();
    virtual void paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget* );

    QVariantMap geometryData() const;
    qreal zoom() const;
    QRectF imageRect() const;

public Q_SLOTS:
    void setGeometryData(QVariantMap geometryData);
    void setZoom(qreal zoom);
    void setImageRect(QRectF imageRect);

private:
    void drawArrow(QPainter* p, QPointF first, QPointF second);
    void drawAnchor(QPainter* p, Qt::Orientation orientation, qreal ownAnchorLine, qreal offset, const QString& label);
    void updateGeometryData();

private:
    QVariantMap m_geometryData;
    qreal m_zoom;
    QRectF m_imageRect;
    QRectF m_itemRect;
    QRectF m_boundingRect;
    QRectF m_childrenRect;
    QPointF m_transformOriginPoint;
    QTransform m_transform;
    QTransform m_parentTransform;
    qreal m_leftMargin;
    qreal m_horizonalCenterOffset;
    qreal m_rightMargin;
    qreal m_topMargin;
    qreal m_verticalCenterOffset;
    qreal m_bottomMargin;
    qreal m_baselineOffset;
    qreal m_x;
    qreal m_y;
};

};

#endif // QUICKITEMOVERLAY_H
