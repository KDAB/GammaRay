/*
  jsonobjectpropertyadaptor.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

    if(oi.variant().userType() == qMetaTypeId<QJsonArray>()) {
        m_array = oi.variant().toJsonArray();
        m_isObject = false;
    } else if(oi.variant().toJsonValue().isArray()) {
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
    } else {
        return m_array.size();
    }
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
