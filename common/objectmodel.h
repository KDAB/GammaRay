/*
  objectmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
/**
  @file
  This file is part of the GammaRay Plugin API and declares the public object model roles.

  @brief
  Declares the public object model roles.

  @author Volker Krause \<volker.krause@kdab.com\>
*/

#ifndef GAMMARAY_OBJECTMODEL_H
#define GAMMARAY_OBJECTMODEL_H

#include "modelroles.h"

namespace GammaRay {
/*!
 * GammaRay Object Models.
 *
 * Public object model roles, for use by tool plugins without needing access
 * to the real object model classes.
 */
namespace ObjectModel {
/*! Role enum, to be used with the object list and tree models. */
enum Role {
    ObjectRole = GammaRay::UserRole + 1,   /**< Pointer to the represented object (available in the probe code only). */
    ObjectIdRole,           /**< ObjectId instance for represented object. */
    CreationLocationRole,   /**< Source location where this object was created, if known. */
    DeclarationLocationRole,/**< Source location where the type for this object has been declared, if known. */
    DecorationIdRole,       /**< The classes icon id to display as Qt::DecorationRole (see ClientDecorationIdentityProxyModel). */
    UserRole                /**< UserRole, as defined by Qt. */
};
}
}

#endif
