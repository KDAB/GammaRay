/*
  hooks.cpp

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

extern "C" Q_DECL_EXPORT void gammaray_startup_hook()
{
  Probe::startupHookReceived();

  new ProbeCreator(ProbeCreator::CreateOnly);
}

extern "C" Q_DECL_EXPORT void gammaray_addObject(QObject *obj)
{
  Probe::objectAdded(obj, true);
}

extern "C" Q_DECL_EXPORT void gammaray_removeObject(QObject *obj)
{
  Probe::objectRemoved(obj);
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

  if (qtHookData[QHooks::AddQObject] || qtHookData[QHooks::RemoveQObject] || qtHookData[QHooks::Startup])
    qFatal("There is another debugging tool using QHooks already, this is not yet supported!\n");

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
