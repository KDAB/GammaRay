/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <common/objectmodel.h>
#include <common/objectbroker.h>
#include <core/probeinterface.h>
#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/remote/serverproxymodel.h>

#include <QtPlugin>
#include <QGraphicsWidget>
#include <QItemSelectionModel>
#include <QMenu>

#include <iostream>

using namespace GammaRay;
using namespace std;

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_DECLARE_METATYPE(QAction *)
Q_DECLARE_METATYPE(QActionGroup *)
Q_DECLARE_METATYPE(QMenu *)
#endif

ActionInspector::ActionInspector(ProbeInterface *probe, QObject *parent)
    : QObject(parent)
{
    registerMetaTypes();
    ObjectBroker::registerObject(QStringLiteral("com.kdab.GammaRay.ActionInspector"), this);

    ActionModel *actionModel = new ActionModel(this);
    connect(probe->probe(), SIGNAL(objectCreated(QObject*)), actionModel,
            SLOT(objectAdded(QObject*)));
    connect(probe->probe(), SIGNAL(objectDestroyed(QObject*)), actionModel,
            SLOT(objectRemoved(QObject*)));
    connect(probe->probe(), SIGNAL(objectSelected(QObject*,QPoint)),
            SLOT(objectSelected(QObject*)));

    auto proxy = new ServerProxyModel<QSortFilterProxyModel>(this);
    proxy->setSourceModel(actionModel);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.ActionModel"), proxy);

    m_selectionModel = ObjectBroker::selectionModel(proxy);
}

ActionInspector::~ActionInspector()
{
}

void ActionInspector::triggerAction(int row)
{
    QAbstractItemModel *model
        = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ActionModel"));
    const QModelIndex index = model->index(row, 0);
    if (!index.isValid())
        return;

    QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject *>();
    QAction *action = qobject_cast<QAction *>(obj);

    if (action)
        action->trigger();
}

void GammaRay::ActionInspector::objectSelected(QObject *obj)
{
    QAction *action = qobject_cast<QAction *>(obj);
    if (!action)
        return;

    const QAbstractItemModel *model = m_selectionModel->model();

    const QModelIndexList indexList
        = model->match(model->index(0, 0),
                       ObjectModel::ObjectRole,
                       QVariant::fromValue<QAction *>(action), 1,
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
    MetaObject *mo = 0;
    MO_ADD_METAOBJECT1(QAction, QObject);
    MO_ADD_PROPERTY_RO(QAction, QActionGroup *, actionGroup);
    MO_ADD_PROPERTY_CR(QAction, QVariant, data, setData);
    MO_ADD_PROPERTY(QAction, bool, isSeparator, setSeparator);
    MO_ADD_PROPERTY_RO(QAction, QMenu *, menu);
    MO_ADD_PROPERTY_RO(QAction, QWidget *, parentWidget);
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    MO_ADD_PROPERTY_RO(QAction, QList<QGraphicsWidget *>, associatedGraphicsWidgets);
    MO_ADD_PROPERTY_RO(QAction, QList<QWidget *>, associatedWidgets);
#endif

    MO_ADD_METAOBJECT1(QActionGroup, QObject);
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    MO_ADD_PROPERTY_RO(QActionGroup, QList<QAction *>, actions);
#endif
}

QString ActionInspectorFactory::name() const
{
    return tr("Action Inspector");
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(ActionInspectorFactory)
#endif
