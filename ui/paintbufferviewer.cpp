/*
  paintbufferviewer.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "paintbufferviewer.h"

#include "ui_paintbufferviewer.h"

#include <common/paintanalyzerinterface.h>
#include <common/objectbroker.h>

#include <QDebug>

using namespace GammaRay;

PaintBufferViewer::PaintBufferViewer(const QString &name, QWidget *parent)
  : QDialog(parent)
  , ui(new Ui::PaintBufferViewer)
{
  ui->setupUi(this);

  setWindowTitle(tr("Analyze Painting"));
  setAttribute(Qt::WA_DeleteOnClose);
  setModal(true);

  auto model = ObjectBroker::model(name + QStringLiteral(".paintBufferModel"));
  ui->commandView->setModel(model);
  ui->commandView->setSelectionModel(ObjectBroker::selectionModel(ui->commandView->model()));

  ui->zoom->setModel(ui->replayWidget->zoomLevelModel());
  ui->replayWidget->setSupportedInteractionModes(RemoteViewWidget::ViewInteraction | RemoteViewWidget::Measuring);

  ui->splitter->setStretchFactor(0, 0);
  ui->splitter->setStretchFactor(1, 1);

  auto controller = ObjectBroker::object<PaintAnalyzerInterface*>(name);
  connect(controller, SIGNAL(paintingAnalyzed(QImage)), ui->replayWidget, SLOT(setImage(QImage)));
  connect(ui->zoom, SIGNAL(currentIndexChanged(int)), ui->replayWidget, SLOT(setZoomLevel(int)));
  connect(ui->replayWidget, SIGNAL(zoomLevelChanged(int)), ui->zoom, SLOT(setCurrentIndex(int)));
  ui->zoom->setCurrentIndex(ui->replayWidget->zoomLevelIndex());
}

PaintBufferViewer::~PaintBufferViewer()
{
}
