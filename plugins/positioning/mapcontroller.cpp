/*
  mapcontroller.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "mapcontroller.h"

using namespace GammaRay;

MapController::MapController(QObject *parent)
    : QObject(parent)
    , m_sourceHorizontalAccuracy(.0)
    , m_sourceDirection(.0)
    , m_overrideHorizontalAccuracy(.0)
    , m_overrideDirection(.0)
    , m_overrideEnabled(false)
{
}

MapController::~MapController() = default;

QGeoCoordinate MapController::sourceCoordinate() const
{
    return m_sourceCoordinate;
}

void MapController::setSourceCoordinate(const QGeoCoordinate &coord)
{
    if (m_sourceCoordinate == coord)
        return;
    m_sourceCoordinate = coord;
    emit sourceChanged();
}

double MapController::sourceHorizontalAccuracy() const
{
    return m_sourceHorizontalAccuracy;
}

void MapController::setSourceHorizontalAccuracy(double accu)
{
    if (m_sourceHorizontalAccuracy == accu)
        return;
    m_sourceHorizontalAccuracy = accu;
    emit sourceChanged();
}

double MapController::sourceDirection() const
{
    return m_sourceDirection;
}

void MapController::setSourceDirection(double dir)
{
    if (m_sourceDirection == dir)
        return;
    m_sourceDirection = dir;
    emit sourceChanged();
}

bool MapController::overrideEnabled() const
{
    return m_overrideEnabled;
}

void MapController::setOverrideEnabled(bool enabled)
{
    if (m_overrideEnabled == enabled)
        return;
    m_overrideEnabled = enabled;
    emit overrideEnabledChanged();
}

QGeoCoordinate MapController::overrideCoordinate() const
{
    return m_overrideCoordinate;
}

void MapController::setOverrideCoordinate(const QGeoCoordinate &coord)
{
    if (m_overrideCoordinate == coord)
        return;
    m_overrideCoordinate = coord;
    emit overrideCoordinateChanged();
}

double MapController::overrideHorizontalAccuracy() const
{
    return m_overrideHorizontalAccuracy;
}

void MapController::setOverrideHorizontalAccuracy(double accu)
{
    if (m_overrideHorizontalAccuracy == accu)
        return;
    m_overrideHorizontalAccuracy = accu;
    emit overrideHorizontalAccuracyChanged();
}

double MapController::overrideDirection() const
{
    return m_overrideDirection;
}

void MapController::setOverrideDirection(double dir)
{
    if (m_overrideDirection == dir)
        return;
    m_overrideDirection = dir;
    emit overrideDirectionChanged();
}
