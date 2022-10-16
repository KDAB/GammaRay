/*
  propertyadaptor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
