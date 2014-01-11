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

#ifndef GAMMARAY_GVGRAPHITEMS_H
#define GAMMARAY_GVGRAPHITEMS_H

#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>

#include "gvgraph.h"

namespace GammaRay {

class GVNodeItem : public QGraphicsItemGroup
{
  public:
    explicit GVNodeItem(const GVNode &node, QGraphicsItem *parent = 0);

    enum {
      Type = UserType + 1
    };

    inline int type() const
    {
      return Type;
    }

    QGraphicsTextItem *textItem() const
    {
      return m_textItem;
    }

    void setPen(const QPen &pen);
    QBrush brush() const;
    void setBrush(const QBrush &brush);

  private:
    GVNode m_node;
    QGraphicsTextItem *m_textItem;
    QAbstractGraphicsShapeItem *m_shapeItem;
};

class GVEdgeItem : public QGraphicsItemGroup
{
  public:
    explicit GVEdgeItem(const GVEdge &edge, QGraphicsItem *parent = 0);

    enum {
      Type = UserType + 2
    };

    inline int type() const {
      return Type;
    }

    QGraphicsTextItem *textItem() const
    {
      return m_textItem;
    }

    void setPen(const QPen &pen);

  private:
    GVEdge m_edge;
    QGraphicsPathItem *m_pathItem;
    QGraphicsPolygonItem *m_arrowItem;
    QGraphicsTextItem *m_textItem;
};

class GVGraphItem : public QGraphicsPathItem
{
  public:
    explicit GVGraphItem(const GVSubGraph &edge, QGraphicsItem *parent = 0);

    enum {
      Type = UserType + 3
    };

    inline int type() const
    {
      return Type;
    }

    QGraphicsTextItem *textItem() const
    {
      return m_textItem;
    }

  private:
    GVSubGraph m_graph;
    QGraphicsTextItem *m_textItem;
};

}

#endif // GAMMARAY_GVGRAPHITEMS_H
