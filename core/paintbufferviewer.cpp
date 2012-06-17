/*
  paintbufferviewer.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "paintbufferviewer.h"

#ifdef HAVE_PRIVATE_QT_HEADERS
#include "ui_paintbufferviewer.h"

#include "paintbuffermodel.h"

using namespace GammaRay;

PaintBufferViewer::PaintBufferViewer(QWidget *parent)
  : QWidget(parent),
    ui(new Ui::PaintBufferViewer),
    m_bufferModel(new PaintBufferModel(this))
{
  ui->setupUi(this);

  ui->commandView->setModel(m_bufferModel);
  connect(ui->commandView->selectionModel(),
          SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          SLOT(commandSelected()));

  ui->splitter->setStretchFactor(0, 0);
  ui->splitter->setStretchFactor(1, 1);

  connect(ui->zoomSlider, SIGNAL(valueChanged(int)), SLOT(zoomChanged(int)));
}

PaintBufferViewer::~PaintBufferViewer()
{
}

void PaintBufferViewer::setPaintBuffer(const QPaintBuffer &buffer)
{
  m_buffer = buffer;
  m_bufferModel->setPaintBuffer(buffer);
  ui->replayWidget->setPaintBuffer(buffer);
  commandSelected();
}

void PaintBufferViewer::commandSelected()
{
  if (!ui->commandView->selectionModel()->hasSelection()) {
    ui->replayWidget->setEndCommandIndex(m_bufferModel->rowCount());
    return;
  }

  const QModelIndex index = ui->commandView->selectionModel()->selectedRows().first();
  ui->replayWidget->setEndCommandIndex(index.row() + 1); // include the current row
}

void PaintBufferViewer::zoomChanged(int value)
{
  ui->replayWidget->setZoomFactor(value);
}

#include "paintbufferviewer.moc"
#endif
