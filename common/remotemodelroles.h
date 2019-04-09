/*
  remotemodelroles.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
    enum Roles {
        LoadingState = RemoteModelUserRole + 1
    };
}

/*! Cell loading state of RemoteModel. */
namespace RemoteModelNodeState {
    enum NodeState {
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

