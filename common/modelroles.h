/*
  modelroles.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_MODELROLES_H
#define GAMMARAY_MODELROLES_H

/**
 * @file modelroles.h
 * @brief A collection of custom model roles shared between client and server.
 */
namespace GammaRay {
/*! Portable replacement for Qt::UserRole.
 * Qt4 uses 32, Qt5 256, use the latter globally to allow combining Qt4/5 client/servers.
 */
static const int UserRole = 256;

/*! Custom roles for GammaRay::ToolModel.
 * @todo These can be split again, between core tool model and UI tool model.
 */
namespace ToolModelRole {
/*! Tool model data roles. */
enum Role {
    ToolFactory = UserRole + 1,
    ToolWidget,
    ToolId,
    ToolWidgetParent,
    ToolEnabled,
    ToolHasUi,
    ToolFeedbackId
};
}
}

#endif
