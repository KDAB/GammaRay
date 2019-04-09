/*
  propertyadaptor.cpp

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

#include "propertyadaptor.h"
#include "propertydata.h"

using namespace GammaRay;

PropertyAdaptor::PropertyAdaptor(QObject *parent)
    : QObject(parent)
{
}

PropertyAdaptor::~PropertyAdaptor() = default;

const ObjectInstance &PropertyAdaptor::object() const
{
    return m_oi;
}

void PropertyAdaptor::setObject(const ObjectInstance &oi)
{
    m_oi = oi;
    doSetObject(m_oi);
}

void PropertyAdaptor::writeProperty(int index, const QVariant &value)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
}

bool PropertyAdaptor::canAddProperty() const
{
    return false;
}

void PropertyAdaptor::addProperty(const PropertyData &data)
{
    Q_UNUSED(data);
    Q_ASSERT(false);
}

void PropertyAdaptor::resetProperty(int index)
{
    Q_UNUSED(index);
    Q_ASSERT(false);
}

void PropertyAdaptor::doSetObject(const ObjectInstance &oi)
{
    Q_UNUSED(oi);
}

PropertyAdaptor *GammaRay::PropertyAdaptor::parentAdaptor() const
{
    return qobject_cast<PropertyAdaptor *>(parent());
}
