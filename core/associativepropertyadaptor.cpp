/*
  associativepropertyadaptor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
