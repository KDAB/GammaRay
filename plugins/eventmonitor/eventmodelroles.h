/*
  eventmodelroles.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Tim Henning <tim.henning@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_EVENTMODELROLES
#define GAMMARAY_EVENTMODELROLES

#include <common/modelroles.h>

namespace GammaRay {
/** Additional roles for EventModel. */
namespace EventModelRole {
enum Role
{
    AttributesRole = UserRole + 1,
    ReceiverIdRole,
    EventTypeRole
};
}

/** Column indexes of EventModel. */
namespace EventModelColumn {
enum Columns
{
    Time,
    Type,
    Receiver,
    COUNT
};
}
}

#endif // GAMMARAY_EVENTMODELROLES
