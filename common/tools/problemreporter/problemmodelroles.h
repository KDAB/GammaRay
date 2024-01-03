/*
  problemmodelroles.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
