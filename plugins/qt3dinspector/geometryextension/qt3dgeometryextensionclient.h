/*
  qt3dgeometryextensionclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QT3DGEOMETRYEXTENSIONCLIENT_H
#define GAMMARAY_QT3DGEOMETRYEXTENSIONCLIENT_H

#include "qt3dgeometryextensioninterface.h"

namespace GammaRay {
class Qt3DGeometryExtensionClient : public Qt3DGeometryExtensionInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::Qt3DGeometryExtensionInterface)
public:
    explicit Qt3DGeometryExtensionClient(const QString &name, QObject *parent);
};
}

#endif // GAMMARAY_QT3DGEOMETRYEXTENSIONCLIENT_H
