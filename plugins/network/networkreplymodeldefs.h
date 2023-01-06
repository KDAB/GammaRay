/*
  networkreplymodeldefs.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2019-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_NETWORKREPLYMODELDEFS_H
#define GAMMARAY_NETWORKREPLYMODELDEFS_H

#include <common/modelroles.h>

namespace GammaRay {

namespace NetworkReply {
enum State
{
    Running = 0,
    Error = 1,
    Finished = 2,
    Encrypted = 4, // explicitly known to be encrypted
    Unencrypted = 8, // explicitly known to not be encrypted, if none are set, we guess from URL
    Deleted = 16
};

enum ContentType
{
    Unknown = 0,
    Json = 1,
};
}

namespace NetworkReplyModelRole {
enum Role
{
    ReplyStateRole = GammaRay::UserRole,
    ReplyErrorRole,
    ObjectIdRole,
    ReplyResponseRole,
    ReplyContentType,
};
}

namespace NetworkReplyModelColumn {
enum Column
{
    ObjectColumn = 0,
    OpColumn = 1,
    TimeColumn = 2,
    SizeColumn = 3,
    UrlColumn = 4,
    COLUMN_COUNT
};
}

}

#endif // GAMMARAY_NETWORKREPLYMODELDEFS_H
