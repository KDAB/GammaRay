/*
  qjsvaluepropertyadaptor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
