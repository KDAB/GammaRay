/*
  remotemodelroles.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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

/*! Marker that is used to restore message reading if the stream is unreadable */
static const char REMOTE_MODEL_MARKER[] = "REMOTE_MODEL_MARKER";
}

Q_DECLARE_OPERATORS_FOR_FLAGS(GammaRay::RemoteModelNodeState::NodeStates)
Q_DECLARE_METATYPE(GammaRay::RemoteModelNodeState::NodeStates)

#endif
