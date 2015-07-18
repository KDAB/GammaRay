/*
  propertyaggregator.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "propertyaggregator.h"
#include "propertydata.h"
#include "objectinstance.h"

#include <QDebug>

#include <algorithm>
#include <numeric>

using namespace GammaRay;

PropertyAggregator::PropertyAggregator(QObject* parent): PropertyAdaptor(parent)
{
}

PropertyAggregator::~PropertyAggregator()
{
}

void PropertyAggregator::doSetObject(const ObjectInstance& oi)
{
    std::for_each(m_propertyAdaptors.begin(), m_propertyAdaptors.end(), [oi](PropertyAdaptor *pa) {
        pa->setObject(oi);
    });
}

int PropertyAggregator::count() const
{
    return std::accumulate(m_propertyAdaptors.constBegin(), m_propertyAdaptors.constEnd(), 0, [](int lhs, PropertyAdaptor *rhs) {
        return lhs + rhs->count();
    });
}

PropertyData PropertyAggregator::propertyData(int index) const
{
    int offset = 0;
    foreach (const auto adaptor, m_propertyAdaptors) {
        if (index < offset + adaptor->count())
            return adaptor->propertyData(index - offset);
        offset += adaptor->count();
    }

    Q_ASSERT(false);
    return PropertyData();
}

void PropertyAggregator::writeProperty(int index, const QVariant& value)
{
    int offset = 0;
    foreach (const auto adaptor, m_propertyAdaptors) {
        if (index < offset + adaptor->count())
            return adaptor->writeProperty(index - offset, value);
        offset += adaptor->count();
    }

    Q_ASSERT(false);
}

bool PropertyAggregator::canAddProperty() const
{
    auto count = std::count_if(m_propertyAdaptors.constBegin(), m_propertyAdaptors.constEnd(), [](PropertyAdaptor *pa) {
        return pa->canAddProperty();
    });
    return count == 1;
}

void PropertyAggregator::addProperty(const PropertyData& data)
{
    Q_ASSERT(canAddProperty());

    foreach (const auto adaptor, m_propertyAdaptors) {
        if (adaptor->canAddProperty()) {
            adaptor->addProperty(data);
            return;
        }
    }

    Q_ASSERT(false);
}

void PropertyAggregator::resetProperty(int index)
{
    int offset = 0;
    foreach (const auto adaptor, m_propertyAdaptors) {
        if (index < offset + adaptor->count()) {
            adaptor->resetProperty(index - offset);
            return;
        }
        offset += adaptor->count();
    }

    Q_ASSERT(false);
}

void PropertyAggregator::addPropertyAdaptor(PropertyAdaptor* adaptor)
{
    m_propertyAdaptors.push_back(adaptor);
    connect(adaptor, SIGNAL(propertyChanged(int,int)), this, SLOT(slotPropertyChanged(int,int)));
    connect(adaptor, SIGNAL(propertyAdded(int,int)), this, SLOT(slotPropertyAdded(int,int)));
    connect(adaptor, SIGNAL(propertyRemoved(int,int)), this, SLOT(slotPropertyRemoved(int,int)));
}

void PropertyAggregator::slotPropertyChanged(int first, int last)
{
    auto source = sender();
    Q_ASSERT(source);

    int offset = 0;
    foreach (auto pa, m_propertyAdaptors) {
        if (pa == source) {
            emit propertyChanged(first + offset, last + offset);
            return;
        } else {
            offset += pa->count();
        }
    }
}

void PropertyAggregator::slotPropertyAdded(int first, int last)
{
    auto source = sender();
    Q_ASSERT(source);

    int offset = 0;
    foreach (auto pa, m_propertyAdaptors) {
        if (pa == source) {
            emit propertyAdded(first + offset, last + offset);
            return;
        } else {
            offset += pa->count();
        }
    }
}

void PropertyAggregator::slotPropertyRemoved(int first, int last)
{
    auto source = sender();
    Q_ASSERT(source);

    int offset = 0;
    foreach (auto pa, m_propertyAdaptors) {
        if (pa == source) {
            emit propertyRemoved(first + offset, last + offset);
            return;
        } else {
            offset += pa->count();
        }
    }
}
