/*
  sceneinspectorinterface.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

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

#include "sceneinspectorinterface.h"

#include <common/objectbroker.h>

#include <QGraphicsItem>
#include <QPainter>

using namespace GammaRay;

SceneInspectorInterface::SceneInspectorInterface(QObject *parent)
    : QObject(parent)
{
    ObjectBroker::registerObject<SceneInspectorInterface *>(this);
}

SceneInspectorInterface::~SceneInspectorInterface() = default;

void SceneInspectorInterface::paintItemDecoration(QGraphicsItem *item, const QTransform &transform,
                                                  QPainter *painter)
{
    const QRectF itemBoundingRect = item->boundingRect();
    // coord system, TODO: nicer axis with arrows, tics, markers for current mouse position etc.
    painter->setPen(Qt::black);
    const qreal maxX = qMax(qAbs(itemBoundingRect.left()), qAbs(itemBoundingRect.right()));
    const qreal maxY = qMax(qAbs(itemBoundingRect.top()), qAbs(itemBoundingRect.bottom()));
    const qreal maxXY = qMax(maxX, maxY) * 1.5f;
    painter->drawLine(item->mapToScene(-maxXY, 0), item->mapToScene(maxXY, 0));
    painter->drawLine(item->mapToScene(0, -maxXY), item->mapToScene(0, maxXY));

    painter->setPen(Qt::blue);
    const QPolygonF boundingBox = item->mapToScene(itemBoundingRect);
    painter->drawPolygon(boundingBox);

    painter->setPen(Qt::green);
    const QPainterPath shape = item->mapToScene(item->shape());
    painter->drawPath(shape);

    painter->setPen(Qt::red);
    const QPointF transformOrigin
        = item->mapToScene(item->transformOriginPoint());
    painter->drawEllipse(transformOrigin,
                         5.0 / transform.m11(),
                         5.0 / transform.m22());
}
