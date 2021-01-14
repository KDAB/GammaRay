/*
  boundingvolume.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
        (m_p1.z() + m_p2.z()) * 0.5f
        );
}

float BoundingVolume::radius() const
{
    const auto dx = m_p1.x() - m_p2.x();
    const auto dy = m_p1.y() - m_p2.y();
    const auto dz = m_p1.z() - m_p2.z();
    return std::sqrt(dx*dx + dy*dy + dz*dz) * 0.5;
}
