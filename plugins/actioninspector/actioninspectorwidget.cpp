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

#include "actioninspectorwidget.h"
#include "actionmodel.h" // for column enum only

#include <common/network/objectbroker.h>

#include "kde/kfilterproxysearchline.h"
#include "kde/krecursivefilterproxymodel.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QTreeView>

using namespace GammaRay;

ActionInspectorWidget::ActionInspectorWidget(QWidget *parent)
  : QWidget(parent)
{
  QAbstractItemModel *actionModel = ObjectBroker::model("com.kdab.GammaRay.ActionModel");

  QSortFilterProxyModel *searchFilterProxy = new KRecursiveFilterProxyModel(this);
  searchFilterProxy->setSourceModel(actionModel);
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
  connect(objectTreeView, SIGNAL(doubleClicked(QModelIndex)), SLOT(triggerAction(QModelIndex)));
  mObjectTreeView = objectTreeView;
}

ActionInspectorWidget::~ActionInspectorWidget()
{
}

// TODO this wont work remotely!
void ActionInspectorWidget::triggerAction(const QModelIndex &index)
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

#include "actioninspectorwidget.moc"
