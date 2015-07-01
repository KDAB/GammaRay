/*
  hooks.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
//krazy:excludeall=cpp due to low-level stuff in here

#include <config-gammaray.h>

#include "hooks.h"
#include "functionoverwriterfactory.h"
#include "probecreator.h"

#include <core/probe.h>

#include <QCoreApplication>

#ifdef GAMMARAY_USE_QHOOKS
#include <private/qhooks_p.h>
#endif

#include <stdio.h>
#include <cassert>

#ifdef Q_OS_MAC
#include <inttypes.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/errno.h>
#endif

#define IF_DEBUG(x)

using namespace GammaRay;

// previously installed Qt hooks, for daisy-chaining
static void(*gammaray_next_startup_hook)() = 0;
static void(*gammaray_next_addObject)(QObject*) = 0;
static void(*gammaray_next_removeObject)(QObject*) = 0;

extern "C" Q_DECL_EXPORT void gammaray_startup_hook()
{
  Probe::startupHookReceived();
  new ProbeCreator(ProbeCreator::CreateOnly);

  if (gammaray_next_startup_hook)
    gammaray_next_startup_hook();
}

extern "C" Q_DECL_EXPORT void gammaray_addObject(QObject *obj)
{
  Probe::objectAdded(obj, true);

  if (gammaray_next_addObject)
    gammaray_next_addObject(obj);
}

extern "C" Q_DECL_EXPORT void gammaray_removeObject(QObject *obj)
{
  Probe::objectRemoved(obj);

  if (gammaray_next_removeObject)
    gammaray_next_removeObject(obj);
}

const char* gammaray_flagLocation(const char* method)
{
  SignalSlotsLocationStore::flagLocation(method);
  return method;
}

#ifdef GAMMARAY_USE_QHOOKS
static void installQHooks()
{
  Q_ASSERT(qtHookData[QHooks::HookDataVersion] >= 1);
  Q_ASSERT(qtHookData[QHooks::HookDataSize] >= 6);

  gammaray_next_addObject = reinterpret_cast<QHooks::AddQObjectCallback>(qtHookData[QHooks::AddQObject]);
  gammaray_next_removeObject = reinterpret_cast<QHooks::RemoveQObjectCallback>(qtHookData[QHooks::RemoveQObject]);
  gammaray_next_startup_hook = reinterpret_cast<QHooks::StartupCallback>(qtHookData[QHooks::Startup]);

  qtHookData[QHooks::AddQObject] = reinterpret_cast<quintptr>(&gammaray_addObject);
  qtHookData[QHooks::RemoveQObject] = reinterpret_cast<quintptr>(&gammaray_removeObject);
  qtHookData[QHooks::Startup] = reinterpret_cast<quintptr>(&gammaray_startup_hook);
}
#endif

#ifdef GAMMARAY_USE_FUNCTION_OVERWRITE
static bool functionsOverwritten = false;

static void overwriteQtFunctions()
{
  functionsOverwritten = true;
  AbstractFunctionOverwriter *overwriter = FunctionOverwriterFactory::createFunctionOverwriter();

  overwriter->overwriteFunction(QLatin1String("qt_startup_hook"), (void*)gammaray_startup_hook);
  overwriter->overwriteFunction(QLatin1String("qt_addObject"), (void*)gammaray_addObject);
  overwriter->overwriteFunction(QLatin1String("qt_removeObject"), (void*)gammaray_removeObject);
#if defined(Q_OS_WIN)
#ifdef ARCH_64
#ifdef __MINGW32__
  overwriter->overwriteFunction(
    QLatin1String("_Z13qFlagLocationPKc"), (void*)gammaray_flagLocation);
#else
  overwriter->overwriteFunction(
    QLatin1String("?qFlagLocation@@YAPEBDPEBD@Z"), (void*)gammaray_flagLocation);
#endif
#else
# ifdef __MINGW32__
  overwriter->overwriteFunction(
    QLatin1String("_Z13qFlagLocationPKc"), (void*)gammaray_flagLocation);
# else
  overwriter->overwriteFunction(
    QLatin1String("?qFlagLocation@@YAPBDPBD@Z"), (void*)gammaray_flagLocation);
# endif
#endif
#endif
}
#endif

bool Hooks::hooksInstalled()
{
#ifdef GAMMARAY_USE_QHOOKS
  return qtHookData[QHooks::AddQObject] == reinterpret_cast<quintptr>(&gammaray_addObject);
#elif defined(GAMMARAY_USE_FUNCTION_OVERWRITE)
  return functionsOverwritten;
#else
  return false;
#endif
}

void Hooks::installHooks()
{
  if (hooksInstalled())
    return;

#ifdef GAMMARAY_USE_QHOOKS
  installQHooks();
#elif defined(GAMMARAY_USE_FUNCTION_OVERWRITE)
  overwriteQtFunctions();
#endif
}

extern "C" Q_DECL_EXPORT void gammaray_probe_inject()
{
  if (!qApp) {
    return;
  }
  printf("gammaray_probe_inject()\n");
  // make it possible to re-attach
  new ProbeCreator(ProbeCreator::CreateAndFindExisting);
}
