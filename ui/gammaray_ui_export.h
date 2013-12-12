/*
  gammaray_ui_export.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#ifndef GAMMARAY_UI_EXPORT_H
#define GAMMARAY_UI_EXPORT_H

#include <QtCore/qglobal.h>

#ifdef GAMMARAY_UI_STATICLIB
#  undef GAMMARAY_UI_SHAREDLIB
#  define GAMMARAY_UI_EXPORT
#else
#  ifdef MAKE_GAMMARAY_UI_LIB
#    define GAMMARAY_UI_EXPORT Q_DECL_EXPORT
#  else
#    define GAMMARAY_UI_EXPORT Q_DECL_IMPORT
#  endif
#endif

#endif /* GAMMARAY_UI_EXPORT_H */
