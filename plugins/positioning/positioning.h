/*
  positioning.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_POSITIONING_H
#define GAMMARAY_POSITIONING_H

#include "positioninginterface.h"

#include <core/toolfactory.h>

#include <QGeoPositionInfoSource>
#include <QObject>

#include <vector>

QT_BEGIN_NAMESPACE
class QGeoPositionInfoSource;
QT_END_NAMESPACE

namespace GammaRay {

class Positioning : public PositioningInterface
{
    Q_OBJECT
public:
    explicit Positioning(Probe *probe, QObject *parent = nullptr);

private slots:
    void objectAdded(QObject *obj);

private:
    static void registerMetaTypes();

    std::vector<QGeoPositionInfoSource *> m_nonProxyPositionInfoSources;
};

class PositioningFactory : public QObject,
                           public StandardToolFactory<QGeoPositionInfoSource, Positioning>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_positioning.json")
public:
    explicit PositioningFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
}

#endif // GAMMARAY_POSITIONING_H
