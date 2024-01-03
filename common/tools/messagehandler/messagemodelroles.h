/*
  messagemodelroles.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_MESSAGEMODELROLES
#define GAMMARAY_MESSAGEMODELROLES

#include <common/modelroles.h>

namespace GammaRay {
/** Additional roles for MessageModel. */
namespace MessageModelRole {
enum Role
{
    Sort = UserRole + 1, // not for remoting
    Type,
    File,
    Line,
    Backtrace
};
}

/** Column indexes of MessageModel. */
namespace MessageModelColumn {
enum Columns
{
    Time,
    Message,
    Category,
    Function,
    File,
    COUNT
};
}
}

#endif
