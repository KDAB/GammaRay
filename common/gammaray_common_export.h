/*
  gammaray_common_export.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  acuordance with GammaRay Commercial License Agreement provided with the Software.

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

#ifndef GAMMARAY_COMMON_EXPORT_H
#define GAMMARAY_COMMON_EXPORT_H

#include <QtCore/qglobal.h>

#ifdef GAMMARAY_COMMON_STATICLIB
#  undef GAMMARAY_COMMON_SHAREDLIB
#  define GAMMARAY_COMMON_EXPORT
#else
#  ifdef MAKE_GAMMARAY_COMMON_LIB
#    define GAMMARAY_COMMON_EXPORT Q_DECL_EXPORT
#  else
#    define GAMMARAY_COMMON_EXPORT Q_DECL_IMPORT
#  endif
#endif

#endif /* GAMMARAY_COMMON_EXPORT_H */
