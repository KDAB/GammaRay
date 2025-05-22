/*
  geopositioninfosourcefactory.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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

    QGeoPositionInfoSource *positionInfoSource(QObject *parent, const QVariantMap &parameters) override;
    QGeoSatelliteInfoSource *satelliteInfoSource(QObject *parent, const QVariantMap &parameters) override;
    QGeoAreaMonitorSource *areaMonitor(QObject *parent, const QVariantMap &parameters) override;

private:
    QFactoryLoader *m_factoryLoader;
};

}

#endif // GAMMARAY_GEOPOSITIONINFOSOURCEFACTORY_H
