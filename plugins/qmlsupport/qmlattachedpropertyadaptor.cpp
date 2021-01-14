/*
  qmlattachedpropertyadaptor.cpp

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

#include "qmlattachedpropertyadaptor.h"
#include "qmltypeutil.h"

#include <core/propertydata.h>

#include <QDebug>
#include <private/qqmldata_p.h>
#include <private/qqmlmetatype_p.h>

using namespace GammaRay;

QmlAttachedPropertyAdaptor::QmlAttachedPropertyAdaptor(QObject *parent)
    : PropertyAdaptor(parent)
{
}

QmlAttachedPropertyAdaptor::~QmlAttachedPropertyAdaptor() = default;

void QmlAttachedPropertyAdaptor::doSetObject(const ObjectInstance &oi)
{
    auto data = QQmlData::get(oi.qtObject());
    Q_ASSERT(data);
    Q_ASSERT(data->hasExtendedData());
    Q_ASSERT(data->attachedProperties());

    m_attachedTypes.reserve(data->attachedProperties()->size());
    for (auto it = data->attachedProperties()->constBegin();
         it != data->attachedProperties()->constEnd(); ++it)
        m_attachedTypes.push_back(it.key());
}

int QmlAttachedPropertyAdaptor::count() const
{
    return m_attachedTypes.size();
}

PropertyData QmlAttachedPropertyAdaptor::propertyData(int index) const
{
    PropertyData pd;
    if (!object().isValid())
        return pd;

    Q_ASSERT(index >= 0);
    Q_ASSERT(index < m_attachedTypes.size());

    auto data = QQmlData::get(object().qtObject());
    if (!data)
        return pd;

    auto it = data->attachedProperties()->constFind(m_attachedTypes.at(index));
    if (it == data->attachedProperties()->constEnd())
        return pd;

    auto qmlType = QQmlMetaType::qmlType(it.value()->metaObject());
    if (QmlType::isValid(qmlType) && !QmlType::callable(qmlType)->elementName().isEmpty())
        pd.setName(QmlType::callable(qmlType)->elementName());
    else
        pd.setName(it.value()->metaObject()->className());

    pd.setValue(QVariant::fromValue(it.value()));
    pd.setClassName(it.value()->metaObject()->className());

    return pd;
}

QmlAttachedPropertyAdaptorFactory *QmlAttachedPropertyAdaptorFactory::s_instance = nullptr;

PropertyAdaptor *QmlAttachedPropertyAdaptorFactory::create(const ObjectInstance &oi,
                                                           QObject *parent) const
{
    if (oi.type() != ObjectInstance::QtObject || !oi.qtObject())
        return nullptr;

    auto data = QQmlData::get(oi.qtObject());
    if (!data || !data->hasExtendedData() || !data->attachedProperties()
        || data->attachedProperties()->isEmpty())
        return nullptr;

    return new QmlAttachedPropertyAdaptor(parent);
}

QmlAttachedPropertyAdaptorFactory *QmlAttachedPropertyAdaptorFactory::instance()
{
    if (!s_instance)
        s_instance = new QmlAttachedPropertyAdaptorFactory;
    return s_instance;
}
