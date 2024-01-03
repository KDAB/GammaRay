/*
  timezonemodelroles.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_TIMEZONEMODELROLES_H
#define GAMMARAY_TIMEZONEMODELROLES_H

#include <common/modelroles.h>

namespace GammaRay {

namespace TimezoneModelRoles {
enum Role
{
    LocalZoneRole = GammaRay::UserRole
};
}

namespace TimezoneModelColumns {
enum Column
{
    IanaIdColumn,
    CountryColumn,
    StandardDisplayNameColumn,
    DSTColumn,
    WindowsIdColumn,
    COUNT
};
}

}

#endif
