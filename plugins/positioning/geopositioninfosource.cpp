/*
  geopositioninfosource.cpp

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

#include "geopositioninfosource.h"
#include "positioninginterface.h"

#include <QDebug>

using namespace GammaRay;

GeoPositionInfoSource::GeoPositionInfoSource(QObject* parent) :
    QGeoPositionInfoSource(parent),
    m_source(nullptr),
    m_interface(nullptr)
{
}

GeoPositionInfoSource::~GeoPositionInfoSource() = default;

void GeoPositionInfoSource::setSource(QGeoPositionInfoSource* source)
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

void GeoPositionInfoSource::setInterface(PositioningInterface* iface)
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
    connect(m_source, QOverload<QGeoPositionInfoSource::Error>::of(&QGeoPositionInfoSource::error), this, &GeoPositionInfoSource::error, Qt::UniqueConnection);
    connect(m_source, &QGeoPositionInfoSource::positionUpdated, this, &GeoPositionInfoSource::positionUpdated, Qt::UniqueConnection);
    connect(m_source, &QGeoPositionInfoSource::updateTimeout, this, &GeoPositionInfoSource::updateTimeout, Qt::UniqueConnection);
    QGeoPositionInfoSource::setPreferredPositioningMethods(m_source->preferredPositioningMethods());
    QGeoPositionInfoSource::setUpdateInterval(m_source->updateInterval());
}

void GeoPositionInfoSource::disconnectSource()
{
    if (!m_source)
        return;
    disconnect(m_source, QOverload<QGeoPositionInfoSource::Error>::of(&QGeoPositionInfoSource::error), this, &GeoPositionInfoSource::error);
    disconnect(m_source, &QGeoPositionInfoSource::positionUpdated, this, &GeoPositionInfoSource::positionUpdated);
    disconnect(m_source, &QGeoPositionInfoSource::updateTimeout, this, &GeoPositionInfoSource::updateTimeout);
}

void GeoPositionInfoSource::setupSourceUpdate()
{
    if (!m_source || !m_interface)
        return;
    connect(m_source, &QGeoPositionInfoSource::positionUpdated, m_interface, &PositioningInterface::setPositionInfo);
}
