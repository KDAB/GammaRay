/*
  modelroles.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_MODELROLES_H
#define GAMMARAY_MODELROLES_H

#include <QAbstractItemModel>

/**
 * @file modelroles.h A collection of custom model roles shared between client and server.
 */
namespace GammaRay {

static const int UserRole = 256; // Qt4 uses 32, Qt5 256, use the latter globally to allow combining Qt4/5 client/servers.

/** Custom roles for GammaRay::ToolModel.
 * @todo These can be split again, between core tool model and UI tool model.
 */
namespace ToolModelRole {
  enum Role {
    ToolFactory = UserRole + 1,
    ToolWidget,
    ToolId,
    ToolWidgetParent
  };
}

/** Custom roles for GammaRay::ObjectMethodModel. */
namespace ObjectMethodModelRole {
  enum Role {
    MetaMethod = UserRole + 1,
    MetaMethodType
  };
}

}

#endif
