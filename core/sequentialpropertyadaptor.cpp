/*
  sequentialpropertyadaptor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
