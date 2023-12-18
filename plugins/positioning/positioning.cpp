/*
  positioning.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "positioning.h"
#include "positioninfopropertyadaptor.h"

#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/varianthandler.h>

#include <common/streamoperators.h>

#include <QDataStream>
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

Positioning::Positioning(Probe *probe, QObject *parent)
    : PositioningInterface(parent)
{
    StreamOperators::registerOperators<QGeoPositionInfo>();
    registerMetaTypes();
    connect(probe, &Probe::objectCreated, this, &Positioning::objectAdded);
}

void Positioning::objectAdded(QObject *obj)
{
    if (auto geoInfoSource = qobject_cast<QGeoPositionInfoSource *>(obj)) {
        if (geoInfoSource->sourceName() != QLatin1String("gammaray")) {
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
            // clang-format off
            QMetaObject::invokeMethod(geoInfoSource, "setInterface", Q_ARG(PositioningInterface*, this));
            // clang-format on
        }
    }
}

void Positioning::registerMetaTypes()
{
    MetaObject *mo = nullptr;
    MO_ADD_METAOBJECT0(QGeoPositionInfo);
    MO_ADD_PROPERTY_RO(QGeoPositionInfo, coordinate);
    MO_ADD_PROPERTY(QGeoPositionInfo, timestamp, setTimestamp);

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
