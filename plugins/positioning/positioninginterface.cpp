/*
  positioninginterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "positioninginterface.h"

#include <common/objectbroker.h>

using namespace GammaRay;

PositioningInterface::PositioningInterface(QObject *parent)
    : QObject(parent)
    , m_positioningOverrideAvailable(false)
    , m_positioningOverrideEnabled(false)
{
    ObjectBroker::registerObject<PositioningInterface *>(this);
}

QGeoPositionInfo PositioningInterface::positionInfo() const
{
    return m_postionInfo;
}

void PositioningInterface::setPositionInfo(const QGeoPositionInfo &info)
{
    if (m_postionInfo == info)
        return;
    m_postionInfo = info;
    emit positionInfoChanged();
}

bool PositioningInterface::positioningOverrideAvailable() const
{
    return m_positioningOverrideAvailable;
}

void PositioningInterface::setPositioningOverrideAvailable(bool available)
{
    if (m_positioningOverrideAvailable == available)
        return;
    m_positioningOverrideAvailable = available;
    emit positioningOverrideAvailableChanged();
}

bool PositioningInterface::positioningOverrideEnabled() const
{
    return m_positioningOverrideEnabled;
}

void PositioningInterface::setPositioningOverrideEnabled(bool enabled)
{
    if (m_positioningOverrideEnabled == enabled)
        return;
    m_positioningOverrideEnabled = enabled;
    emit positioningOverrideEnabledChanged();
}

QGeoPositionInfo PositioningInterface::positionInfoOverride() const
{
    return m_postionInfoOverride;
}

void PositioningInterface::setPositionInfoOverride(const QGeoPositionInfo &info)
{
    if (m_postionInfoOverride == info)
        return;
    m_postionInfoOverride = info;
    emit positionInfoOverrideChanged();
}
