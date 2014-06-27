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

#include "annotatedscenepreview.h"

#include <QPainter>
#include <QVector2D>

using namespace GammaRay;

AnnotatedScenePreview::AnnotatedScenePreview(QQuickItem *parent)
  : QQuickPaintedItem(parent),
    m_zoom(1),
    m_leftMargin(0),
    m_horizonalCenterOffset(0),
    m_rightMargin(0),
    m_topMargin(0),
    m_verticalCenterOffset(0),
    m_bottomMargin(0),
    m_baselineOffset(0),
    m_x(0),
    m_y(0)
{
}

AnnotatedScenePreview::~AnnotatedScenePreview()
{
}

void AnnotatedScenePreview::paint(QPainter *p)
{
  if (m_previewData.isEmpty()) {
    return;
  }

  p->setTransform(QTransform::fromTranslate(m_margin.width() / 2, m_margin.height() / 2));

  p->fillRect(QRect(QPoint(0, 0), m_image.size() * m_zoom), Qt::white);
  p->drawImage(QRect(QPoint(0, 0), m_image.size() * m_zoom),
               m_image.transformed(QTransform::fromScale(1, -1)));

  // bounding box
  p->setPen(QColor(232, 87, 82, 170));
  p->setBrush(QBrush(QColor(232, 87, 82, 95)));
  p->drawRect(m_boundingRect);

  // original geometry
  if (m_itemRect != m_boundingRect) {
    p->setPen(Qt::gray);
    p->setBrush(QBrush(Qt::gray, Qt::BDiagPattern));
    p->drawRect(m_itemRect);
  }

  // children rect
  if (m_itemRect != m_boundingRect && m_transform.isIdentity()) {
    // If this item is transformed the children rect will be painted wrongly,
    // so for now skip painting it.
    p->setPen(QColor(0, 99, 193, 170));
    p->setBrush(QBrush(QColor(0, 99, 193, 95)));
    p->drawRect(m_childrenRect);
  }

  // transform origin
  if (m_itemRect != m_boundingRect) {
    p->setPen(QColor(156, 15, 86, 170));
    p->drawEllipse(m_transformOriginPoint, 2.5, 2.5);
    p->drawLine(m_transformOriginPoint - QPointF(0, 6), m_transformOriginPoint + QPointF(0, 6));
    p->drawLine(m_transformOriginPoint - QPointF(6, 0), m_transformOriginPoint + QPointF(6, 0));
  }

  // x and y values
  p->setPen(QColor(136, 136, 136));
  if (!m_previewData.value("left").toBool() &&
      !m_previewData.value("horizontalCenter").toBool() &&
      !m_previewData.value("right").toBool() &&
      m_x != 0) {
    QPointF parentEnd = (QPointF(m_itemRect.x() - m_x, m_itemRect.y()));
    QPointF itemEnd = m_itemRect.topLeft();
    drawArrow(p, parentEnd, itemEnd);
    p->drawText(QRectF(parentEnd.x(), parentEnd.y() + 10, itemEnd.x() - parentEnd.x(), 50),
                Qt::AlignHCenter | Qt::TextDontClip,
                QString("x: %1px").arg(m_x / m_zoom));
  }
  if (!m_previewData.value("top").toBool() &&
      !m_previewData.value("verticalCenter").toBool() &&
      !m_previewData.value("bottom").toBool() &&
      !m_previewData.value("baseline").toBool() &&
      m_y != 0) {
    QPointF parentEnd = (QPointF(m_itemRect.x(), m_itemRect.y() - m_y));
    QPointF itemEnd = m_itemRect.topLeft();
    drawArrow(p, parentEnd, itemEnd);
    p->drawText(QRectF(parentEnd.x() + 10, parentEnd.y(), 100, itemEnd.y() - parentEnd.y()),
                Qt::AlignVCenter | Qt::TextDontClip,
                QString("y: %1px").arg(m_y / m_zoom));
  }

  // anchors
  if (m_previewData.value("left").toBool()) {
    drawAnchor(p, Qt::Horizontal,
               m_itemRect.left(), m_leftMargin,
               QString("margin: %1px").arg(m_leftMargin / m_zoom));
  }

  if (m_previewData.value("horizontalCenter").toBool()) {
    drawAnchor(p, Qt::Horizontal,
               (m_itemRect.left() + m_itemRect.right()) / 2, m_horizonalCenterOffset,
               QString("offset: %1px").arg(m_horizonalCenterOffset / m_zoom));
  }

  if (m_previewData.value("right").toBool()) {
    drawAnchor(p, Qt::Horizontal,
               m_itemRect.right(), -m_rightMargin,
               QString("margin: %1px").arg(m_rightMargin / m_zoom));
  }

  if (m_previewData.value("top").toBool()) {
    drawAnchor(p, Qt::Vertical,
               m_itemRect.top(), m_topMargin,
               QString("margin: %1px").arg(m_topMargin / m_zoom));
  }

  if (m_previewData.value("verticalCenter").toBool()) {
    drawAnchor(p, Qt::Vertical,
               (m_itemRect.top() + m_itemRect.bottom()) / 2, m_verticalCenterOffset,
               QString("offset: %1px").arg(m_verticalCenterOffset / m_zoom));
  }

  if (m_previewData.value("bottom").toBool()) {
    drawAnchor(p, Qt::Vertical,
               m_itemRect.bottom(), -m_bottomMargin,
               QString("margin: %1px").arg(m_bottomMargin / m_zoom));
  }

  if (m_previewData.value("baseline").toBool()) {
    drawAnchor(p, Qt::Vertical,
               m_itemRect.top(), m_baselineOffset,
               QString("offset: %1px").arg(m_baselineOffset / m_zoom));
  }
}

