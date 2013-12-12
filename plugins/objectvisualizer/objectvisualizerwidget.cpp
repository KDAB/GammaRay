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

#include "objectvisualizerwidget.h"
#include "vtkcontainer.h"
#include "vtkpanel.h"
#include "vtkwidget.h"

#include "kde/kfilterproxysearchline.h"
#include "kde/krecursivefilterproxymodel.h"
#include <common/objectbroker.h>
#include <common/objectmodel.h>

#include <QCoreApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QStateMachine>
#include <QTreeView>

using namespace GammaRay;

GraphViewerWidget::GraphViewerWidget(QWidget *parent)
  : QWidget(parent),
    mWidget(new GraphWidget(this))
{
  mModel = ObjectBroker::model("com.kdab.GammaRay.ObjectVisualizerModel");

  QSortFilterProxyModel *objectFilter = new KRecursiveFilterProxyModel(this);
  objectFilter->setSourceModel(mModel);
  objectFilter->setDynamicSortFilter(true);

  QVBoxLayout *vbox = new QVBoxLayout;
  KFilterProxySearchLine *objectSearchLine = new KFilterProxySearchLine(this);
  objectSearchLine->setProxy(objectFilter);
  vbox->addWidget(objectSearchLine);
  QTreeView *objectTreeView = new QTreeView(this);
  objectTreeView->setModel(objectFilter);
  objectTreeView->setSortingEnabled(true);
  vbox->addWidget(objectTreeView);

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

GraphViewerWidget::~GraphViewerWidget()
{
}

void GraphViewerWidget::delayedInit()
{
  // make all existing objects known to the vtk widget
  mWidget->vtkWidget()->setModel(mModel);
  mWidget->vtkWidget()->setSelectionModel(mObjectTreeView->selectionModel());

  /// FIXME: This won't work for remote clients!
  // select the qApp object (if any) in the object treeView
  const QAbstractItemModel *viewModel = mObjectTreeView->model();
  const QModelIndexList matches = viewModel->match(viewModel->index(0, 0),
      ObjectModel::ObjectRole, QVariant::fromValue<QObject*>(qApp), 1,
      Qt::MatchFlags(Qt::MatchExactly|Qt::MatchRecursive));

  if (!matches.isEmpty()) {
    Q_ASSERT(matches.first().data(ObjectModel::ObjectRole).value<QObject*>() == qApp);
    mObjectTreeView->setCurrentIndex(matches.first());
  }
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(ObjectVisualizerUiFactory)
#endif
