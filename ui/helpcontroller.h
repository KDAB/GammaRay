/*
  helpcontroller.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_HELPCONTROLLER_H
#define GAMMARAY_HELPCONTROLLER_H

#include "gammaray_ui_export.h"

#include <qglobal.h>

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

namespace GammaRay {
/*! Controls the Assistant-based help browser. */
namespace HelpController {
/*! Returns @c true if Assistant and our help collection are found. */
GAMMARAY_UI_EXPORT bool isAvailable();

/*! Open start page of the help collection. */
GAMMARAY_UI_EXPORT void openContents();

/*! Opens the specified page of the help collection. */
GAMMARAY_UI_EXPORT void openPage(const QString &page);
}
}

#endif // GAMMARAY_HELPCONTROLLER_H
