/*
  mapcontroller.h

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

#ifndef GAMMARAY_MAPCONTROLLER_H
#define GAMMARAY_MAPCONTROLLER_H

#include <QObject>
#include <QGeoCoordinate>

namespace GammaRay {

class MapController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QGeoCoordinate overrideCoordinate READ overrideCoordinate WRITE setOverrideCoordinate NOTIFY overrideCoordinateChanged)
    Q_PROPERTY(double overrideHorizontalAccuracy READ overrideHorizontalAccuracy WRITE setOverrideHorizontalAccuracy NOTIFY overrideHorizontalAccuracyChanged)
public:
    explicit MapController(QObject *parent = Q_NULLPTR);
    ~MapController();

    QGeoCoordinate overrideCoordinate() const;
    void setOverrideCoordinate(const QGeoCoordinate &coord);

    double overrideHorizontalAccuracy() const;
    void setOverrideHorizontalAccuracy(double accu);

signals:
    void overrideCoordinateChanged();
    void overrideHorizontalAccuracyChanged();

private:
    QGeoCoordinate m_overrideCoordinate;
    double m_horizontalAccuracy;
};
}

#endif // GAMMARAY_MAPCONTROLLER_H
