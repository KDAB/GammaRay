/*
  boundingvolume.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_BOUNDINGVOLUME_H
#define GAMMARAY_BOUNDINGVOLUME_H

#include <QVector3D>

namespace GammaRay {
class BoundingVolume
{
public:
    BoundingVolume() = default;
    void addPoint(const QVector3D &p);

    QVector3D center() const;
    float radius() const;

private:
    QVector3D m_p1;
    QVector3D m_p2;
    bool m_null = true;
};
}

#endif // GAMMARAY_BOUNDINGVOLUME_H
