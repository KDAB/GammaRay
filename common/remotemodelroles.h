/*
  remotemodelroles.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_REMOTEMODELROLES_H
#define GAMMARAY_REMOTEMODELROLES_H

#include <QFlags>
#include <QMetaType>

namespace GammaRay {

/*! Starting point for remoting-related roles. */
static const int RemoteModelUserRole = 0xFFFFFF;

/*! Custom roles for RemoteModel. */
namespace RemoteModelRole {
enum Roles
{
    LoadingState = RemoteModelUserRole + 1
};
}

/*! Cell loading state of RemoteModel. */
namespace RemoteModelNodeState {
enum NodeState
{
    NoState = 0,
    Empty = 1,
    Loading = 2,
    Outdated = 4
};
Q_DECLARE_FLAGS(NodeStates, NodeState)
}

}

Q_DECLARE_OPERATORS_FOR_FLAGS(GammaRay::RemoteModelNodeState::NodeStates)
Q_DECLARE_METATYPE(GammaRay::RemoteModelNodeState::NodeStates)

#endif
