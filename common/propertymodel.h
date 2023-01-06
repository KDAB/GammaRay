/*
  propertymodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PROPERTYMODEL_H
#define GAMMARAY_PROPERTYMODEL_H

#include "modelroles.h"

#include <QMetaType>
#include <QFlags>

namespace GammaRay {
/**
 * @brief GammaRay property model roles.
 *
 * Public property model roles, for use by tool plugins without needing access
 * to the real property model classes.
 */
namespace PropertyModel {
/** Role enum, to be used with the property models. */
enum Role
{
    ActionRole = GammaRay::UserRole + 1, /**< the property action role */
    DeprecatedRole, /**< @deprecated do not use */
    ObjectIdRole,
    ResetActionRole,
    PropertyFlagsRole,
    PropertyRevisionRole,
    NotifySignalRole
};

/** Available property actions. */
enum Action
{
    NoAction = 0,
    Delete = 1,
    Reset = 2,
    NavigateTo = 4
};

/** Available columns. */
enum Column
{
    PropertyColumn = 0,
    ValueColumn,
    TypeColumn,
    ClassColumn
};

/** Property flags.
 *  @see QMetaProperty
 */
enum PropertyFlag
{
    None = 0,
    Constant = 1,
    Designable = 2,
    Final = 4,
    Resetable = 8,
    Scriptable = 16,
    Stored = 32,
    User = 64,
    Writable = 128
};
Q_DECLARE_FLAGS(PropertyFlags, PropertyFlag)

}
}

Q_DECLARE_OPERATORS_FOR_FLAGS(GammaRay::PropertyModel::PropertyFlags)
Q_DECLARE_METATYPE(GammaRay::PropertyModel::PropertyFlags)

#endif
