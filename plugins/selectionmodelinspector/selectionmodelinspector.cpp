/*
  selectionmodelinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#include "selectionmodelinspector.h"

#include <core/objecttypefilterproxymodel.h>
#include <core/probeinterface.h>

#include <common/objectbroker.h>

#include <QtPlugin>

#if QT_VERSION < QT_VERSION_CHECK(4, 8, 0)
#include <QSortFilterProxyModel>
typedef QSortFilterProxyModel QIdentityProxyModel;
#else
#include <QIdentityProxyModel>
#endif

using namespace GammaRay;

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(SelectionModelInspectorFactory)
#endif

SelectionModelInspector::SelectionModelInspector(ProbeInterface *probe, QObject *parent)
  : QObject(parent)
  , m_current(new QIdentityProxyModel(this))
{
  ObjectTypeFilterProxyModel<QItemSelectionModel> *selectionModelProxy =
    new ObjectTypeFilterProxyModel<QItemSelectionModel>(this);
  selectionModelProxy->setSourceModel(probe->objectListModel());
  probe->registerModel("com.kdab.GammaRay.SelectionModelsModel", selectionModelProxy);

  QItemSelectionModel *selectionModel = ObjectBroker::selectionModel(selectionModelProxy);
  connect(selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)),
          SLOT(currentChanged(QModelIndex)));

  probe->registerModel("com.kdab.GammaRay.CurrentSelectionModel", m_current);
}

void SelectionModelInspector::currentChanged(const QModelIndex &current)
{
  QObject *selectionModelObject = current.data(ObjectModel::ObjectRole).value<QObject*>();
  QItemSelectionModel *selectionModel = qobject_cast<QItemSelectionModel*>(selectionModelObject);
  if (selectionModel && selectionModel->model()) {
    m_current->setSourceModel(const_cast<QAbstractItemModel*>(selectionModel->model()));
  } else {
    m_current->setSourceModel(0);
  }
}
