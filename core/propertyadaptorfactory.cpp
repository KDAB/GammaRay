/*
  propertyadaptorfactory.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "propertyadaptorfactory.h"

#include "propertyaggregator.h"
#include "objectinstance.h"
#include "qmetapropertyadaptor.h"
#include "dynamicpropertyadaptor.h"
#include "sequentialpropertyadaptor.h"
#include "associativepropertyadaptor.h"
#include "metapropertyadaptor.h"
#include "jsonpropertyadaptor.h"

#include <QVector>

#include <algorithm>

using namespace GammaRay;

Q_GLOBAL_STATIC(QVector<AbstractPropertyAdaptorFactory *>, s_propertyAdaptorFactories)

AbstractPropertyAdaptorFactory::AbstractPropertyAdaptorFactory() = default;

AbstractPropertyAdaptorFactory::~AbstractPropertyAdaptorFactory() = default;

PropertyAdaptor *PropertyAdaptorFactory::create(const ObjectInstance &oi, QObject *parent)
{
    QVector<PropertyAdaptor *> adaptors;

    if (oi.metaObject())
        adaptors.push_back(new QMetaPropertyAdaptor(parent));
    if (oi.type() == ObjectInstance::QtObject)
        adaptors.push_back(new DynamicPropertyAdaptor(parent));
    if (oi.type() == ObjectInstance::QtObject || oi.type() == ObjectInstance::Object
        || oi.type() == ObjectInstance::Value || oi.type() == ObjectInstance::QtGadgetPointer || oi.type() == ObjectInstance::QtGadgetValue)
        adaptors.push_back(new MetaPropertyAdaptor(parent));

    if (oi.type() == ObjectInstance::QtVariant) {

        if (oi.typeName() == "QJsonObject" || oi.typeName() == "QJsonArray")
            adaptors.push_back(new JsonPropertyAdaptor(parent));
        else if (oi.typeName() == "QJsonValue") {
            if (oi.variant().toJsonValue().isObject() || oi.variant().toJsonValue().isArray()) {
                adaptors.push_back(new JsonPropertyAdaptor(parent));
            }
        } else if (oi.typeName() == "QJSValue") {
        } else {
            const auto &v = oi.variant();
            if (v.canConvert<QVariantList>())
                adaptors.push_back(new SequentialPropertyAdaptor(parent));
            else if (v.canConvert<QVariantHash>())
                adaptors.push_back(new AssociativePropertyAdaptor(parent));
        }
    }

    const auto factories = *s_propertyAdaptorFactories();
    for (auto factory : factories) {
        auto a = factory->create(oi, parent);
        if (a)
            adaptors.push_back(a);
    }

    if (adaptors.isEmpty())
        return nullptr;
    if (adaptors.size() == 1) {
        adaptors.first()->setObject(oi);
        return adaptors.first();
    }

    auto aggregator = new PropertyAggregator(parent);
    std::for_each(adaptors.constBegin(), adaptors.constEnd(), [&aggregator](PropertyAdaptor *pa) {
        aggregator->addPropertyAdaptor(pa);
    });
    aggregator->setObject(oi);

    return aggregator;
}

void PropertyAdaptorFactory::registerFactory(AbstractPropertyAdaptorFactory *factory)
{
    s_propertyAdaptorFactories()->push_back(factory);
}
