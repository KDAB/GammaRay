/*
  positioninfopropertyadaptor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "positioninfopropertyadaptor.h"

#include <core/metaenum.h>
#include <core/propertydata.h>

#include <QDebug>
#include <QGeoPositionInfo>

using namespace GammaRay;

#define E(x)                    \
    {                           \
        QGeoPositionInfo::x, #x \
    }
static const MetaEnum::Value<QGeoPositionInfo::Attribute> attribute_table[] = {
    E(Direction),
    E(GroundSpeed),
    E(VerticalSpeed),
    E(MagneticVariation),
    E(HorizontalAccuracy),
    E(VerticalAccuracy)
};
#undef E

PositionInfoPropertyAdaptor::PositionInfoPropertyAdaptor(QObject *parent)
    : PropertyAdaptor(parent)
{
}

PositionInfoPropertyAdaptor::~PositionInfoPropertyAdaptor() = default;

int PositionInfoPropertyAdaptor::count() const
{
    return MetaEnum::count(attribute_table);
}

PropertyData PositionInfoPropertyAdaptor::propertyData(int index) const
{
    PropertyData pd;
    pd.setName(MetaEnum::enumToString(static_cast<QGeoPositionInfo::Attribute>(index), attribute_table));
    pd.setValue(object().variant().value<QGeoPositionInfo>().attribute(static_cast<QGeoPositionInfo::Attribute>(index)));
    pd.setClassName(QStringLiteral("QGeoPositionInfo"));
    return pd;
}


PositionInfoPropertyAdaptorFactory *PositionInfoPropertyAdaptorFactory::s_instance = nullptr;

PropertyAdaptor *PositionInfoPropertyAdaptorFactory::create(const ObjectInstance &oi, QObject *parent) const
{
    if (oi.type() != ObjectInstance::Value)
        return nullptr;

    if (oi.variant().userType() != qMetaTypeId<QGeoPositionInfo>())
        return nullptr;

    return new PositionInfoPropertyAdaptor(parent);
}

PositionInfoPropertyAdaptorFactory *PositionInfoPropertyAdaptorFactory::instance()
{
    if (!s_instance)
        s_instance = new PositionInfoPropertyAdaptorFactory;
    return s_instance;
}
