/*
  objectmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
enum Role
{
    ObjectRole = GammaRay::UserRole + 1, /**< Pointer to the represented object (available in the probe code only). */
    ObjectIdRole, /**< ObjectId instance for represented object. */
    DecorationIdRole, /**< The classes icon id to display as Qt::DecorationRole (see ClientDecorationIdentityProxyModel). */
    IsFavoriteRole,
    UserRole, /**< UserRole, as defined by Qt. */

    CreationLocationRole = 9999, /**< Source location where this object was created, if known. */
    DeclarationLocationRole, /**< Source location where the type for this object has been declared, if known. */
};
}
}

#endif
