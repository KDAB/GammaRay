/*
  graphicssceneview.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
    ui->sceneCoordLabel->setFixedWidth(fm.width(QStringLiteral("00000.00 x 00000.00")));
    ui->itemCoordLabel->setFixedWidth(fm.width(QStringLiteral("00000.00 x 00000.00")));
#else
    ui->sceneCoordLabel->setFixedWidth(fm.horizontalAdvance(QStringLiteral("00000.00 x 00000.00")));
    ui->itemCoordLabel->setFixedWidth(fm.horizontalAdvance(QStringLiteral("00000.00 x 00000.00")));
#endif
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
