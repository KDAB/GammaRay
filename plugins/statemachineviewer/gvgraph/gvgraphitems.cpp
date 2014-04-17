/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#include "gvgraphitems.h"

#include <QDebug>
#include <QPen>

using namespace GammaRay;

GVNodeItem::GVNodeItem(const GVNode &node, QGraphicsItem *parent)
  : QGraphicsItemGroup(parent),
    m_node(node)
{
  const QSizeF size = node.size();
  const QRectF rect(-size.width() / 2, -size.height() / 2,
                    size.width(), size.height());
  switch (node.shape()) {
    case GVNode::Rect:
    {
      QGraphicsRectItem *item = new QGraphicsRectItem(this);
      item->setRect(rect);
      m_shapeItem = item;
      break;
    }
    case GVNode::RoundedRect:
    {
      QPainterPath path;
      path.addRoundedRect(rect, 10, 10);
      QGraphicsPathItem *item = new QGraphicsPathItem(this);
      item->setPath(path);
      m_shapeItem = item;
      break;
    }
    case GVNode::Ellipse:
    case GVNode::DoubleEllipse:
    {
      QGraphicsEllipseItem *item = new QGraphicsEllipseItem(this);
      item->setRect(rect);
      if (node.shape() == GVNode::DoubleEllipse) {
        QGraphicsEllipseItem *doubleItem = new QGraphicsEllipseItem(item);
        doubleItem->setRect(rect.adjusted(4, 4, -4, -4));
        m_shapeItem = doubleItem;
      } else {
        m_shapeItem = item;
      }
      break;
    }
    default:
      Q_ASSERT(false);
  }

  m_textItem = new QGraphicsTextItem(node.name(), this);
  m_textItem->setFont(node.m_font);

  setPos(m_node.centerPos());
  setToolTip(QObject::tr("State: %1").arg(node.name()));
  setFlags(ItemIsSelectable);

  // init text item child
  {
    QGraphicsTextItem *item = m_textItem;
    QRectF textRect = item->boundingRect();
    while (size.width() < textRect.size().width() && item->font().pointSize() > 1) {
      QFont font = item->font();
      font.setPointSize(font.pointSize() - 1);
      item->setFont(font);
      textRect = item->boundingRect();
    }
    const QSizeF size = textRect.size();
    item->setPos(QPointF(-size.width() / 2, -size.height() / 2));
  }

  if (node.fillColor().isValid()) {
    setBrush(node.fillColor());
  }
}

void GVNodeItem::setPen(const QPen &pen)
{
  m_shapeItem->setPen(pen);
}

QBrush GVNodeItem::brush() const
{
  return m_shapeItem->brush();
}

void GVNodeItem::setBrush(const QBrush &brush)
{
  m_shapeItem->setBrush(brush);
}

GVEdgeItem::GVEdgeItem(const GVEdge &edge, QGraphicsItem *parent)
  : QGraphicsItemGroup(parent),
    m_edge(edge)
{
  m_pathItem = new QGraphicsPathItem(this);
  m_pathItem->setPath(edge.m_path);
  setToolTip(QObject::tr("Transition: %1 -> %2").arg(edge.m_source).arg(edge.m_target));

  // arrow head quick-fix
  QVector<QPointF> points = QVector<QPointF>() << QPointF(0, 0)
                                               << QPointF(-8, 4)
                                               << QPointF(-8, -4);
  m_arrowItem = new QGraphicsPolygonItem(this);
  m_arrowItem->setPolygon(QPolygonF(points));
  m_arrowItem->setPos(edge.m_path.pointAtPercent(1.0));
  m_arrowItem->setRotation(-edge.m_path.angleAtPercent(1.0));

  setPen(m_pathItem->pen());

  m_textItem = new QGraphicsTextItem(edge.m_label, this);
  m_textItem->setFont(edge.m_font);
  // init text item child
  {
    QGraphicsTextItem *item = m_textItem;
    const QRectF boundingRect = item->mapRectFromScene(edge.m_labelBoundingRect);
    const QSizeF size = boundingRect.size();
    QRectF textRect = item->boundingRect();
    while (size.width() < textRect.size().width() && item->font().pointSize() > 1) {
      QFont font = item->font();
      font.setPointSize(font.pointSize() - 1);
      item->setFont(font);
      textRect = item->boundingRect();
    }
    item->setPos(boundingRect.x(), boundingRect.y());
  }
}

void GVEdgeItem::setPen(const QPen &pen)
{
  m_pathItem->setPen(pen);
  QPen arrowPen(pen);
  arrowPen.setStyle(Qt::SolidLine);
  m_arrowItem->setPen(arrowPen);
  m_arrowItem->setBrush(arrowPen.color());
}

GVGraphItem::GVGraphItem(const GVSubGraph &graph, QGraphicsItem *parent)
  : QGraphicsPathItem(parent),
    m_graph(graph),
    m_textItem(0)
{
  setZValue(-1);

  setPath(graph.path());
  setPen(QColor(Qt::gray));
  setBrush(QColor(100, 100, 100, 20));
  setToolTip(QObject::tr("Graph: %1").arg(graph.name()));

  {
    // TODO: Implement text item
  }
}
