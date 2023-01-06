/*
  jsonpropertyadaptor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "jsonpropertyadaptor.h"
#include "objectinstance.h"
#include "propertydata.h"
#include "varianthandler.h"

using namespace GammaRay;

JsonPropertyAdaptor::JsonPropertyAdaptor(QObject *parent)
    : PropertyAdaptor(parent)
    , m_isObject(false)
{
}

JsonPropertyAdaptor::~JsonPropertyAdaptor() = default;

void JsonPropertyAdaptor::doSetObject(const ObjectInstance &oi)
{

    if (oi.variant().userType() == qMetaTypeId<QJsonArray>()) {
        m_array = oi.variant().toJsonArray();
        m_isObject = false;
    } else if (oi.variant().toJsonValue().isArray()) {
        m_array = oi.variant().toJsonValue().toArray();
        m_isObject = false;
    } else {
        m_object = oi.variant().toJsonObject();
        m_isObject = true;
    }
}

int JsonPropertyAdaptor::count() const
{
    if (m_isObject) {
        return m_object.size();
    }
    return m_array.size();
}

PropertyData JsonPropertyAdaptor::propertyData(int index) const
{
    PropertyData data;
    if (m_isObject) {
        auto it = m_object.begin();
        it += index;
        data.setName(VariantHandler::displayString(it.key()));
        data.setValue(it.value().toVariant());
        data.setClassName(QStringLiteral("QJsonObject"));
    } else {
        data.setName(VariantHandler::displayString(index));
        data.setValue(m_array[index]);
        data.setClassName(QStringLiteral("QJsonArray"));
    }

    return data;
}
