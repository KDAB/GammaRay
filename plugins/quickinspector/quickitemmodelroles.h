/*
  quickitemmodelroles.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKITEMMODELROLES_H
#define GAMMARAY_QUICKINSPECTOR_QUICKITEMMODELROLES_H

#include <common/objectmodel.h>

namespace GammaRay {

/** Model roles shared between client and server. */
namespace QuickItemModelRole {
  enum Roles {
    ItemFlags = ObjectModel::UserRole
  };

  enum ItemFlag {
    None = 0,
    Invisible = 1,
    ZeroSize = 2,
    OutOfView = 4,
    HasFocus = 8,
    HasActiveFocus = 16,
    JustRecievedEvent = 32
  };
}

}

Q_DECLARE_METATYPE(GammaRay::QuickItemModelRole::ItemFlag)

#endif
