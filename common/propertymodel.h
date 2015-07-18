/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  acuordance with GammaRay Commercial License Agreement provided with the Software.

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

#ifndef GAMMARAY_PROPERTYMODEL_H
#define GAMMARAY_PROPERTYMODEL_H

namespace GammaRay {

/**
 * @brief GammaRay property model roles.
 *
 * Public property model roles, for use by tool plugins without needing access
 * to the real property model classes.
 */
namespace PropertyModel {

    /** Role enum, to be used with the property models. */
    enum Role {
      // Qt4 uses 32, Qt5 256, for Qt::UserRole - use the latter globally to allow combining Qt4/5 client/servers.
      ActionRole = 256 + 1, /**< the property action role */
      UserRole ,             /**< the UserRole, as defined by Qt */
      ValueRole,
      AppropriateToolRole,
      ResetActionRole
    };

    /** Available property actions. */
    enum Action {
      NoAction = 0,
      Delete = 1,
      Reset = 2,
      NavigateTo = 4
    };
}

}

#endif
