/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#include "actioninspector.h"
#include "actionmodel.h"

#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/remote/serverproxymodel.h>

#include <common/objectmodel.h>
#include <common/objectbroker.h>
#include <common/objectid.h>

#include <QActionGroup>
#include <QtPlugin>
#include <QGraphicsWidget>
#include <QItemSelectionModel>
#include <QMenu>

#include <iostream>

using namespace GammaRay;
using namespace std;

ActionInspector::ActionInspector(Probe *probe, QObject *parent)
    : QObject(parent)
{
    registerMetaTypes();
    ObjectBroker::registerObject(QStringLiteral("com.kdab.GammaRay.ActionInspector"), this);

    auto *actionModel = new ActionModel(this);
    connect(probe, &Probe::objectCreated, actionModel,
            &ActionModel::objectAdded);
    connect(probe, &Probe::objectDestroyed, actionModel,
            &ActionModel::objectRemoved);
    connect(probe, &Probe::objectSelected,
            this, &ActionInspector::objectSelected);

    auto proxy = new ServerProxyModel<QSortFilterProxyModel>(this);
    proxy->setSourceModel(actionModel);
    proxy->addRole(ActionModel::ObjectIdRole);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.ActionModel"), proxy);

    m_selectionModel = ObjectBroker::selectionModel(proxy);
}

ActionInspector::~ActionInspector() = default;

void ActionInspector::triggerAction(int row)
{
    auto model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ActionModel"));
    const QModelIndex index = model->index(row, 0);
    if (!index.isValid())
        return;

    QObject *obj = index.data(ActionModel::ObjectRole).value<QObject*>();
    QAction *action = qobject_cast<QAction*>(obj);

    if (action)
        action->trigger();
}

void GammaRay::ActionInspector::objectSelected(QObject *obj)
{
    QAction *action = qobject_cast<QAction *>(obj);
    if (!action)
        return;

    const QAbstractItemModel *model = m_selectionModel->model();

    const auto indexList = model->match(model->index(0, 0),
                       ActionModel::ObjectIdRole,
                       QVariant::fromValue(ObjectId(action)), 1,
                       Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap);
    if (indexList.isEmpty())
        return;

    const QModelIndex index = indexList.first();
    m_selectionModel->select(index,
                             QItemSelectionModel::Select
                             |QItemSelectionModel::Clear
                             |QItemSelectionModel::Rows
                             |QItemSelectionModel::Current);
}

void ActionInspector::registerMetaTypes()
{
    MetaObject *mo = nullptr;
    MO_ADD_METAOBJECT1(QAction, QObject);
    MO_ADD_PROPERTY_RO(QAction, actionGroup);
    MO_ADD_PROPERTY(QAction, data, setData);
    MO_ADD_PROPERTY(QAction, isSeparator, setSeparator);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    MO_ADD_PROPERTY_RO(QAction, menu);
    MO_ADD_PROPERTY_RO(QAction, parentWidget);
    MO_ADD_PROPERTY_RO(QAction, associatedGraphicsWidgets);
    MO_ADD_PROPERTY_RO(QAction, associatedWidgets);
#else
    MO_ADD_PROPERTY_RO(QAction, associatedObjects);
#endif

    MO_ADD_METAOBJECT1(QActionGroup, QObject);
    MO_ADD_PROPERTY_RO(QActionGroup, actions);
}
