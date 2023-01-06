/*
  boundingvolume.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "boundingvolume.h"

#include <cmath>

using namespace GammaRay;

void BoundingVolume::addPoint(const QVector3D &p)
{
    if (m_null) {
        m_null = false;
        m_p1 = p;
        m_p2 = p;
    } else {
        m_p1.setX(std::max(m_p1.x(), p.x()));
        m_p1.setY(std::max(m_p1.y(), p.y()));
        m_p1.setZ(std::max(m_p1.z(), p.z()));
        m_p2.setX(std::min(m_p2.x(), p.x()));
        m_p2.setY(std::min(m_p2.y(), p.y()));
        m_p2.setZ(std::min(m_p2.z(), p.z()));
    }
}

QVector3D BoundingVolume::center() const
{
    return QVector3D(
        (m_p1.x() + m_p2.x()) * 0.5f,
        (m_p1.y() + m_p2.y()) * 0.5f,
        (m_p1.z() + m_p2.z()) * 0.5f);
}

float BoundingVolume::radius() const
{
    const auto dx = m_p1.x() - m_p2.x();
    const auto dy = m_p1.y() - m_p2.y();
    const auto dz = m_p1.z() - m_p2.z();
    return std::sqrt(dx * dx + dy * dy + dz * dz) * 0.5;
}
