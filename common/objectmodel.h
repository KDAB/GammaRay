/*
  objectmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
/**
  @file
  This file is part of the GammaRay Plugin API and declares the public object model roles.

  @brief
  Declares the public object model roles.

  @author Volker Krause \<volker.krause@kdab.com\>
*/

#ifndef GAMMARAY_OBJECTMODEL_H
#define GAMMARAY_OBJECTMODEL_H

#include <Qt>

namespace GammaRay {

/**
 * @brief GammaRay Object Models.
 *
 * Public object model roles, for use by tool plugins without needing access
 * to the real object model classes.
 */
namespace ObjectModel {

    /** Role enum, to be used with the object list and tree models. */
    enum Role {
      // Qt4 uses 32, Qt5 256, for Qt::UserRole - use the latter globally to allow combining Qt4/5 client/servers.
      ObjectRole = 256 + 1, /**< the Object role */
      UserRole                       /**< the UserRole, as defined by Qt */
    };
}

}

#endif
