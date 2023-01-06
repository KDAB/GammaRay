/*
  positioning.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
