/*
  objectbroker.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "objectbroker.h"
#include "endpoint.h"
#include "modelevent.h"

#include <kde/klinkitemselectionmodel.h>

#include <QHash>
#include <QString>
#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QAbstractProxyModel>
#include <QCoreApplication>
#include <QVector>

namespace GammaRay {
struct ObjectlBrokerData {
    ObjectlBrokerData() = default;
    QHash<QString, QObject *> objects;
    QHash<QString, QAbstractItemModel *> models;
    QHash<QAbstractItemModel *, QItemSelectionModel *> selectionModels;
    QHash<QByteArray, ObjectBroker::ClientObjectFactoryCallback> clientObjectFactories;
    ObjectBroker::ModelFactoryCallback modelCallback = nullptr;
    ObjectBroker::selectionModelFactoryCallback selectionCallback = nullptr;
    QVector<QObject *> ownedObjects;
};

Q_GLOBAL_STATIC(ObjectlBrokerData, s_objectBroker)

void ObjectBroker::registerObject(const QString &name, QObject *object)
{
    Q_ASSERT(!name.isEmpty());
    Q_ASSERT(object->objectName().isEmpty());
    object->setObjectName(name);

    Q_ASSERT(!s_objectBroker()->objects.contains(name));
    s_objectBroker()->objects.insert(name, object);

    Q_ASSERT(Endpoint::instance());
    Endpoint::instance()->registerObject(name, object);
}

bool ObjectBroker::hasObject(const QString &name)
{
    return s_objectBroker()->objects.contains(name);
}

QObject *ObjectBroker::objectInternal(const QString &name, const QByteArray &type)
{
    auto it = s_objectBroker()->objects.constFind(name);
    if (it != s_objectBroker()->objects.constEnd())
        return it.value();

    // Below here only valid for clients!
    // Remote/probe side should have registered the object directly
    QObject *obj = nullptr;

    if (!type.isEmpty()) {
        Q_ASSERT(s_objectBroker()->clientObjectFactories.contains(type));
        obj = s_objectBroker()->clientObjectFactories.value(type)(name, qApp);
    } else {
        // fallback
        obj = new QObject(qApp);
        registerObject(name, obj);
    }
    s_objectBroker()->ownedObjects.push_back(obj);

    Q_ASSERT(obj);
    // ensure it was registered
    Q_ASSERT_X(s_objectBroker()->objects.value(name, nullptr) == obj, Q_FUNC_INFO,
               qPrintable(QStringLiteral("Object %1 was not registered in the broker.").arg(name)));

    return obj;
}

void ObjectBroker::registerClientObjectFactoryCallbackInternal(const QByteArray &type,
                                                               ObjectBroker::ClientObjectFactoryCallback callback)
{
    Q_ASSERT(!type.isEmpty());
    s_objectBroker()->clientObjectFactories[type] = callback;
}

void ObjectBroker::registerModelInternal(const QString &name, QAbstractItemModel *model)
{
    Q_ASSERT(!s_objectBroker()->models.contains(name));
    model->setObjectName(name);
    s_objectBroker()->models.insert(name, model);
}

QAbstractItemModel *ObjectBroker::model(const QString &name)
{
    ModelEvent event(true);
    auto it = s_objectBroker()->models.constFind(name);
    if (it != s_objectBroker()->models.constEnd()) {
        QCoreApplication::sendEvent(it.value(), &event);
        return it.value();
    }

    if (s_objectBroker()->modelCallback) {
        QAbstractItemModel *model = s_objectBroker()->modelCallback(name);
        if (model) {
            model->setObjectName(name);
            s_objectBroker()->models.insert(name, model);
            s_objectBroker()->ownedObjects.push_back(model);
            QCoreApplication::sendEvent(model, &event);
            return model;
        }
    }
    return nullptr;
}

void ObjectBroker::setModelFactoryCallback(ObjectBroker::ModelFactoryCallback callback)
{
    s_objectBroker()->modelCallback = callback;
}

void ObjectBroker::registerSelectionModel(QItemSelectionModel *selectionModel)
{
    Q_ASSERT(!s_objectBroker()->selectionModels.contains(const_cast<QAbstractItemModel *>(
                                                             selectionModel->model())));
    s_objectBroker()->selectionModels.insert(
        const_cast<QAbstractItemModel *>(selectionModel->model()), selectionModel);
}

void ObjectBroker::unregisterSelectionModel(QItemSelectionModel *selectionModel)
{
    Q_ASSERT(s_objectBroker()->selectionModels.contains(const_cast<QAbstractItemModel *>(
                                                            selectionModel->model())));
    s_objectBroker()->selectionModels.remove(
        const_cast<QAbstractItemModel *>(selectionModel->model()));
}

bool ObjectBroker::hasSelectionModel(QAbstractItemModel *model)
{
    return s_objectBroker()->selectionModels.contains(model);
}

static QAbstractItemModel *sourceModelForProxy(QAbstractItemModel *model)
{
    // stop once we found a registered model, this is what network communication is based on
    if (s_objectBroker()->models.values().contains(model))
        return model;

    QAbstractProxyModel *proxy = qobject_cast<QAbstractProxyModel *>(model);
    if (!proxy)
        return model;
    return sourceModelForProxy(proxy->sourceModel());
}

QItemSelectionModel *ObjectBroker::selectionModel(QAbstractItemModel *model)
{
    auto it = s_objectBroker()->selectionModels.constFind(model);
    if (it != s_objectBroker()->selectionModels.constEnd())
        return it.value();

    if (s_objectBroker()->selectionCallback) {
        QAbstractItemModel *sourceModel = sourceModelForProxy(model);

        QItemSelectionModel *selectionModel = nullptr;
        if (sourceModel == model) {
            selectionModel = s_objectBroker()->selectionCallback(sourceModel);
            s_objectBroker()->ownedObjects.push_back(selectionModel);
        } else {
            QItemSelectionModel *sourceSelectionModel = ObjectBroker::selectionModel(sourceModel);
            selectionModel = new KLinkItemSelectionModel(model, sourceSelectionModel, model);
        }

        if (selectionModel) {
            registerSelectionModel(selectionModel);
            return selectionModel;
        }
    }
    return nullptr;
}

void ObjectBroker::setSelectionModelFactoryCallback(
    ObjectBroker::selectionModelFactoryCallback callback)
{
    s_objectBroker()->selectionCallback = callback;
}

void ObjectBroker::clear()
{
    auto *ob = s_objectBroker();
    qDeleteAll(ob->ownedObjects);
    ob->ownedObjects.clear();
    ob->objects.clear();
    ob->models.clear();
    ob->selectionModels.clear();
}
}
