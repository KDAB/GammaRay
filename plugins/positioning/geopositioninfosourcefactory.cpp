/*
  geopositioninfosourcefactory.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "geopositioninfosourcefactory.h"
#include "geopositioninfosource.h"

#include <QDebug>
#include <private/qfactoryloader_p.h>

#include <algorithm>
#include <numeric>

using namespace GammaRay;

GeoPositionInfoSourceFactory::GeoPositionInfoSourceFactory(QObject* parent):
    QObject(parent),
    m_factoryLoader(new QFactoryLoader("org.qt-project.qt.position.sourcefactory/5.0", QStringLiteral("/position")))
{
}

GeoPositionInfoSourceFactory::~GeoPositionInfoSourceFactory()
{
    delete m_factoryLoader;
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
QGeoPositionInfoSource* GeoPositionInfoSourceFactory::positionInfoSource(QObject *parent)
#else
QGeoPositionInfoSource* GeoPositionInfoSourceFactory::positionInfoSource(QObject *parent, const QVariantMap &/*parameters*/)
#endif
{
    auto proxy = new GeoPositionInfoSource(parent);

    auto metaData = m_factoryLoader->metaData();
    QVector<int> indexes;
    indexes.resize(metaData.size());
    std::iota(indexes.begin(), indexes.end(), 0);

    // filter anything not applicable
    for (auto it = indexes.begin(); it != indexes.end();) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
        const auto data = metaData.at(*it).toCbor();
#else
        const auto data = metaData.at(*it).value(QStringLiteral("MetaData")).toObject();
#endif
        const auto correctType = data.value(QStringLiteral("Position")).toBool();
        const auto isGammaray = data.value(QStringLiteral("Provider")).toString() == QLatin1String("gammaray");

        if (correctType && !isGammaray)
            ++it;
        else
            it = indexes.erase(it);
    }

    // sort by priority
    std::sort(indexes.begin(), indexes.end(), [metaData](int lhs, int rhs) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
        const auto lData = metaData.at(lhs).toCbor();
        const auto rData = metaData.at(rhs).toCbor();
        return lData.value(QStringLiteral("Priority")).toInteger() > rData.value(QStringLiteral("Priority")).toInteger();
#else
        const auto lData = metaData.at(lhs).value(QStringLiteral("MetaData")).toObject();
        const auto rData = metaData.at(rhs).value(QStringLiteral("MetaData")).toObject();
        return lData.value(QStringLiteral("Priority")).toInt() > rData.value(QStringLiteral("Priority")).toInt();
#endif
    });

    // actually try the plugins
    QGeoPositionInfoSource *source = nullptr;
    for (auto it = indexes.constBegin(); it != indexes.constEnd(); ++it) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
        const auto data = metaData.at(*it).toCbor();
#else
        const auto data = metaData.at(*it).value(QStringLiteral("MetaData")).toObject();
#endif
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

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
QGeoSatelliteInfoSource* GeoPositionInfoSourceFactory::satelliteInfoSource(QObject *parent)
#else
QGeoSatelliteInfoSource* GeoPositionInfoSourceFactory::satelliteInfoSource(QObject *parent, const QVariantMap &/*parameters*/)
#endif
{
    Q_UNUSED(parent);
    return nullptr;
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
QGeoAreaMonitorSource* GeoPositionInfoSourceFactory::areaMonitor(QObject *parent)
#else
QGeoAreaMonitorSource* GeoPositionInfoSourceFactory::areaMonitor(QObject *parent, const QVariantMap &/*parameters*/)
#endif
{
    Q_UNUSED(parent);
    return nullptr;
}
