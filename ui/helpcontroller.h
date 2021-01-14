/*
  helpcontroller.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
