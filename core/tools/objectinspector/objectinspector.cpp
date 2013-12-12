/*
  objectinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "objectinspector.h"
#include "propertycontroller.h"
#include "probeinterface.h"

#include <common/objectbroker.h>
#include <common/objectmodel.h>

#include <QCoreApplication>
#include <QItemSelectionModel>

using namespace GammaRay;

ObjectInspector::ObjectInspector(ProbeInterface *probe, QObject *parent)
  : QObject(parent),
  m_propertyController(new PropertyController("com.kdab.GammaRay.ObjectInspector", this))
{
  m_selectionModel = ObjectBroker::selectionModel(ObjectBroker::model("com.kdab.GammaRay.ObjectTree"));

  connect(m_selectionModel,
          SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          SLOT(objectSelectionChanged(QItemSelection)));

  connect(probe->probe(), SIGNAL(objectSelected(QObject*,QPoint)), SLOT(objectSelected(QObject*)));

  // when we end up here the object model isn't populated yet
  QMetaObject::invokeMethod(this, "selectDefaultItem", Qt::QueuedConnection);
}

void ObjectInspector::selectDefaultItem()
{
  // select the qApp object (if any) in the object treeView
  const QAbstractItemModel *viewModel = m_selectionModel->model();
  const QModelIndexList matches = viewModel->match(viewModel->index(0, 0),
      ObjectModel::ObjectRole, QVariant::fromValue<QObject*>(qApp), 1,
      Qt::MatchFlags(Qt::MatchExactly|Qt::MatchRecursive));

  if (!matches.isEmpty()) {
    m_selectionModel->setCurrentIndex(matches.first(), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
  }
}

void ObjectInspector::objectSelectionChanged(const QItemSelection& selection)
{
  if (selection.isEmpty())
    objectSelected(QModelIndex());
  else
    objectSelected(selection.first().topLeft());
}

void ObjectInspector::objectSelected(const QModelIndex &index)
{
  if (index.isValid()) {
    QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject*>();
    m_propertyController->setObject(obj);
  } else {
    m_propertyController->setObject(0);
  }
}

void ObjectInspector::objectSelected(QObject *object)
{
  const QAbstractItemModel *model = m_selectionModel->model();
  const QModelIndexList indexList =
  model->match(model->index(0, 0),
               ObjectModel::ObjectRole,
               QVariant::fromValue<QObject*>(object), 1,
               Qt::MatchExactly | Qt::MatchRecursive);
  if (indexList.isEmpty()) {
    return;
  }

  const QModelIndex index = indexList.first();
  m_selectionModel->select(
    index,
    QItemSelectionModel::Select | QItemSelectionModel::Clear |
    QItemSelectionModel::Rows | QItemSelectionModel::Current);
  // TODO: move this to the client side!
  //ui->objectTreeView->scrollTo(index);
  objectSelected(index);
}

