/*
  connectionsmodelroles.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
