/*
  metaobject.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "metaobject.h"

using namespace GammaRay;

MetaObject::MetaObject() = default;

MetaObject::~MetaObject()
{
    qDeleteAll(m_properties);
}

int MetaObject::propertyCount() const
{
    int count = 0;
    for (MetaObject *mo : m_baseClasses)
        count += mo->propertyCount();
    return count + m_properties.size();
}

MetaProperty *MetaObject::propertyAt(int index) const
{
    for (MetaObject *mo : m_baseClasses) {
        if (index >= mo->propertyCount())
            index -= mo->propertyCount();
        else
            return mo->propertyAt(index);
    }
    Q_ASSERT(index >= 0 && index < m_properties.size());
    return m_properties.at(index);
}

void MetaObject::addBaseClass(MetaObject *baseClass)
{
    Q_ASSERT(baseClass);
    m_baseClasses.push_back(baseClass);
}

void MetaObject::addProperty(MetaProperty *property)
{
    Q_ASSERT(property);
    // TODO: sort
    property->setMetaObject(this);
    m_properties.push_back(property);
}

QString MetaObject::className() const
{
    return m_className;
}

void MetaObject::setClassName(const QString &className)
{
    m_className = className;
}

void *MetaObject::castForPropertyAt(void *object, int index) const
{
    for (int i = 0; i < m_baseClasses.size(); ++i) {
        const MetaObject *base = m_baseClasses.at(i);
        if (index >= base->propertyCount())
            index -= base->propertyCount();
        else
            return base->castForPropertyAt(castToBaseClass(object, i), index);
    }
    return object; // our own property
}

void *MetaObject::castTo(void *object, const QString &baseClass) const
{
    if (className() == baseClass)
        return object;

    for (int i = 0; i < m_baseClasses.size(); ++i) {
        const MetaObject *base = m_baseClasses.at(i);
        const auto result = base->castTo(castToBaseClass(object, i), baseClass);
        if (result)
            return result;
    }

    return nullptr;
}

bool MetaObject::isPolymorphic() const
{
    if (isClassPolymorphic())
        return true;
    for (const auto &baseClass : m_baseClasses) {
        if (baseClass->isPolymorphic())
            return true;
    }
    return false;
}

void *MetaObject::castFrom(void *object, MetaObject *baseClass) const
{
    const auto idx = m_baseClasses.indexOf(baseClass);
    if (idx < 0)
        return nullptr;
    return castFromBaseClass(object, idx);
}

MetaObject *MetaObject::superClass(int index) const
{
    if (m_baseClasses.size() <= index)
        return nullptr;
    return m_baseClasses[index];
}

bool MetaObject::inherits(const QString &className) const
{
    if (className == m_className)
        return true;
    for (MetaObject *metaObject : m_baseClasses) {
        if (metaObject->inherits(className))
            return true;
    }
    return false;
}
