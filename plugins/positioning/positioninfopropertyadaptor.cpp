/*
  positioninfopropertyadaptor.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "positioninfopropertyadaptor.h"

#include <core/metaenum.h>
#include <core/propertydata.h>

#include <QDebug>
#include <QGeoPositionInfo>

#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
Q_DECLARE_METATYPE(QGeoPositionInfo)
#endif

using namespace GammaRay;

#define E(x) { QGeoPositionInfo:: x, #x }
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


PositionInfoPropertyAdaptorFactory* PositionInfoPropertyAdaptorFactory::s_instance = nullptr;

PropertyAdaptor* PositionInfoPropertyAdaptorFactory::create(const ObjectInstance &oi, QObject *parent) const
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
