/*
  preload.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <config-gammaray.h>

#include "hooks.h"

#include <dlfcn.h>

using namespace GammaRay;

extern "C" Q_DECL_EXPORT void qt_startup_hook()
{
  gammaray_startup_hook();

#if !defined Q_OS_MAC
  if (!Hooks::hooksInstalled()) {
    static void(*next_qt_startup_hook)() = (void (*)()) dlsym(RTLD_NEXT, "qt_startup_hook");
    next_qt_startup_hook();
  }
#endif
}

extern "C" Q_DECL_EXPORT void qt_addObject(QObject *obj)
{
  gammaray_addObject(obj);

#if !defined Q_OS_MAC
  if (!Hooks::hooksInstalled()) {
    static void (*next_qt_addObject)(QObject *obj) =
    (void (*)(QObject *obj)) dlsym(RTLD_NEXT, "qt_addObject");
    next_qt_addObject(obj);
  }
#endif
}

extern "C" Q_DECL_EXPORT void qt_removeObject(QObject *obj)
{
  gammaray_removeObject(obj);

#if !defined Q_OS_WIN && !defined Q_OS_MAC
  if (!Hooks::hooksInstalled()) {
    static void (*next_qt_removeObject)(QObject *obj) =
    (void (*)(QObject *obj)) dlsym(RTLD_NEXT, "qt_removeObject");
    next_qt_removeObject(obj);
  }
#endif
}

#ifndef GAMMARAY_UNKNOWN_CXX_MANGLED_NAMES
Q_DECL_EXPORT const char *qFlagLocation(const char *method)
{
  gammaray_flagLocation(method);

  static const char *(*next_qFlagLocation)(const char *method) =
  (const char * (*)(const char *method)) dlsym(RTLD_NEXT, "_Z13qFlagLocationPKc");

  Q_ASSERT_X(next_qFlagLocation, "",
             "Recompile with GAMMARAY_UNKNOWN_CXX_MANGLED_NAMES enabled, "
             "your compiler uses an unsupported C++ name mangling scheme");
  return next_qFlagLocation(method);
}
#endif

