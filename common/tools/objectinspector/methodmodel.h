/*
  methodmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
