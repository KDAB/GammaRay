/*
  quickitemmodelroles.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKITEMMODELROLES_H
#define GAMMARAY_QUICKINSPECTOR_QUICKITEMMODELROLES_H

#include <common/objectmodel.h>

namespace GammaRay {
/** Model roles shared between client and server. */
namespace QuickItemModelRole {
enum Roles
{
    ItemFlags = ObjectModel::UserRole,
    ItemEvent,
    ItemActions
};

enum ItemFlag
{
    None = 0,
    Invisible = 1,
    ZeroSize = 2,
    PartiallyOutOfView = 4,
    OutOfView = 8,
    HasFocus = 16,
    HasActiveFocus = 32,
    JustRecievedEvent = 64
};
}
}

Q_DECLARE_METATYPE(GammaRay::QuickItemModelRole::ItemFlag)

#endif
