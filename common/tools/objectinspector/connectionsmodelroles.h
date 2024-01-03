/*
  connectionsmodelroles.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_CONNECTIONSMODELROLES_H
#define GAMMARAY_CONNECTIONSMODELROLES_H

#include <common/modelroles.h>

namespace GammaRay {
/** @brief Roles for the object inspector connections models. */
namespace ConnectionsModelRoles {
enum Role
{
    WarningFlagRole = UserRole + 1,
    EndpointRole,
    ActionRole
};
}

/** @brief Connection actions.
 * Returns via ActionRole from the connections models.
 */
namespace ConnectionsModelActions {
enum Action
{
    NoAction = 0,
    NavigateToEndpoint = 1
};
}
}

#endif
