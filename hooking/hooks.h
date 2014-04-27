/*
  hooks.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_HOOKS_H
#define GAMMARAY_HOOKS_H

#include <qglobal.h>

class QObject;

#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
#define GAMMARAY_USE_QHOOKS
#endif

#if (defined(Q_OS_WIN) || defined(Q_OS_MAC)) && !defined(GAMMARAY_USE_QHOOKS)
#define GAMMARAY_USE_FUNCTION_OVERWRITE
#endif

extern "C" {

extern Q_DECL_EXPORT void gammaray_startup_hook();
extern Q_DECL_EXPORT void gammaray_addObject(QObject* obj);
extern Q_DECL_EXPORT void gammaray_removeObject(QObject* obj);

/** Entry point for runtime attaching. */
extern Q_DECL_EXPORT void gammaray_probe_inject();

}

extern Q_DECL_EXPORT const char *gammaray_flagLocation(const char *method);

namespace GammaRay {

namespace Hooks {

  /** Returns @c true if we have installed the hooks.
   *  This is useful to avoid loops from preloaded hooks for example.
   */
  bool hooksInstalled();

  /** Install hooks, either by function overwriting or using qhooks. */
  void installHooks();
}

}

#endif
