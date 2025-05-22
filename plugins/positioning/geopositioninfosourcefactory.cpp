/*
  geopositioninfosourcefactory.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "geopositioninfosourcefactory.h"
#include "geopositioninfosource.h"

#include <QDebug>
#include <private/qfactoryloader_p.h>

#include <algorithm>
#include <numeric>

using namespace GammaRay;

GeoPositionInfoSourceFactory::GeoPositionInfoSourceFactory(QObject *parent)
    : QObject(parent)
    , m_factoryLoader(new QFactoryLoader("org.qt-project.qt.position.sourcefactory/5.0", QStringLiteral("/position")))
{
}

GeoPositionInfoSourceFactory::~GeoPositionInfoSourceFactory()
{
    delete m_factoryLoader;
}

QGeoPositionInfoSource *GeoPositionInfoSourceFactory::positionInfoSource(QObject *parent, const QVariantMap & /*parameters*/)
{
    auto proxy = new GeoPositionInfoSource(parent);

    auto metaData = m_factoryLoader->metaData();
    QVector<int> indexes;
    indexes.resize(metaData.size());
    std::iota(indexes.begin(), indexes.end(), 0);

    // filter anything not applicable
    for (auto it = indexes.begin(); it != indexes.end();) {
        const auto data = metaData.at(*it).toCbor();
        const auto correctType = data.value(QStringLiteral("Position")).toBool();
        const auto isGammaray = data.value(QStringLiteral("Provider")).toString() == QLatin1String("gammaray");

        if (correctType && !isGammaray)
            ++it;
        else
            it = indexes.erase(it);
    }

    // sort by priority
    std::sort(indexes.begin(), indexes.end(), [metaData](int lhs, int rhs) {
        const auto lData = metaData.at(lhs).toCbor();
        const auto rData = metaData.at(rhs).toCbor();
        return lData.value(QStringLiteral("Priority")).toInteger() > rData.value(QStringLiteral("Priority")).toInteger();
    });

    // actually try the plugins
    QGeoPositionInfoSource *source = nullptr;
    for (auto it = indexes.constBegin(); it != indexes.constEnd(); ++it) {
        const auto data = metaData.at(*it).toCbor();
        const auto provider = data.value(QStringLiteral("Provider")).toString();
        if (provider.isEmpty())
            continue;
        if (auto s = QGeoPositionInfoSource::createSource(provider, proxy)) {
            source = s;
            break;
        }
    }

    proxy->setSource(source);
    return proxy;
}

QGeoSatelliteInfoSource *GeoPositionInfoSourceFactory::satelliteInfoSource(QObject *parent, const QVariantMap & /*parameters*/)
{
    Q_UNUSED(parent);
    return nullptr;
}

QGeoAreaMonitorSource *GeoPositionInfoSourceFactory::areaMonitor(QObject *parent, const QVariantMap & /*parameters*/)
{
    Q_UNUSED(parent);
    return nullptr;
}
