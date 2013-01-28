/*
  hooks.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "probe.h"
#include "functionoverwriterfactory.h"
#include "probecreator.h"

#include <QCoreApplication>

#ifndef Q_OS_WIN
#include <dlfcn.h>
#else
#include <windows.h>
#endif

#include <stdio.h>
#include <cassert>

#ifdef Q_OS_MAC
#include <dlfcn.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/errno.h>
#endif

#define IF_DEBUG(x)

using namespace GammaRay;

bool functionsOverwritten = false;

extern "C" Q_DECL_EXPORT void qt_startup_hook()
{
  Probe::startupHookReceived();

  new ProbeCreator(ProbeCreator::CreateOnly);
#if !defined Q_OS_WIN && !defined Q_OS_MAC
  if (!functionsOverwritten) {
    static void(*next_qt_startup_hook)() = (void (*)()) dlsym(RTLD_NEXT, "qt_startup_hook");
    next_qt_startup_hook();
  }
#endif
}

extern "C" Q_DECL_EXPORT void qt_addObject(QObject *obj)
{
  if (!Probe::isInitialized()) {
    IF_DEBUG(cout
             << "objectAdded Before: "
             << hex << obj
             << (fromCtor ? " (from ctor)" : "") << endl;)
    ProbeCreator::trackObject(obj);
  } else {
    Probe::objectAdded(obj, true);
  }

#if !defined Q_OS_WIN && !defined Q_OS_MAC
  if (!functionsOverwritten) {
    static void (*next_qt_addObject)(QObject *obj) =
      (void (*)(QObject *obj)) dlsym(RTLD_NEXT, "qt_addObject");
    next_qt_addObject(obj);
  }
#endif
}

extern "C" Q_DECL_EXPORT void qt_removeObject(QObject *obj)
{
  if (!Probe::isInitialized()) {
    ProbeCreator::untrackObject(obj);
  } else {
    Probe::objectRemoved(obj);
  }

#if !defined Q_OS_WIN && !defined Q_OS_MAC
  if (!functionsOverwritten) {
    static void (*next_qt_removeObject)(QObject *obj) =
      (void (*)(QObject *obj)) dlsym(RTLD_NEXT, "qt_removeObject");
    next_qt_removeObject(obj);
  }
#endif
}

#ifndef GAMMARAY_UNKNOWN_CXX_MANGLED_NAMES
#ifndef Q_OS_WIN
Q_DECL_EXPORT const char *qFlagLocation(const char *method)
#else
Q_DECL_EXPORT const char *myFlagLocation(const char *method)
#endif
{
  SignalSlotsLocationStore::flagLocation(method);

#ifndef Q_OS_WIN
  static const char *(*next_qFlagLocation)(const char *method) =
    (const char * (*)(const char *method)) dlsym(RTLD_NEXT, "_Z13qFlagLocationPKc");

  Q_ASSERT_X(next_qFlagLocation, "",
             "Recompile with GAMMARAY_UNKNOWN_CXX_MANGLED_NAMES enabled, "
             "your compiler uses an unsupported C++ name mangling scheme");
  return next_qFlagLocation(method);
#else
  return method;
#endif
}
#endif

#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
void overwriteQtFunctions()
{
  functionsOverwritten = true;
  AbstractFunctionOverwriter *overwriter = FunctionOverwriterFactory::createFunctionOverwriter();

  overwriter->overwriteFunction(QLatin1String("qt_startup_hook"), (void*)qt_startup_hook);
  overwriter->overwriteFunction(QLatin1String("qt_addObject"), (void*)qt_addObject);
  overwriter->overwriteFunction(QLatin1String("qt_removeObject"), (void*)qt_removeObject);
#if defined(Q_OS_WIN)
#ifdef ARCH_64
#ifdef __MINGW32__
  overwriter->overwriteFunction(
    QLatin1String("_Z13qFlagLocationPKc"), (void*)myFlagLocation);
#else
  overwriter->overwriteFunction(
    QLatin1String("?qFlagLocation@@YAPEBDPEBD@Z"), (void*)myFlagLocation);
#endif
#else
# ifdef __MINGW32__
  overwriter->overwriteFunction(
    QLatin1String("_Z13qFlagLocationPKc"), (void*)myFlagLocation);
# else
  overwriter->overwriteFunction(
    QLatin1String("?qFlagLocation@@YAPBDPBD@Z"), (void*)myFlagLocation);
# endif
#endif
#endif
}
#endif

#ifdef Q_OS_WIN
extern "C" Q_DECL_EXPORT void gammaray_probe_inject();

extern "C" BOOL WINAPI DllMain(HINSTANCE/*hInstance*/, DWORD dwReason, LPVOID/*lpvReserved*/)
{
  switch(dwReason) {
  case DLL_PROCESS_ATTACH:
  {
    overwriteQtFunctions();

    gammaray_probe_inject();
    break;
  }
  case DLL_PROCESS_DETACH:
  {
      //Unloading does not work, because we overwrite existing code
      exit(-1);
      break;
  }
  };
  return TRUE;
}
#endif

extern "C" Q_DECL_EXPORT void gammaray_probe_inject()
{
  if (!qApp) {
    return;
  }
  printf("gammaray_probe_inject()\n");
  // make it possible to re-attach
  new ProbeCreator(ProbeCreator::CreateAndFindExisting);
}

#ifdef Q_OS_MAC
// we need a way to execute some code upon load, so let's abuse
// static initialization
class HitMeBabyOneMoreTime
{
  public:
    HitMeBabyOneMoreTime()
    {
      overwriteQtFunctions();
    }

};
static HitMeBabyOneMoreTime britney;
#endif
