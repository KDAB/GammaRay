/*
  qmllistpropertyadaptor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "qmllistpropertyadaptor.h"

#include <core/propertydata.h>

#include <QQmlListProperty>

using namespace GammaRay;

QmlListPropertyAdaptor::QmlListPropertyAdaptor(QObject *parent)
    : PropertyAdaptor(parent)
{
}

QmlListPropertyAdaptor::~QmlListPropertyAdaptor() = default;

int QmlListPropertyAdaptor::count() const
{
    auto var = object().variant(); // we need to keep that alive for the runtime of this method
    QQmlListProperty<QObject> *prop = reinterpret_cast<QQmlListProperty<QObject> *>(const_cast<void *>(var.data()));
    if (!prop || !prop->count)
        return 0;
    return prop->count(prop);
}

PropertyData QmlListPropertyAdaptor::propertyData(int index) const
{
    PropertyData pd;

    auto var = object().variant(); // we need to keep that alive for the runtime of this method
    QQmlListProperty<QObject> *prop = reinterpret_cast<QQmlListProperty<QObject> *>(const_cast<void *>(var.data()));
    if (!prop || !prop->at || !prop->count || index >= prop->count(prop))
        return pd;

    auto obj = prop->at(prop, index);
    pd.setName(QString::number(index));
    pd.setValue(QVariant::fromValue(obj));
    if (obj)
        pd.setTypeName(obj->metaObject()->className());
    pd.setClassName(var.typeName());
    return pd;
}

QmlListPropertyAdaptorFactory *QmlListPropertyAdaptorFactory::s_instance = nullptr;

PropertyAdaptor *QmlListPropertyAdaptorFactory::create(const ObjectInstance &oi,
                                                       QObject *parent) const
{
    if (oi.type() != ObjectInstance::QtVariant)
        return nullptr;

    if (!oi.variant().isValid() || qstrncmp(oi.typeName(), "QQmlListProperty<", 17) != 0)
        return nullptr;

    return new QmlListPropertyAdaptor(parent);
}

QmlListPropertyAdaptorFactory *QmlListPropertyAdaptorFactory::instance()
{
    if (!s_instance)
        s_instance = new QmlListPropertyAdaptorFactory;
    return s_instance;
}
