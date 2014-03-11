/*
  quickitemoverlay.cpp

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

#include "quickitemoverlay.h"
#include <QPainter>
#include <QVector2D>

using namespace GammaRay;

QuickItemOverlay::QuickItemOverlay(QDeclarativeItem* parent): QDeclarativeItem(parent)
{
  setFlags(flags() & ~QGraphicsItem::ItemHasNoContents);
}

QuickItemOverlay::~QuickItemOverlay()
{

}

void QuickItemOverlay::paint(QPainter* p, const QStyleOptionGraphicsItem* , QWidget* )
{
    if (m_geometryData.isEmpty())
      return;

    p->setTransform(QTransform::fromTranslate(m_imageRect.x(), m_imageRect.y()), true);

    // bounding box
    p->setPen(QColor("#aaE85752"));
    p->setBrush(QBrush(QColor("#5fE85752")));
    p->drawRect(m_boundingRect);

    // original geometry
    if (m_itemRect != m_boundingRect) {
      p->setPen(Qt::gray);
      p->setBrush(QBrush(Qt::gray, Qt::BDiagPattern));
      p->drawRect(m_itemRect);
    }

    // children rect
    if (m_itemRect != m_boundingRect && m_transform.isIdentity()) { // If this item is transformed the children rect will be painted wrongly, so for now skip painting it.
      p->setPen(QColor("#aa0063c1"));
      p->setBrush(QBrush(QColor("#5f0063c1")));
      p->drawRect(m_childrenRect);
    }

    // transform origin
    p->setPen(QColor("#aa9C0F56"));
    p->drawEllipse(m_transformOriginPoint, 2.5, 2.5);
    p->drawLine(m_transformOriginPoint - QPointF(0, 6), m_transformOriginPoint + QPointF(0, 6));
    p->drawLine(m_transformOriginPoint - QPointF(6, 0), m_transformOriginPoint + QPointF(6, 0));

    // x and y values
    p->setPen(QColor("#888"));
    if (!m_geometryData.value("left").toBool() && !m_geometryData.value("horizontalCenter").toBool() && !m_geometryData.value("right").toBool() && m_x != 0) {
        QPointF parentEnd = (QPointF(m_itemRect.x() - m_x, m_itemRect.y()));
        QPointF itemEnd = m_itemRect.topLeft();
        drawArrow(p, parentEnd, itemEnd);
        p->drawText(QRectF(parentEnd.x(), parentEnd.y() + 10, itemEnd.x() - parentEnd.x(), 50),
                    Qt::AlignHCenter | Qt::TextDontClip,
                    QString("x: %1px").arg(m_x / m_zoom));
    }
    if (!m_geometryData.value("top").toBool() && !m_geometryData.value("verticalCenter").toBool() && !m_geometryData.value("bottom").toBool() && !m_geometryData.value("baseline").toBool() && m_y != 0) {
        QPointF parentEnd = (QPointF(m_itemRect.x(), m_itemRect.y() - m_y));
        QPointF itemEnd = m_itemRect.topLeft();
        drawArrow(p, parentEnd, itemEnd);
        p->drawText(QRectF(parentEnd.x() + 10, parentEnd.y(), 100, itemEnd.y() - parentEnd.y()),
                    Qt::AlignVCenter | Qt::TextDontClip,
                    QString("y: %1px").arg(m_y / m_zoom));
    }

    // anchors
    if (m_geometryData.value("left").toBool())
      drawAnchor(p, Qt::Horizontal, m_itemRect.left(), m_leftMargin, QString("margin: %1px").arg(m_leftMargin / m_zoom));
    if (m_geometryData.value("horizontalCenter").toBool())
      drawAnchor(p, Qt::Horizontal, (m_itemRect.left() + m_itemRect.right()) / 2, m_horizonalCenterOffset, QString("offset: %1px").arg(m_horizonalCenterOffset / m_zoom));
    if (m_geometryData.value("right").toBool())
      drawAnchor(p, Qt::Horizontal, m_itemRect.right(), -m_rightMargin, QString("margin: %1px").arg(m_rightMargin / m_zoom));
    if (m_geometryData.value("top").toBool())
      drawAnchor(p, Qt::Vertical, m_itemRect.top(), m_topMargin, QString("margin: %1px").arg(m_topMargin / m_zoom));
    if (m_geometryData.value("verticalCenter").toBool())
      drawAnchor(p, Qt::Vertical, (m_itemRect.top() + m_itemRect.bottom()) / 2, m_verticalCenterOffset, QString("offset: %1px").arg(m_verticalCenterOffset / m_zoom));
    if (m_geometryData.value("bottom").toBool())
      drawAnchor(p, Qt::Vertical, m_itemRect.bottom(), -m_bottomMargin, QString("margin: %1px").arg(m_bottomMargin / m_zoom));
    if (m_geometryData.value("baseline").toBool())
      drawAnchor(p, Qt::Vertical, m_itemRect.top(), m_baselineOffset, QString("offset: %1px").arg(m_baselineOffset / m_zoom));
}

void QuickItemOverlay::drawArrow(QPainter* p, QPointF first, QPointF second)
{
  p->drawLine(first, second);
  QPointF vector(second - first);
  QMatrix m;
  m.rotate(30);
  QVector2D v1 = QVector2D(m.map(vector)).normalized() * 10;
  m.rotate(-60);
  QVector2D v2 = QVector2D(m.map(vector)).normalized() * 10;
  p->drawLine(first, first + v1.toPointF());
  p->drawLine(first, first + v2.toPointF());
  p->drawLine(second, second - v1.toPointF());
  p->drawLine(second, second - v2.toPointF());
}

void QuickItemOverlay::drawAnchor(QPainter* p, Qt::Orientation orientation, qreal ownAnchorLine, qreal offset, const QString &label)
{
    qreal foreignAnchorLine = ownAnchorLine - offset;
    QPen pen(QColor("#8BB300"));

    // Margin arrow
    if (offset) {
        p->setPen(pen);
        if (orientation == Qt::Horizontal)
          drawArrow(p, QPointF(foreignAnchorLine, (m_itemRect.top() + m_itemRect.bottom()) / 2), QPointF(ownAnchorLine, (m_itemRect.top() + m_itemRect.bottom()) / 2));
        else
          drawArrow(p, QPointF((m_itemRect.left() + m_itemRect.right()) / 2, foreignAnchorLine), QPointF((m_itemRect.left() + m_itemRect.right()) / 2, ownAnchorLine));

        // Margin text
        if (orientation == Qt::Horizontal)
          p->drawText(QRectF(foreignAnchorLine, (m_itemRect.top() + m_itemRect.bottom()) / 2 + 10, offset, 50),
                      Qt::AlignHCenter | Qt::TextDontClip,
                      label);
        else
          p->drawText(QRectF((m_itemRect.left() + m_itemRect.right()) / 2 + 10, foreignAnchorLine, 100, offset),
                      Qt::AlignVCenter | Qt::TextDontClip,
                      label);
    }

    // Own Anchor line
    pen.setWidth(2);
    p->setPen(pen);
    if (orientation == Qt::Horizontal)
      p->drawLine(ownAnchorLine, m_itemRect.top(), ownAnchorLine, m_itemRect.bottom());
    else
      p->drawLine(m_itemRect.left(), ownAnchorLine, m_itemRect.right(), ownAnchorLine);

    // Foreign Anchor line
    pen.setStyle(Qt::DotLine);
    p->setPen(pen);
    if (orientation == Qt::Horizontal)
      p->drawLine(foreignAnchorLine, 0, foreignAnchorLine, m_imageRect.height());
    else
      p->drawLine(0, foreignAnchorLine, m_imageRect.width(), foreignAnchorLine);
}

QVariantMap QuickItemOverlay::geometryData() const
{
    return m_geometryData;
}

void QuickItemOverlay::setGeometryData(QVariantMap geometryData)
{
    m_geometryData = geometryData;

    updateGeometryData();

    update();
}

void QuickItemOverlay::updateGeometryData()
{
    m_itemRect = m_geometryData.value("itemRect").value<QRectF>();
    m_itemRect = QRectF(m_itemRect.topLeft() * m_zoom, m_itemRect.bottomRight() * m_zoom);
    m_boundingRect = m_geometryData.value("boundingRect").value<QRectF>();
    m_boundingRect = QRectF(m_boundingRect.topLeft() * m_zoom, m_boundingRect.bottomRight() * m_zoom);
    m_childrenRect = m_geometryData.value("childrenRect").value<QRectF>();
    m_childrenRect = QRectF(m_childrenRect.topLeft() * m_zoom, m_childrenRect.bottomRight() * m_zoom);
    m_transformOriginPoint = m_geometryData.value("transformOriginPoint").value<QPointF>() * m_zoom;
    m_transform = m_geometryData.value("transform").value<QTransform>();
    m_parentTransform = m_geometryData.value("parentTransform").value<QTransform>();
    m_leftMargin = m_geometryData.value("leftMargin").toReal() * m_zoom;
    m_horizonalCenterOffset = m_geometryData.value("horizontalCenterOffset").toReal() * m_zoom;
    m_rightMargin = m_geometryData.value("rightMargin").toReal() * m_zoom;
    m_topMargin = m_geometryData.value("topMargin").toReal() * m_zoom;
    m_verticalCenterOffset = m_geometryData.value("verticalCenterOffset").toReal() * m_zoom;
    m_bottomMargin = m_geometryData.value("bottomMargin").toReal() * m_zoom;
    m_baselineOffset = m_geometryData.value("baselineOffset").toReal() * m_zoom;
    m_x = m_geometryData.value("x").value<qreal>() * m_zoom;
    m_y = m_geometryData.value("y").value<qreal>() * m_zoom;
}

qreal QuickItemOverlay::zoom() const
{
    return m_zoom;
}

void QuickItemOverlay::setZoom(qreal zoom)
{
    m_zoom = zoom;
    updateGeometryData();
    update();
}

QRectF QuickItemOverlay::imageRect() const
{
    return m_imageRect;
}

void QuickItemOverlay::setImageRect(QRectF imageRect)
{
    m_imageRect = imageRect;
    update();
}
