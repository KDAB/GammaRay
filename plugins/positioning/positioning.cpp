/*
  positioning.cpp

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

#include "positioning.h"
#include "positioninfopropertyadaptor.h"

#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/varianthandler.h>

#include <QDataStream>
#include <QGeoPositionInfoSource>
#include <QGeoSatelliteInfoSource>
#include <QGeoAreaMonitorSource>

#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
Q_DECLARE_METATYPE(QGeoPositionInfo)
#endif
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

Positioning::Positioning(Probe *probe, QObject *parent)
    : PositioningInterface(parent)
{
    qRegisterMetaTypeStreamOperators<QGeoPositionInfo>("QGeoPositionInfo");
    registerMetaTypes();
    connect(probe, &Probe::objectCreated, this, &Positioning::objectAdded);
}

void Positioning::objectAdded(QObject* obj)
{
    if (auto geoInfoSource = qobject_cast<QGeoPositionInfoSource*>(obj)) {
        if (geoInfoSource->sourceName() != QLatin1Literal("gammaray")) {
            if (positioningOverrideAvailable()) // we already have a proxy source taking care of things
                return;
            // until we have a proxy, just forward the position from the real source
            connect(geoInfoSource, &QGeoPositionInfoSource::positionUpdated, this, &Positioning::setPositionInfo);
            setPositionInfo(geoInfoSource->lastKnownPosition());
            m_nonProxyPositionInfoSources.push_back(geoInfoSource);
        } else {
            // we previously got non-proxied sources, disconnect those
            if (!m_nonProxyPositionInfoSources.empty()) {
                std::for_each(m_nonProxyPositionInfoSources.begin(), m_nonProxyPositionInfoSources.end(), [this](QGeoPositionInfoSource *source) {
                    disconnect(source, &QGeoPositionInfoSource::positionUpdated, this, &Positioning::setPositionInfo);
                });
                m_nonProxyPositionInfoSources.clear();
            }
            QMetaObject::invokeMethod(geoInfoSource, "setInterface", Q_ARG(PositioningInterface*, this));
        }
    }
}

void Positioning::registerMetaTypes()
{
    MetaObject *mo = nullptr;
    MO_ADD_METAOBJECT0(QGeoPositionInfo);
    MO_ADD_PROPERTY_RO(QGeoPositionInfo, coordinate);
    MO_ADD_PROPERTY   (QGeoPositionInfo, timestamp, setTimestamp);

    MO_ADD_METAOBJECT1(QGeoPositionInfoSource, QObject);
    MO_ADD_PROPERTY_RO(QGeoPositionInfoSource, error);
    MO_ADD_PROPERTY_LD(QGeoPositionInfoSource, lastKnownPosition, [](QGeoPositionInfoSource *s) { return s->lastKnownPosition(); });
    MO_ADD_PROPERTY_RO(QGeoPositionInfoSource, preferredPositioningMethods);
    MO_ADD_PROPERTY_RO(QGeoPositionInfoSource, supportedPositioningMethods);

    MO_ADD_METAOBJECT1(QGeoSatelliteInfoSource, QObject);
    MO_ADD_PROPERTY_RO(QGeoSatelliteInfoSource, error);
    MO_ADD_PROPERTY_RO(QGeoSatelliteInfoSource, sourceName);

    MO_ADD_METAOBJECT1(QGeoAreaMonitorSource, QObject);
    MO_ADD_PROPERTY_RO(QGeoAreaMonitorSource, error);
    MO_ADD_PROPERTY_RO(QGeoAreaMonitorSource, sourceName);
    MO_ADD_PROPERTY_RO(QGeoAreaMonitorSource, supportedAreaMonitorFeatures);

    VariantHandler::registerStringConverter<QGeoPositionInfoSource::PositioningMethods>(positioningMethodsToString);
    VariantHandler::registerStringConverter<QGeoPositionInfo>([](const QGeoPositionInfo &info) {
        return VariantHandler::displayString(info.coordinate());
    });
    VariantHandler::registerStringConverter<QGeoCoordinate>([](const QGeoCoordinate &coord) {
        return coord.toString();
    });

    PropertyAdaptorFactory::registerFactory(PositionInfoPropertyAdaptorFactory::instance());
}