void AnnotatedScenePreview::drawArrow(QPainter *p, QPointF first, QPointF second)
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

void AnnotatedScenePreview::drawAnchor(QPainter *p, Qt::Orientation orientation,
                                       qreal ownAnchorLine, qreal offset,
                                       const QString &label)
{
  qreal foreignAnchorLine = ownAnchorLine - offset;
  QPen pen(QColor(139, 179, 0));

  // Margin arrow
  if (offset) {
    p->setPen(pen);
    if (orientation == Qt::Horizontal) {
      drawArrow(p,
                QPointF(foreignAnchorLine, (m_itemRect.top() + m_itemRect.bottom()) / 2),
                QPointF(ownAnchorLine, (m_itemRect.top() + m_itemRect.bottom()) / 2));
    } else {
      drawArrow(p,
                QPointF((m_itemRect.left() + m_itemRect.right()) / 2, foreignAnchorLine),
                QPointF((m_itemRect.left() + m_itemRect.right()) / 2, ownAnchorLine));
    }

    // Margin text
    if (orientation == Qt::Horizontal) {
      p->drawText(
        QRectF(foreignAnchorLine,
               (m_itemRect.top() + m_itemRect.bottom()) / 2 + 10, offset, 50),
        Qt::AlignHCenter | Qt::TextDontClip,
        label);
    } else {
      p->drawText(
        QRectF((m_itemRect.left() + m_itemRect.right()) / 2 + 10,
               foreignAnchorLine, 100, offset),
        Qt::AlignVCenter | Qt::TextDontClip,
        label);
    }
  }

  // Own Anchor line
  pen.setWidth(2);
  p->setPen(pen);
  if (orientation == Qt::Horizontal) {
    p->drawLine(ownAnchorLine, m_itemRect.top(), ownAnchorLine, m_itemRect.bottom());
  } else {
    p->drawLine(m_itemRect.left(), ownAnchorLine, m_itemRect.right(), ownAnchorLine);
  }

  // Foreign Anchor line
  pen.setStyle(Qt::DotLine);
  p->setPen(pen);
  if (orientation == Qt::Horizontal) {
    p->drawLine(foreignAnchorLine, 0, foreignAnchorLine, m_image.height() * m_zoom);
  } else {
    p->drawLine(0, foreignAnchorLine, m_image.width() * m_zoom, foreignAnchorLine);
  }
}

QVariantMap AnnotatedScenePreview::previewData() const
{
  return m_previewData;
}

void AnnotatedScenePreview::setPreviewData(QVariantMap previewData)
{
  m_previewData = previewData;

  updatePreviewData();
  update();

  emit previewDataChanged();
}

void AnnotatedScenePreview::updatePreviewData()
{
  QImage oldImage = m_image;
  m_image = m_previewData.value("image").value<QImage>();

  if (m_image.size() != oldImage.size()) {
    emit sourceSizeChanged();
    setImplicitHeight(m_zoom * m_image.height() + m_margin.height());
    setImplicitWidth(m_zoom * m_image.width() + m_margin.width());
  }

  m_itemRect = m_previewData.value("itemRect").value<QRectF>();
  m_itemRect = QRectF(m_itemRect.topLeft() * m_zoom, m_itemRect.bottomRight() * m_zoom);
  m_boundingRect = m_previewData.value("boundingRect").value<QRectF>();
  m_boundingRect = QRectF(m_boundingRect.topLeft() * m_zoom, m_boundingRect.bottomRight() * m_zoom);
  m_childrenRect = m_previewData.value("childrenRect").value<QRectF>();
  m_childrenRect = QRectF(m_childrenRect.topLeft() * m_zoom, m_childrenRect.bottomRight() * m_zoom);
  m_transformOriginPoint = m_previewData.value("transformOriginPoint").value<QPointF>() * m_zoom;
  m_transform = m_previewData.value("transform").value<QTransform>();
  m_parentTransform = m_previewData.value("parentTransform").value<QTransform>();
  m_leftMargin = m_previewData.value("leftMargin").toReal() * m_zoom;
  m_horizonalCenterOffset = m_previewData.value("horizontalCenterOffset").toReal() * m_zoom;
  m_rightMargin = m_previewData.value("rightMargin").toReal() * m_zoom;
  m_topMargin = m_previewData.value("topMargin").toReal() * m_zoom;
  m_verticalCenterOffset = m_previewData.value("verticalCenterOffset").toReal() * m_zoom;
  m_bottomMargin = m_previewData.value("bottomMargin").toReal() * m_zoom;
  m_baselineOffset = m_previewData.value("baselineOffset").toReal() * m_zoom;
  m_x = m_previewData.value("x").value<qreal>() * m_zoom;
  m_y = m_previewData.value("y").value<qreal>() * m_zoom;
}

qreal AnnotatedScenePreview::zoom() const
{
  return m_zoom;
}

void AnnotatedScenePreview::setZoom(qreal zoom)
{
  m_zoom = zoom;
  updatePreviewData();
  update();
  emit zoomChanged();
  setImplicitHeight(m_zoom * m_image.height() + m_margin.height());
  setImplicitWidth(m_zoom * m_image.width() + m_margin.width());
}

QSize AnnotatedScenePreview::sourceSize() const
{
  return m_image.size();
}

QSize AnnotatedScenePreview::margin() const
{
  return m_margin;
}

void AnnotatedScenePreview::setMargin(QSize margin)
{
  m_margin = margin;
  setImplicitHeight(m_zoom * m_image.height() + m_margin.height());
  setImplicitWidth(m_zoom * m_image.width() + m_margin.width());
  emit marginChanged();
}
