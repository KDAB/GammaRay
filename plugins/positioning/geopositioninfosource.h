/*
  geopositioninfosource.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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

    void setInterface(PositioningInterface *iface);

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
