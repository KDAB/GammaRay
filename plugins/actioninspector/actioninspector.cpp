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

#include "actioninspector.h"
#include "actionmodel.h"

#include "include/objectmodel.h"
#include "include/probeinterface.h"
#include "include/objecttypefilterproxymodel.h"

#include "kde/kfilterproxysearchline.h"
#include "kde/krecursivefilterproxymodel.h"

#include <QCoreApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QStateMachine>
#include <QTreeView>

#include <QtPlugin>

using namespace GammaRay;

ActionInspector::ActionInspector(ProbeInterface *probe, QWidget *parent)
  : QWidget(parent),
    mProbeIface(probe)
{
  ActionModel *actionFilterProxy = new ActionModel(this);
  actionFilterProxy->setSourceModel(probe->objectListModel());

  QSortFilterProxyModel *searchFilterProxy = new KRecursiveFilterProxyModel(this);
  searchFilterProxy->setSourceModel(actionFilterProxy);
  searchFilterProxy->setDynamicSortFilter(true);

  QVBoxLayout *vbox = new QVBoxLayout(this);

  KFilterProxySearchLine *objectSearchLine = new KFilterProxySearchLine(this);
  objectSearchLine->setProxy(searchFilterProxy);
  vbox->addWidget(objectSearchLine);

  QTreeView *objectTreeView = new QTreeView(this);
  objectTreeView->setModel(searchFilterProxy);
  objectTreeView->setSortingEnabled(true);
  objectTreeView->sortByColumn(ActionModel::ShortcutsPropColumn);
  objectTreeView->setRootIsDecorated(false);
  vbox->addWidget(objectTreeView);
  connect(objectTreeView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
          SLOT(handleRowChanged(QModelIndex)));
  connect(objectTreeView, SIGNAL(doubleClicked(QModelIndex)), SLOT(triggerAction(QModelIndex)));
  mObjectTreeView = objectTreeView;

  QMetaObject::invokeMethod(this, "delayedInit", Qt::QueuedConnection);
}

ActionInspector::~ActionInspector()
{
}

void ActionInspector::delayedInit()
{
  // select the qApp object (if any) in the object treeView
  const QAbstractItemModel *viewModel = mObjectTreeView->model();
  const QModelIndexList matches = viewModel->match(viewModel->index(0, 0),
                                                   ObjectModel::ObjectRole,
                                                   QVariant::fromValue<QObject*>(qApp));
  if (!matches.isEmpty()) {
    mObjectTreeView->setCurrentIndex(matches.first());
  }
}

void ActionInspector::handleRowChanged(const QModelIndex &index)
{
  Q_UNUSED(index);
  // TODO: Unused
}

void ActionInspector::triggerAction(const QModelIndex &index)
{
  if (!index.isValid()) {
    return;
  }

  QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject*>();
  QAction *action = qobject_cast<QAction*>(obj);

  if (action) {
    action->trigger();
  }
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(ActionInspectorFactory)
#endif

#include "actioninspector.moc"
