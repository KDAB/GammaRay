/*
  messagemodelroles.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
