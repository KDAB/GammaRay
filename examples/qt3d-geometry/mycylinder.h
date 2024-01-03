/*
  mycylinder.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef MYCYLINDER_H
#define MYCYLINDER_H

#include <Qt3DRender/QGeometryRenderer>

class MyCylinder : public Qt3DRender::QGeometryRenderer
{
    Q_OBJECT
public:
    explicit MyCylinder(Qt3DCore::QNode *parent = nullptr);
    ~MyCylinder();
};

#endif // MYCYLINDER_H
