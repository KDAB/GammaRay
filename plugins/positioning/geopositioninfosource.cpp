/*
  geopositioninfosource.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "geopositioninfosource.h"
#include "positioninginterface.h"

#include <QDebug>

using namespace GammaRay;

GeoPositionInfoSource::GeoPositionInfoSource(QObject *parent)
    : QGeoPositionInfoSource(parent)
    , m_source(nullptr)
    , m_interface(nullptr)
{
}

GeoPositionInfoSource::~GeoPositionInfoSource() = default;

void GeoPositionInfoSource::setSource(QGeoPositionInfoSource *source)
{
    Q_ASSERT(!m_source);
    m_source = source;
    if (source && !overrideEnabled())
        connectSource();
    setupSourceUpdate();
}

QGeoPositionInfoSource::Error GeoPositionInfoSource::error() const
{
    if (m_source)
        return m_source->error();
    return UnknownSourceError;
}

QGeoPositionInfo GeoPositionInfoSource::lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const
{
    if (m_source && !overrideEnabled())
        return m_source->lastKnownPosition(fromSatellitePositioningMethodsOnly);

    if (m_interface)
        return m_interface->positionInfoOverride();
    return QGeoPositionInfo();
}

int GeoPositionInfoSource::minimumUpdateInterval() const
{
    if (m_source)
        return m_source->minimumUpdateInterval();
    return 10000;
}

void GeoPositionInfoSource::setPreferredPositioningMethods(PositioningMethods methods)
{
    if (m_source) {
        m_source->setPreferredPositioningMethods(methods);
        QGeoPositionInfoSource::setPreferredPositioningMethods(m_source->preferredPositioningMethods());
    } else {
        QGeoPositionInfoSource::setPreferredPositioningMethods(methods);
    }
}

void GeoPositionInfoSource::setUpdateInterval(int msec)
{
    if (m_source) {
        m_source->setUpdateInterval(msec);
        QGeoPositionInfoSource::setUpdateInterval(m_source->updateInterval());
    } else {
        QGeoPositionInfoSource::setUpdateInterval(msec);
    }
}

QGeoPositionInfoSource::PositioningMethods GeoPositionInfoSource::supportedPositioningMethods() const
{
    if (m_source)
        return m_source->supportedPositioningMethods();
    return AllPositioningMethods;
}

void GeoPositionInfoSource::requestUpdate(int timeout)
{
    if (m_source)
        m_source->requestUpdate(timeout);
}

void GeoPositionInfoSource::startUpdates()
{
    if (m_source)
        m_source->startUpdates();
}

void GeoPositionInfoSource::stopUpdates()
{
    if (m_source)
        m_source->stopUpdates();
}

void GeoPositionInfoSource::setInterface(PositioningInterface *iface)
{
    Q_ASSERT(iface);
    m_interface = iface;
    m_interface->setPositioningOverrideAvailable(true);
    connect(m_interface, &PositioningInterface::positioningOverrideEnabledChanged, this, &GeoPositionInfoSource::overrideChanged);
    connect(m_interface, &PositioningInterface::positionInfoOverrideChanged, this, &GeoPositionInfoSource::positionInfoOverrideChanged);
    if (overrideEnabled())
        emit positionUpdated(lastKnownPosition());
    setupSourceUpdate();
}

bool GeoPositionInfoSource::overrideEnabled() const
{
    return m_interface && m_interface->positioningOverrideEnabled();
}

void GeoPositionInfoSource::overrideChanged()
{
    if (!overrideEnabled())
        connectSource();
    else
        disconnectSource();
    emit positionUpdated(lastKnownPosition());
}

void GeoPositionInfoSource::positionInfoOverrideChanged()
{
    if (overrideEnabled())
        emit positionUpdated(lastKnownPosition());
}

void GeoPositionInfoSource::connectSource()
{
    if (!m_source)
        return;

    connect(m_source, &QGeoPositionInfoSource::errorOccurred, this, &GeoPositionInfoSource::errorOccurred, Qt::UniqueConnection);
    connect(m_source, &QGeoPositionInfoSource::positionUpdated, this, &GeoPositionInfoSource::positionUpdated, Qt::UniqueConnection);
    QGeoPositionInfoSource::setPreferredPositioningMethods(m_source->preferredPositioningMethods());
    QGeoPositionInfoSource::setUpdateInterval(m_source->updateInterval());
}

void GeoPositionInfoSource::disconnectSource()
{
    if (!m_source)
        return;

    disconnect(m_source, &QGeoPositionInfoSource::errorOccurred, this, &GeoPositionInfoSource::errorOccurred);
    disconnect(m_source, &QGeoPositionInfoSource::positionUpdated, this, &GeoPositionInfoSource::positionUpdated);
}

void GeoPositionInfoSource::setupSourceUpdate()
{
    if (!m_source || !m_interface)
        return;
    connect(m_source, &QGeoPositionInfoSource::positionUpdated, m_interface, &PositioningInterface::setPositionInfo);
}
