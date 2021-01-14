/*
  positioning.h

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
    void registerMetaTypes();

    std::vector<QGeoPositionInfoSource*> m_nonProxyPositionInfoSources;
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
