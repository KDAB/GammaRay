/*
  graphicsview.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>
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

#include "graphicsview.h"
#include "sceneinspectorinterface.h"

#include <QGraphicsItem>
#include <QKeyEvent>

using namespace GammaRay;

GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView(parent)
    , m_currentItem(nullptr)
{
    setMouseTracking(true);
}

void GraphicsView::showItem(QGraphicsItem *item)
{
    m_currentItem = item;
    if (!item)
        return;

    fitInView(item, Qt::KeepAspectRatio);
    scale(0.8f, 0.8f);
    emit transformChanged();
}

void GraphicsView::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == Qt::CTRL) {
        switch (event->key()) {
        case Qt::Key_Plus:
            scale(1.2, 1.2);
            emit transformChanged();
            event->accept();
            return;
        case Qt::Key_Minus:
            scale(0.8, 0.8);
            emit transformChanged();
            event->accept();
            return;
        case Qt::Key_Left:
            rotate(-5);
            emit transformChanged();
            event->accept();
            break;
        case Qt::Key_Right:
            rotate(5);
            emit transformChanged();
            event->accept();
            break;
        }
    }
    QGraphicsView::keyPressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    emit sceneCoordinatesChanged(mapToScene(event->pos()));
    if (m_currentItem)
        emit itemCoordinatesChanged(m_currentItem->mapFromScene(mapToScene(event->pos())));
    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::drawForeground(QPainter *painter, const QRectF &rect)
{
    QGraphicsView::drawForeground(painter, rect);
    if (m_currentItem)
        SceneInspectorInterface::paintItemDecoration(m_currentItem, transform(), painter);
}
