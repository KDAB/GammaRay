/*
  qjsvaluepropertyadaptor.cpp

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

#include "qjsvaluepropertyadaptor.h"

#include <core/propertydata.h>

#include <QDebug>
#include <QJSValue>

using namespace GammaRay;

QJSValuePropertyAdaptor::QJSValuePropertyAdaptor(QObject *parent)
    : PropertyAdaptor(parent)
{
}

QJSValuePropertyAdaptor::~QJSValuePropertyAdaptor() = default;

int QJSValuePropertyAdaptor::count() const
{
    auto jsValue = object().variant().value<QJSValue>();
    if (jsValue.isArray())
        return jsValue.toVariant().value<QVariantList>().size();
    return 0;
}

PropertyData QJSValuePropertyAdaptor::propertyData(int index) const
{
    PropertyData pd;
    auto jsValue = object().variant().value<QJSValue>();

    if (jsValue.isArray()) {
        auto list = jsValue.toVariant().value<QVariantList>();
        if (index < 0 || index >= list.size())
            return pd;
        pd.setName(QString::number(index));
        pd.setValue(list.at(index));
        pd.setClassName(QStringLiteral("QJSValue"));
    }

    return pd;
}

QJSValuePropertyAdaptorFactory *QJSValuePropertyAdaptorFactory::s_instance = nullptr;

PropertyAdaptor *QJSValuePropertyAdaptorFactory::create(const ObjectInstance &oi,
                                                        QObject *parent) const
{
    if (oi.type() != ObjectInstance::QtVariant)
        return nullptr;

    if (!oi.variant().isValid() || !oi.variant().canConvert<QJSValue>())
        return nullptr;

    return new QJSValuePropertyAdaptor(parent);
}

QJSValuePropertyAdaptorFactory *QJSValuePropertyAdaptorFactory::instance()
{
    if (!s_instance)
        s_instance = new QJSValuePropertyAdaptorFactory;
    return s_instance;
}
