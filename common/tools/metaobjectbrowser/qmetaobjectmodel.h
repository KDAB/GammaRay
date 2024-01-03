/*
  qmetaobjectmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_METAOBJECTBROWSER_QMETAOBJECTMODEL_H
#define GAMMARAY_METAOBJECTBROWSER_QMETAOBJECTMODEL_H

#include <common/modelroles.h>

namespace GammaRay {

/*! Column and role definitions for the QMetaObject tree model. */
namespace QMetaObjectModel {
enum Role
{
    MetaObjectRole = UserRole + 1,
    MetaObjectIssues,
    MetaObjectInvalid
};

enum Column
{
    ObjectColumn,
    ObjectSelfCountColumn,
    ObjectInclusiveCountColumn,
    ObjectSelfAliveCountColumn,
    ObjectInclusiveAliveCountColumn,
    _Last
};
}

}

#endif
