/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "vtkcontainer.h"
#include "vtkpanel.h"
#include "vtkwidget.h"

#include "include/objectmodel.h"
#include "include/probeinterface.h"

#include "kde/kfilterproxysearchline.h"
#include "kde/krecursivefilterproxymodel.h"

#include <QCoreApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QStateMachine>
#include <QTreeView>

#include <QtPlugin>

using namespace GammaRay;

GraphViewer::GraphViewer(ProbeInterface *probe, QWidget *parent)
  : QWidget(parent),
    mWidget(new GraphWidget(this)),
    mProbeIface(probe)
{
  QSortFilterProxyModel *objectFilter = new KRecursiveFilterProxyModel(this);
  objectFilter->setSourceModel(probe->objectTreeModel());
  objectFilter->setDynamicSortFilter(true);

  QVBoxLayout *vbox = new QVBoxLayout;
  KFilterProxySearchLine *objectSearchLine = new KFilterProxySearchLine(this);
  objectSearchLine->setProxy(objectFilter);
  vbox->addWidget(objectSearchLine);
  QTreeView *objectTreeView = new QTreeView(this);
  objectTreeView->setModel(objectFilter);
  objectTreeView->setSortingEnabled(true);
  vbox->addWidget(objectTreeView);
  connect(objectTreeView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
          SLOT(handleRowChanged(QModelIndex)));
  mObjectTreeView = objectTreeView;

  QWidget *treeViewWidget = new QWidget(this);
  treeViewWidget->setLayout(vbox);

  QSplitter *splitter = new QSplitter(this);
  splitter->addWidget(treeViewWidget);
  splitter->addWidget(mWidget);
  QHBoxLayout *hbox = new QHBoxLayout(this);
  hbox->addWidget(splitter);

  QMetaObject::invokeMethod(this, "delayedInit", Qt::QueuedConnection);
}

GraphViewer::~GraphViewer()
{
}

void GraphViewer::delayedInit()
{
  // make all existing objects known to the vtk widget
  const QAbstractItemModel *listModel = mProbeIface->objectListModel();
  for (int i = 0; i < listModel->rowCount(); ++i) {
    const QModelIndex index = listModel->index(i, 0);
    QObject *object = index.data(ObjectModel::ObjectRole).value<QObject*>();
    Q_ASSERT(object);
    mWidget->vtkWidget()->addObject(object);
  }
  connect(mProbeIface->probe(), SIGNAL(objectCreated(QObject*)),
          mWidget->vtkWidget(), SLOT(addObject(QObject*)));
  connect(mProbeIface->probe(), SIGNAL(objectDestroyed(QObject*)),
          mWidget->vtkWidget(), SLOT(removeObject(QObject*)));

  // select the qApp object (if any) in the object treeView
  const QAbstractItemModel *viewModel = mObjectTreeView->model();
  const QModelIndexList matches = viewModel->match(viewModel->index(0, 0),
                                                   ObjectModel::ObjectRole,
                                                   QVariant::fromValue<QObject*>(qApp));
  if (!matches.isEmpty()) {
    mObjectTreeView->setCurrentIndex(matches.first());
  }
}

void GraphViewer::handleRowChanged(const QModelIndex &index)
{
  QObject *object = index.data(ObjectModel::ObjectRole).value<QObject*>();
  Q_ASSERT(object);
  mWidget->vtkWidget()->setObjectFilter(object);
}

Q_EXPORT_PLUGIN(GraphViewerFactory)

#include "objectvisualizer.moc"
