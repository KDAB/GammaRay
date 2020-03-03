/*
  commonutils.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Hannah von Reth <hannah.vonreth@kdab.com>

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

#ifndef GAMMARAY_COMMON_UTILS_H
#define GAMMARAY_COMMON_UTILS_H

#include "gammaray_common_export.h"

#include <QtGlobal>


#define WIN_ERROR_ASSERT(condition, action) if (condition) {} else { qWarning("%s Error: %s failed: %s", Q_FUNC_INFO, #condition, qPrintable(qt_error_string())); action; } do {} while(false)
#define WIN_ERROR_CHECK(condition) WIN_ERROR_ASSERT(condition, qt_noop();)

namespace GammaRay {
namespace CommonUtils {

}
}

#endif // GAMMARAY_COMMON_UTILS_H
