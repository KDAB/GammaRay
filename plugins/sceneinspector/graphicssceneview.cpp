/*
  graphicssceneview.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    ui->sceneCoordLabel->setFixedWidth(fm.horizontalAdvance(QStringLiteral("00000.00 x 00000.00")));
    ui->itemCoordLabel->setFixedWidth(fm.horizontalAdvance(QStringLiteral("00000.00 x 00000.00")));
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
    ui->sceneCoordLabel->setText(QStringLiteral("%1 x %2").arg(coord.x(), 0, 'f', 2).arg(coord.y(), 0, 'f', 2));
}

void GraphicsSceneView::itemCoordinatesChanged(const QPointF &coord)
{
    ui->itemCoordLabel->setText(QStringLiteral("%1 x %2").arg(coord.x(), 0, 'f', 2).arg(coord.y(), 0, 'f', 2));
}
