/*
  utils.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

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
  This file is part of the GammaRay Common API and declares various utility
  methods needed when writing shared code.

  @brief
  Declares various utility methods needed when writing shared code.

  @author Filipe Azevedo \<filipe.azevedo@kdab.com\>
*/

#ifndef GAMMARAY_UI_UTILS_H
#define GAMMARAY_UI_UTILS_H

#include "gammaray_ui_export.h"

#include <QColor>

namespace GammaRay {
/**
 * @brief GammaRay UI utilities.
 */
namespace Utils {
/**
 * Returns true if the given color is ligth, else false.
 * @param color is the color to check.
 *
 * @return a bool telling if the given color is light or not.
 */
GAMMARAY_UI_EXPORT bool isLightColor(const QColor &color);
/**
 * Returns true if the given color is dark, else false.
 * @param color is the color to check.
 *
 * @return a bool telling if the given color is dark or not.
 */
GAMMARAY_UI_EXPORT bool isDarkColor(const QColor &color);
}
}

#endif // GAMMARAY_UI_UTILS_H
