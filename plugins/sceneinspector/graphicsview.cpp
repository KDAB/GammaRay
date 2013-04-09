/*
  graphicsview.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "graphicsview.h"

#include <QGraphicsItem>
#include <QKeyEvent>

using namespace GammaRay;

GraphicsView::GraphicsView(QWidget *parent)
  : QGraphicsView(parent),
    m_currentItem(0)
{
  setMouseTracking(true);
}

void GraphicsView::showItem(QGraphicsItem *item)
{
  m_currentItem = item;
  fitInView(item, Qt::KeepAspectRatio);
  scale(0.8f, 0.8f);
}

void GraphicsView::keyPressEvent(QKeyEvent *event)
{
  if (event->modifiers() == Qt::CTRL) {
    switch (event->key()) {
    case Qt::Key_Plus:
      scale(1.2, 1.2);
      event->accept();
      return;
    case Qt::Key_Minus:
      scale(0.8, 0.8);
      event->accept();
      return;
    case Qt::Key_Left:
      rotate(-5);
      event->accept();
      break;
    case Qt::Key_Right:
      rotate(5);
      event->accept();
      break;
    }
  }
  QGraphicsView::keyPressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
  emit sceneCoordinatesChanged(mapToScene(event->pos()));
  if (m_currentItem) {
    emit itemCoordinatesChanged(m_currentItem->mapFromScene(mapToScene(event->pos())));
  }
  QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::drawForeground(QPainter *painter, const QRectF &rect)
{
  QGraphicsView::drawForeground(painter, rect);
  if (m_currentItem) {
    const QRectF itemBoundingRect = m_currentItem->boundingRect();
    // coord system, TODO: nicer axis with arrows, tics, markers for current mouse position etc.
    painter->setPen(Qt::black);
    const qreal maxX = qMax(qAbs(itemBoundingRect.left()), qAbs(itemBoundingRect.right()));
    const qreal maxY = qMax(qAbs(itemBoundingRect.top()), qAbs(itemBoundingRect.bottom()));
    const qreal maxXY = qMax(maxX, maxY) * 1.5f;
    painter->drawLine(m_currentItem->mapToScene(-maxXY, 0), m_currentItem->mapToScene(maxXY, 0));
    painter->drawLine(m_currentItem->mapToScene(0, -maxXY), m_currentItem->mapToScene(0, maxXY));

    painter->setPen(Qt::blue);
    const QPolygonF boundingBox = m_currentItem->mapToScene(itemBoundingRect);
    painter->drawPolygon(boundingBox);

    painter->setPen(Qt::green);
    const QPainterPath shape = m_currentItem->mapToScene(m_currentItem->shape());
    painter->drawPath(shape);

    painter->setPen(Qt::red);
    const QPointF transformOrigin =
      m_currentItem->mapToScene(m_currentItem->transformOriginPoint());
    painter->drawEllipse(transformOrigin,
                         5.0 / transform().m11(),
                         5.0 / transform().m22());
  }
}

#include "graphicsview.moc"
