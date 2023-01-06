/*
  problemmodelroles.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PROBLEMMODELROLES_H
#define GAMMARAY_PROBLEMMODELROLES_H

#include <common/objectmodel.h>

namespace GammaRay {
namespace ProblemModelRoles {

enum Roles
{
    SeverityRole = ObjectModel::UserRole + 1,
    SourceLocationRole,
    ProblemIdRole
};

}
}

#endif
