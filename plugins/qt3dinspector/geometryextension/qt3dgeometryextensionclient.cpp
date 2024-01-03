/*
  qt3dgeometryextensionclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "qt3dgeometryextensionclient.h"

using namespace GammaRay;

Qt3DGeometryExtensionClient::Qt3DGeometryExtensionClient(const QString &name, QObject *parent)
    : Qt3DGeometryExtensionInterface(name, parent)
{
}
