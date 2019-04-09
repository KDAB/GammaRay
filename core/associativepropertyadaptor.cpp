/*
  associativepropertyadaptor.cpp

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

#include "associativepropertyadaptor.h"
#include "objectinstance.h"
#include "propertydata.h"
#include "varianthandler.h"

#include <QAssociativeIterable>

using namespace GammaRay;

AssociativePropertyAdaptor::AssociativePropertyAdaptor(QObject *parent)
    : PropertyAdaptor(parent)
{
}

AssociativePropertyAdaptor::~AssociativePropertyAdaptor() = default;

void AssociativePropertyAdaptor::doSetObject(const ObjectInstance &oi)
{
    if (oi.type() == ObjectInstance::QtVariant)
        m_value = oi.variant();
}

int AssociativePropertyAdaptor::count() const
{
    if (!m_value.canConvert<QVariantHash>())
        return 0;
    auto it = m_value.value<QAssociativeIterable>();
    return it.size();
}

PropertyData AssociativePropertyAdaptor::propertyData(int index) const
{
    Q_ASSERT(m_value.canConvert<QVariantHash>());

    auto iterable = m_value.value<QAssociativeIterable>();
    auto it = iterable.begin();
    it += index;

    PropertyData data;
    data.setName(VariantHandler::displayString(it.key()));
    data.setValue(it.value());
    data.setClassName(m_value.typeName());

    return data;
}
