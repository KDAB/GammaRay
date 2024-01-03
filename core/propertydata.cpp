/*
  propertydata.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "propertydata.h"

using namespace GammaRay;

QString PropertyData::name() const
{
    return m_name;
}

void PropertyData::setName(const QString &name)
{
    m_name = name;
}

QVariant PropertyData::value() const
{
    return m_value;
}

void PropertyData::setValue(const QVariant &value)
{
    m_value = value;
}

QString PropertyData::typeName() const
{
    if (m_typeName.isEmpty())
        return m_value.typeName();
    return m_typeName;
}

void PropertyData::setTypeName(const QString &typeName)
{
    m_typeName = typeName;
}

QString PropertyData::className() const
{
    return m_className;
}

void PropertyData::setClassName(const QString &className)
{
    m_className = className;
}

PropertyData::AccessFlags PropertyData::accessFlags() const
{
    return m_accessFlags;
}

void PropertyData::setAccessFlags(PropertyData::AccessFlags flags)
{
    m_accessFlags = flags;
}

PropertyModel::PropertyFlags PropertyData::propertyFlags() const
{
    return m_propertyFlags;
}

void PropertyData::setPropertyFlags(PropertyModel::PropertyFlags flags)
{
    m_propertyFlags = flags;
}

int PropertyData::revision() const
{
    return m_revision;
}

void PropertyData::setRevision(int rev)
{
    m_revision = rev;
}

QString PropertyData::notifySignal() const
{
    return m_notifySignal;
}

void PropertyData::setNotifySignal(const QString &notifySignal)
{
    m_notifySignal = notifySignal;
}
