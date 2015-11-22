/*
  positioning.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "positioning.h"

#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/varianthandler.h>

#include <QGeoPositionInfoSource>
#include <QGeoSatelliteInfoSource>
#include <QGeoAreaMonitorSource>

Q_DECLARE_METATYPE(QGeoPositionInfoSource::Error)
Q_DECLARE_METATYPE(QGeoPositionInfoSource::PositioningMethods)
Q_DECLARE_METATYPE(QGeoSatelliteInfoSource::Error)
Q_DECLARE_METATYPE(QGeoAreaMonitorSource::Error)
Q_DECLARE_METATYPE(QGeoAreaMonitorSource::AreaMonitorFeatures)

using namespace GammaRay;

static QString positioningMethodsToString(QGeoPositionInfoSource::PositioningMethods methods)
{
    if (methods == QGeoPositionInfoSource::NoPositioningMethods)
        return QStringLiteral("NoPositioningMethods");
    if (methods == QGeoPositionInfoSource::AllPositioningMethods)
        return QStringLiteral("AllPositioningMethods");

    QStringList l;
    if (methods & QGeoPositionInfoSource::SatellitePositioningMethods)
        l.push_back(QStringLiteral("SatellitePositioningMethods"));
    if (methods & QGeoPositionInfoSource::NonSatellitePositioningMethods)
        l.push_back(QStringLiteral("NonSatellitePositioningMethods"));

  return l.join(QLatin1Char('|'));
}

Positioning::Positioning(ProbeInterface* probe, QObject* parent): QObject(parent)
{
    Q_UNUSED(probe);

    MetaObject *mo = 0;
    MO_ADD_METAOBJECT1(QGeoPositionInfoSource, QObject);
    MO_ADD_PROPERTY_RO(QGeoPositionInfoSource, QGeoPositionInfoSource::Error, error);
    MO_ADD_PROPERTY_RO(QGeoPositionInfoSource, QGeoPositionInfoSource::PositioningMethods, preferredPositioningMethods);
    MO_ADD_PROPERTY_RO(QGeoPositionInfoSource, QGeoPositionInfoSource::PositioningMethods, supportedPositioningMethods);

    MO_ADD_METAOBJECT1(QGeoSatelliteInfoSource, QObject);
    MO_ADD_PROPERTY_RO(QGeoSatelliteInfoSource, QGeoSatelliteInfoSource::Error, error);
    MO_ADD_PROPERTY_RO(QGeoSatelliteInfoSource, QString, sourceName);

    MO_ADD_METAOBJECT1(QGeoAreaMonitorSource, QObject);
    MO_ADD_PROPERTY_RO(QGeoAreaMonitorSource, QGeoAreaMonitorSource::Error, error);
    MO_ADD_PROPERTY_RO(QGeoAreaMonitorSource, QString, sourceName);
    MO_ADD_PROPERTY_RO(QGeoAreaMonitorSource, QGeoAreaMonitorSource::AreaMonitorFeatures, supportedAreaMonitorFeatures);

    VariantHandler::registerStringConverter<QGeoPositionInfoSource::PositioningMethods>(positioningMethodsToString);
}

QString PositioningFactory::name() const
{
    return tr("Positioning");
}
