/*
  paintbuffermodelroles.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PAINTBUFFERMODELROLES_H
#define GAMMARAY_PAINTBUFFERMODELROLES_H

#include "modelroles.h"

namespace GammaRay {
namespace PaintBufferModelRoles {
enum Role
{
    ValueRole = GammaRay::UserRole + 1,
    ClipPathRole,
    MaxCostRole,
    ObjectIdRole,
};
}
}

#endif
