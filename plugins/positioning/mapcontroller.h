/*
  mapcontroller.h

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

#ifndef GAMMARAY_MAPCONTROLLER_H
#define GAMMARAY_MAPCONTROLLER_H

#include <QObject>
#include <QGeoCoordinate>

namespace GammaRay {

class MapController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QGeoCoordinate sourceCoordinate READ sourceCoordinate WRITE setSourceCoordinate NOTIFY sourceChanged)
    Q_PROPERTY(double sourceHorizontalAccuracy READ sourceHorizontalAccuracy WRITE setSourceHorizontalAccuracy NOTIFY sourceChanged)
    Q_PROPERTY(double sourceDirection READ sourceDirection WRITE setSourceDirection NOTIFY sourceChanged)
    Q_PROPERTY(bool overrideEnabled READ overrideEnabled WRITE setOverrideEnabled NOTIFY overrideEnabledChanged)
    Q_PROPERTY(QGeoCoordinate overrideCoordinate READ overrideCoordinate WRITE setOverrideCoordinate NOTIFY overrideCoordinateChanged)
    Q_PROPERTY(double overrideHorizontalAccuracy READ overrideHorizontalAccuracy WRITE setOverrideHorizontalAccuracy NOTIFY overrideHorizontalAccuracyChanged)
    Q_PROPERTY(double overrideDirection READ overrideDirection WRITE setOverrideDirection NOTIFY overrideDirectionChanged)
public:
    explicit MapController(QObject *parent = nullptr);
    ~MapController() override;

    QGeoCoordinate sourceCoordinate() const;
    void setSourceCoordinate(const QGeoCoordinate &coord);

    double sourceHorizontalAccuracy() const;
    void setSourceHorizontalAccuracy(double accu);

    double sourceDirection() const;
    void setSourceDirection(double dir);

    bool overrideEnabled() const;
    void setOverrideEnabled(bool enabled);

    QGeoCoordinate overrideCoordinate() const;
    void setOverrideCoordinate(const QGeoCoordinate &coord);

    double overrideHorizontalAccuracy() const;
    void setOverrideHorizontalAccuracy(double accu);

    double overrideDirection() const;
    void setOverrideDirection(double dir);

signals:
    void centerOnPosition();

    void sourceChanged();
    void overrideEnabledChanged();
    void overrideCoordinateChanged();
    void overrideHorizontalAccuracyChanged();
    void overrideDirectionChanged();

private:
    QGeoCoordinate m_sourceCoordinate;
    QGeoCoordinate m_overrideCoordinate;
    double m_sourceHorizontalAccuracy;
    double m_sourceDirection;
    double m_overrideHorizontalAccuracy;
    double m_overrideDirection;
    bool m_overrideEnabled;
};
}

#endif // GAMMARAY_MAPCONTROLLER_H
