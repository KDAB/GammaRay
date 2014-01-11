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

#include "statemachineview.h"

#include <QWheelEvent>

using namespace GammaRay;

StateMachineView::StateMachineView(QWidget *parent)
  : QGraphicsView(parent)
{
}

StateMachineView::StateMachineView(QGraphicsScene *scene, QWidget *parent)
  : QGraphicsView(scene, parent)
{
}

void StateMachineView::zoomBy(qreal scaleFactor)
{
  scale(scaleFactor, scaleFactor);
}

void StateMachineView::wheelEvent(QWheelEvent *event)
{
  const qreal step = qAbs(event->delta() / 100.0);
  const qreal zoomFactor = event->delta() >= 0 ? step : 1.0f / step ;
  zoomBy(zoomFactor);
  event->accept();
}
