/*
  geopositioninfosource.cpp

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

#include "geopositioninfosource.h"

using namespace GammaRay;

GeoPositionInfoSource::GeoPositionInfoSource(QObject* parent) :
    QGeoPositionInfoSource(parent),
    m_source(Q_NULLPTR)
{
}

GeoPositionInfoSource::~GeoPositionInfoSource()
{
}

void GeoPositionInfoSource::setSource(QGeoPositionInfoSource* source)
{
    Q_ASSERT(!m_source);
    m_source = source;
    if (source) {
        connect(source, SIGNAL(error(QGeoPositionInfoSource::Error)), this, SIGNAL(error(QGeoPositionInfoSource::Error)));
        connect(source, SIGNAL(positionUpdated(QGeoPositionInfo)), this, SIGNAL(positionUpdated(QGeoPositionInfo)));
        connect(source, SIGNAL(updateTimeout()), this, SIGNAL(updateTimeout()));
        QGeoPositionInfoSource::setPreferredPositioningMethods(source->preferredPositioningMethods());
        QGeoPositionInfoSource::setUpdateInterval(source->updateInterval());
    }
}

QGeoPositionInfoSource::Error GeoPositionInfoSource::error() const
{
    if (m_source)
        return m_source->error();
    return UnknownSourceError;
}

QGeoPositionInfo GeoPositionInfoSource::lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const
{
    if (m_source)
        return m_source->lastKnownPosition(fromSatellitePositioningMethodsOnly);

    QGeoPositionInfo info;
    info.setCoordinate(QGeoCoordinate(52.5, 13.5));
    info.setTimestamp(QDateTime::currentDateTime());
    return info;
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
