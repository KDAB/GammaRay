/*
  geopositioninfosource.h

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

#ifndef GAMMARAY_GEOPOSITIONINFOSOURCE_H
#define GAMMARAY_GEOPOSITIONINFOSOURCE_H

#include <QGeoPositionInfoSource>

namespace GammaRay {

class PositioningInterface;

class GeoPositionInfoSource : public QGeoPositionInfoSource
{
    Q_OBJECT
public:
    explicit GeoPositionInfoSource(QObject *parent = nullptr);
    ~GeoPositionInfoSource() override;

    void setSource(QGeoPositionInfoSource *source);

    Error error() const override;
    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const override;
    int minimumUpdateInterval() const override;
    void setPreferredPositioningMethods(PositioningMethods methods) override;
    void setUpdateInterval(int msec) override;
    PositioningMethods supportedPositioningMethods() const override;

public slots:
    void requestUpdate(int timeout = 0) override;
    void startUpdates() override;
    void stopUpdates() override;

    void setInterface(PositioningInterface* iface);

private:
    bool overrideEnabled() const;
    void connectSource();
    void disconnectSource();
    void setupSourceUpdate();

private slots:
    void overrideChanged();
    void positionInfoOverrideChanged();

private:
    QGeoPositionInfoSource *m_source;
    PositioningInterface *m_interface;
};

}

#endif // GAMMARAY_GEOPOSITIONINFOSOURCE_H
