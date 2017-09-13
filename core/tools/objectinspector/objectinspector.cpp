/*
  objectinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "objectinspector.h"
#include "propertycontroller.h"
#include "probeinterface.h"
#include "methodsextension.h"
#include "classinfoextension.h"
#include "enumsextension.h"
#include "propertiesextension.h"
#include "connectionsextension.h"
#include "applicationattributeextension.h"
#include "../bindinginspector/bindingextension.h"

#include <common/objectbroker.h>
#include <common/objectmodel.h>
#include <remote/serverproxymodel.h>

#include <3rdparty/kde/krecursivefilterproxymodel.h>

#include <QCoreApplication>
#include <QItemSelectionModel>

using namespace GammaRay;

ObjectInspector::ObjectInspector(ProbeInterface *probe, QObject *parent)
    : QObject(parent)
{
    registerPCExtensions();

    m_propertyController = new PropertyController(QStringLiteral(
                                                      "com.kdab.GammaRay.ObjectInspector"), this);

    auto proxy = new ServerProxyModel<KRecursiveFilterProxyModel>(this);
    proxy->setSourceModel(probe->objectTreeModel());
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.ObjectInspectorTree"), proxy);

    m_selectionModel = ObjectBroker::selectionModel(proxy);

    connect(m_selectionModel,
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(objectSelectionChanged(QItemSelection)));

    connect(probe->probe(), SIGNAL(objectSelected(QObject*,QPoint)),
            SLOT(objectSelected(QObject*)));
}

void ObjectInspector::objectSelectionChanged(const QItemSelection &selection)
{
    if (selection.isEmpty())
        objectSelected(QModelIndex());
    else
        objectSelected(selection.first().topLeft());
}

void ObjectInspector::objectSelected(const QModelIndex &index)
{
    if (index.isValid()) {
        QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject *>();
        m_propertyController->setObject(obj);
    } else {
        m_propertyController->setObject(nullptr);
    }
}

void ObjectInspector::objectSelected(QObject *object)
{
    const QAbstractItemModel *model = m_selectionModel->model();
    const QModelIndexList indexList
        = model->match(model->index(0, 0),
                       ObjectModel::ObjectRole,
                       QVariant::fromValue<QObject *>(object), 1,
                       Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap);
    if (indexList.isEmpty())
        return;

    const QModelIndex index = indexList.first();
    m_selectionModel->select(
        index,
        QItemSelectionModel::Select | QItemSelectionModel::Clear
        |QItemSelectionModel::Rows | QItemSelectionModel::Current);
    // TODO: move this to the client side!
    // ui->objectTreeView->scrollTo(index);
    objectSelected(index);
}

void ObjectInspector::registerPCExtensions()
{
    PropertyController::registerExtension<ClassInfoExtension>();
    PropertyController::registerExtension<MethodsExtension>();
    PropertyController::registerExtension<EnumsExtension>();
    PropertyController::registerExtension<PropertiesExtension>();
    PropertyController::registerExtension<ConnectionsExtension>();
    PropertyController::registerExtension<ApplicationAttributeExtension>();
    PropertyController::registerExtension<BindingExtension>();
}

QVector<QByteArray> GammaRay::ObjectInspectorFactory::selectableTypes() const
{
    return QVector<QByteArray>() << QObject::staticMetaObject.className();
}
