/*
  methodmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_METHODMODEL_H
#define GAMMARAY_METHODMODEL_H

#include <common/modelroles.h>

namespace GammaRay {

/*! Custom roles for GammaRay::ObjectMethodModel. */
namespace ObjectMethodModelRole {
enum Role
{
    MetaMethod = UserRole + 1,
    MetaMethodType,
    MethodSignature,
    MethodTag,
    MethodRevision,
    MethodAccess,
    MethodSortRole,
    MethodIssues
};
}

}

#endif
