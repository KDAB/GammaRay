/*
  graphicsview.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
