/*
  mycylinder.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
