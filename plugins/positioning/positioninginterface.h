/*
  positioninginterface.h

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

#ifndef GAMMARAY_POSITIONINGINTERFACE_H
#define GAMMARAY_POSITIONINGINTERFACE_H

#include <QObject>
#include <QGeoPositionInfo>

namespace GammaRay {

class PositioningInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QGeoPositionInfo positionInfo READ positionInfo WRITE setPositionInfo NOTIFY positionInfoChanged)
    Q_PROPERTY(bool positioningOverrideEnabled READ positioningOverrideEnabled WRITE setPositioningOverrideEnabled NOTIFY positioningOverrideEnabledChanged)
    Q_PROPERTY(QGeoPositionInfo positionInfoOverride READ positionInfoOverride WRITE setPositionInfoOverride NOTIFY positionInfoOverrideChanged)
public:
    explicit PositioningInterface(QObject* parent = Q_NULLPTR);

    /** Position info from the source. */
    QGeoPositionInfo positionInfo() const;

    /** Override source position info with the override provided by the client. */
    bool positioningOverrideEnabled() const;
    void setPositioningOverrideEnabled(bool enabled);

    /** Override position info provided by the client. */
    QGeoPositionInfo positionInfoOverride() const;
    void setPositionInfoOverride(const QGeoPositionInfo &info);

public slots:
    void setPositionInfo(const QGeoPositionInfo &info);

signals:
    void positionInfoChanged();
    void positioningOverrideEnabledChanged();
    void positionInfoOverrideChanged();

private:
    QGeoPositionInfo m_postionInfo;
    bool m_positioningOverrideEnabled;
    QGeoPositionInfo m_postionInfoOverride;
};

}

Q_DECLARE_INTERFACE(GammaRay::PositioningInterface, "com.kdab.GammaRay.PositioningInterface")

#endif // GAMMARAY_POSITIONINGINTERFACE_H
