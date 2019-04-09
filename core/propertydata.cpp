/*
  propertydata.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

void PropertyData::setNotifySignal(const QString& notifySignal)
{
    m_notifySignal = notifySignal;
}
