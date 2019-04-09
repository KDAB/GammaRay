/*
  graphicssceneview.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "graphicssceneview.h"
#include "ui_graphicssceneview.h"

using namespace GammaRay;

GraphicsSceneView::GraphicsSceneView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GraphicsSceneView)
{
    ui->setupUi(this);

    QFontMetrics fm(ui->sceneCoordLabel->font());
    ui->sceneCoordLabel->setFixedWidth(fm.width(QStringLiteral("00000.00 x 00000.00")));
    ui->itemCoordLabel->setFixedWidth(fm.width(QStringLiteral("00000.00 x 00000.00")));

    connect(ui->graphicsView, &GraphicsView::sceneCoordinatesChanged,
            this, &GraphicsSceneView::sceneCoordinatesChanged);
    connect(ui->graphicsView, &GraphicsView::itemCoordinatesChanged,
            this, &GraphicsSceneView::itemCoordinatesChanged);
}

GraphicsSceneView::~GraphicsSceneView()
{
    delete ui;
}

GraphicsView *GraphicsSceneView::view() const
{
    return ui->graphicsView;
}

void GraphicsSceneView::showGraphicsItem(QGraphicsItem *item)
{
    ui->graphicsView->showItem(item);
}

void GraphicsSceneView::setGraphicsScene(QGraphicsScene *scene)
{
    ui->graphicsView->setScene(scene);
}

void GraphicsSceneView::sceneCoordinatesChanged(const QPointF &coord)
{
    ui->sceneCoordLabel->setText(QStringLiteral("%1 x %2").
                                 arg(coord.x(), 0, 'f', 2).
                                 arg(coord.y(), 0, 'f', 2));
}

void GraphicsSceneView::itemCoordinatesChanged(const QPointF &coord)
{
    ui->itemCoordLabel->setText(QStringLiteral("%1 x %2").
                                arg(coord.x(), 0, 'f', 2).
                                arg(coord.y(), 0, 'f', 2));
}
