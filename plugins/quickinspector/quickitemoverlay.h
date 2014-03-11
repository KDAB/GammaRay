/*
  quickitemoverlay.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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

}

#endif // QUICKITEMOVERLAY_H
