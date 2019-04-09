/*
  sequentialpropertyadaptor.cpp

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

#include "sequentialpropertyadaptor.h"
#include "objectinstance.h"
#include "propertydata.h"

#include <QSequentialIterable>

using namespace GammaRay;

SequentialPropertyAdaptor::SequentialPropertyAdaptor(QObject *parent)
    : PropertyAdaptor(parent)
{
}

SequentialPropertyAdaptor::~SequentialPropertyAdaptor() = default;

void SequentialPropertyAdaptor::doSetObject(const ObjectInstance &oi)
{
    if (oi.type() == ObjectInstance::QtVariant)
        m_value = oi.variant();
}

int SequentialPropertyAdaptor::count() const
{
    if (!m_value.canConvert<QVariantList>())
        return 0;
    auto it = m_value.value<QSequentialIterable>();
    return it.size();
}

PropertyData SequentialPropertyAdaptor::propertyData(int index) const
{
    Q_ASSERT(m_value.canConvert<QVariantList>());

    auto seq = m_value.value<QSequentialIterable>();
    auto it = seq.begin();
    it += index;

    PropertyData data;
    data.setName(QString::number(index));
    data.setValue(*it);
    data.setClassName(m_value.typeName());

    return data;
}
