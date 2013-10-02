/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "objectvisualizer.h"
#include "objectvisualizermodel.h"

#include <QtPlugin>
#include <QLabel>

using namespace GammaRay;

GraphViewer::GraphViewer(ProbeInterface *probe, QObject *parent)
  : QObject(parent)
{
  ObjectVisualizerModel *model = new ObjectVisualizerModel(this);
  model->setSourceModel(probe->objectTreeModel());
  probe->registerModel("com.kdab.GammaRay.ObjectVisualizerModel", model);
}

GraphViewer::~GraphViewer()
{
}

#ifndef VTKGUI_ENABLED
QWidget *GraphViewerFactory::createWidget(QWidget *parentWidget)
{
  ///FIXME: disable selection of the tool, should be possible once we have a proper separation
  /// between server and client. Only the Client needs to be built with VTK support.
  QLabel *label = new QLabel(parentWidget);
  label->setAlignment(Qt::AlignCenter);
  label->setText(tr("GammaRay was built without VTK support. Object visualization is disabled."));
  return label;
}
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(GraphViewerFactory)
#endif

#include "objectvisualizer.moc"
