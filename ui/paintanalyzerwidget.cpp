/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "paintanalyzerwidget.h"

#include "ui_paintanalyzerwidget.h"

#include <common/paintanalyzerinterface.h>
#include <common/objectbroker.h>

#include <QComboBox>
#include <QDebug>
#include <QLabel>
#include <QToolBar>

using namespace GammaRay;

PaintAnalyzerWidget::PaintAnalyzerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PaintAnalyzerWidget)
{
    ui->setupUi(this);
    ui->commandView->header()->setObjectName("commandViewHeader");

    auto toolbar = new QToolBar;
    // Our icons are 16x16 and support hidpi, so let force iconSize on every styles
    toolbar->setIconSize(QSize(16, 16));
    toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    ui->replayContainer->setMenuBar(toolbar);

    foreach (auto action, ui->replayWidget->interactionModeActions()->actions())
        toolbar->addAction(action);
    toolbar->addSeparator();

    toolbar->addAction(ui->replayWidget->zoomOutAction());
    auto zoom = new QComboBox;
    zoom->setModel(ui->replayWidget->zoomLevelModel());
    toolbar->addWidget(zoom);
    toolbar->addAction(ui->replayWidget->zoomInAction());

    ui->replayWidget->setSupportedInteractionModes(
        RemoteViewWidget::ViewInteraction | RemoteViewWidget::Measuring | RemoteViewWidget::ColorPicking);

    ui->paintAnalyzerSplitter->setStretchFactor(0, 0);
    ui->paintAnalyzerSplitter->setStretchFactor(1, 1);

    connect(zoom, SIGNAL(currentIndexChanged(int)), ui->replayWidget, SLOT(setZoomLevel(int)));
    connect(ui->replayWidget, SIGNAL(zoomLevelChanged(int)), zoom, SLOT(setCurrentIndex(int)));
    zoom->setCurrentIndex(ui->replayWidget->zoomLevelIndex());
}

PaintAnalyzerWidget::~PaintAnalyzerWidget()
{
}

void PaintAnalyzerWidget::setBaseName(const QString &name)
{
    auto model = ObjectBroker::model(name + QStringLiteral(".paintBufferModel"));
    ui->commandView->setModel(model);
    ui->commandView->setSelectionModel(ObjectBroker::selectionModel(ui->commandView->model()));

    ui->replayWidget->setName(name + QStringLiteral(".remoteView"));
}
