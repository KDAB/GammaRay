/*
  metapropertyadaptor.cpp

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

#include "metapropertyadaptor.h"
#include "objectinstance.h"
#include "metaobjectrepository.h"
#include "metaobject.h"
#include "propertydata.h"

#include <QDebug>

using namespace GammaRay;

MetaPropertyAdaptor::MetaPropertyAdaptor(QObject *parent)
    : PropertyAdaptor(parent)
    , m_metaObj(nullptr)
    , m_obj(nullptr)
{
}

MetaPropertyAdaptor::~MetaPropertyAdaptor() = default;

void MetaPropertyAdaptor::doSetObject(const ObjectInstance &oi)
{
    Q_ASSERT(m_metaObj == nullptr);
    Q_ASSERT(m_obj == nullptr);

    switch (oi.type()) {
    case ObjectInstance::Object:
    case ObjectInstance::Value:
        m_obj = oi.object();
        m_metaObj = MetaObjectRepository::instance()->metaObject(oi.typeName(), m_obj);
        break;
    case ObjectInstance::QtObject:
    case ObjectInstance::QtGadgetPointer:
    case ObjectInstance::QtGadgetValue:
    {
        const QMetaObject *mo = oi.metaObject();
        while (mo && !m_metaObj) {
            m_metaObj = MetaObjectRepository::instance()->metaObject(mo->className());
            mo = mo->superClass();
        }
        if (m_metaObj)
            m_obj = oi.object();
        break;
    }
    default:
        break;
    }
}

int MetaPropertyAdaptor::count() const
{
    if (!m_metaObj || !object().isValid())
        return 0;
    return m_metaObj->propertyCount();
}

PropertyData MetaPropertyAdaptor::propertyData(int index) const
{
    Q_ASSERT(m_metaObj);
    PropertyData data;
    if (!object().isValid())
        return data;

    const auto property = m_metaObj->propertyAt(index);

    data.setName(property->name());
    data.setTypeName(property->typeName());
    data.setClassName(property->metaObject()->className());
    data.setAccessFlags(property->isReadOnly() ? PropertyData::Readable : PropertyData::Writable);

    if (m_obj) {
        const auto value = property->value(m_metaObj->castForPropertyAt(m_obj, index));
        data.setValue(value);
    }

    return data;
}

void MetaPropertyAdaptor::writeProperty(int index, const QVariant &value)
{
    if (!object().isValid())
        return;

    Q_ASSERT(m_metaObj && m_obj);
    const auto prop = m_metaObj->propertyAt(index);
    prop->setValue(m_metaObj->castForPropertyAt(m_obj, index), value);
    emit propertyChanged(index, index);
}
