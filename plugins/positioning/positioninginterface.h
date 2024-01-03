/*
  positioninginterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    Q_PROPERTY(bool positioningOverrideAvailable READ positioningOverrideAvailable WRITE setPositioningOverrideAvailable NOTIFY positioningOverrideAvailableChanged)
    Q_PROPERTY(bool positioningOverrideEnabled READ positioningOverrideEnabled WRITE setPositioningOverrideEnabled NOTIFY positioningOverrideEnabledChanged)
    Q_PROPERTY(QGeoPositionInfo positionInfoOverride READ positionInfoOverride WRITE setPositionInfoOverride NOTIFY positionInfoOverrideChanged)
public:
    explicit PositioningInterface(QObject *parent = nullptr);

    /** Position info from the source. */
    QGeoPositionInfo positionInfo() const;

    /** Overriding the position information is possible. */
    bool positioningOverrideAvailable() const;
    void setPositioningOverrideAvailable(bool available);

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
    void positioningOverrideAvailableChanged();
    void positioningOverrideEnabledChanged();
    void positionInfoOverrideChanged();

private:
    QGeoPositionInfo m_postionInfo;
    QGeoPositionInfo m_postionInfoOverride;
    bool m_positioningOverrideAvailable;
    bool m_positioningOverrideEnabled;
};

}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::PositioningInterface, "com.kdab.GammaRay.PositioningInterface")
QT_END_NAMESPACE

#endif // GAMMARAY_POSITIONINGINTERFACE_H
