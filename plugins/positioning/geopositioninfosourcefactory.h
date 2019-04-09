/*
  geopositioninfosourcefactory.h

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

#ifndef GAMMARAY_GEOPOSITIONINFOSOURCEFACTORY_H
#define GAMMARAY_GEOPOSITIONINFOSOURCEFACTORY_H

#include <QObject>
#include <QGeoPositionInfoSourceFactory>

QT_BEGIN_NAMESPACE
class QFactoryLoader;
QT_END_NAMESPACE

namespace GammaRay {

class GeoPositionInfoSourceFactory : public QObject, QGeoPositionInfoSourceFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.qt.position.sourcefactory/5.0" FILE "geopositioninfosourcefactory.json")
    Q_INTERFACES(QGeoPositionInfoSourceFactory)
public:
    explicit GeoPositionInfoSourceFactory(QObject *parent = nullptr);
    ~GeoPositionInfoSourceFactory() override;

    QGeoPositionInfoSource *positionInfoSource(QObject *parent) override;
    QGeoSatelliteInfoSource *satelliteInfoSource(QObject *parent) override;
    QGeoAreaMonitorSource *areaMonitor(QObject *parent) override;

private:
    QFactoryLoader *m_factoryLoader;
};

}

#endif // GAMMARAY_GEOPOSITIONINFOSOURCEFACTORY_H
