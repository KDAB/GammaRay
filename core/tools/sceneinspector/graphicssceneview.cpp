/*
  graphicssceneview.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "graphicssceneview.h"
#include "ui_graphicssceneview.h"

using namespace GammaRay;

GraphicsSceneView::GraphicsSceneView(QWidget *parent)
  : QWidget(parent),
    ui(new Ui::GraphicsSceneView)
{
  ui->setupUi(this);

  QFontMetrics fm(ui->sceneCoordLabel->font());
  ui->sceneCoordLabel->setFixedWidth(fm.width("00000.00 x 00000.00"));
  ui->itemCoordLabel->setFixedWidth(fm.width("00000.00 x 00000.00"));

  connect(ui->graphicsView, SIGNAL(sceneCoordinatesChanged(QPointF)),
          SLOT(sceneCoordinatesChanged(QPointF)));
  connect(ui->graphicsView, SIGNAL(itemCoordinatesChanged(QPointF)),
          SLOT(itemCoordinatesChanged(QPointF)));
}

GraphicsSceneView::~GraphicsSceneView()
{
  delete ui;
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
  ui->sceneCoordLabel->setText(QString::fromLatin1("%1 x %2").
                               arg(coord.x(), 0, 'f', 2).
                               arg(coord.y(), 0, 'f', 2));
}

void GraphicsSceneView::itemCoordinatesChanged(const QPointF &coord)
{
  ui->itemCoordLabel->setText(QString::fromLatin1("%1 x %2").
                              arg(coord.x(), 0, 'f', 2).
                              arg(coord.y(), 0, 'f', 2));
}

#include "graphicssceneview.moc"
