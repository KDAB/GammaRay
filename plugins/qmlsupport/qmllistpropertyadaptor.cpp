/*
  qmllistpropertyadaptor.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
    QQmlListProperty<QObject> *prop
        = reinterpret_cast<QQmlListProperty<QObject> *>(const_cast<void *>(var.data()));
    if (!prop || !prop->count)
        return 0;
    return prop->count(prop);
}

PropertyData QmlListPropertyAdaptor::propertyData(int index) const
{
    PropertyData pd;

    auto var = object().variant(); // we need to keep that alive for the runtime of this method
    QQmlListProperty<QObject> *prop
        = reinterpret_cast<QQmlListProperty<QObject> *>(const_cast<void *>(var.data()));
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
