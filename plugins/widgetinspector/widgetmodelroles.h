/*
  widgetmodelroles.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_WIDGETINSPECTOR_WIDGETMODELROLES_H
#define GAMMARAY_WIDGETINSPECTOR_WIDGETMODELROLES_H

#include <common/objectmodel.h>

namespace GammaRay {
/** Model roles shared between client and server. */
namespace WidgetModelRoles {
enum Roles
{
    WidgetFlags = ObjectModel::UserRole,
};

enum WidgetFlag
{
    None = 0,
    Invisible = 1
};
}
}

Q_DECLARE_METATYPE(GammaRay::WidgetModelRoles::WidgetFlag)

#endif // GAMMARAY_WIDGETINSPECTOR_WIDGETMODELROLES_H
