/*
  hooks.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
// krazy:excludeall=cpp due to low-level stuff in here

#include <config-gammaray.h>

#include "hooks.h"
#include "probecreator.h"

#include <core/probe.h>

#include <QCoreApplication>

#include <private/qhooks_p.h>

#include <stdio.h> //cannot use cstdio on QNX6.6
#include <cassert>

#ifdef Q_OS_MAC
#include <inttypes.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/errno.h>
#elif defined(Q_OS_WIN)
#include <qt_windows.h>
#endif

#define IF_DEBUG(x)

using namespace GammaRay;

static void log_injection(const char *msg) {
#ifdef Q_OS_WIN
    OutputDebugStringA(msg);
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
    printf(msg); // NOLINT clang-tidy
#pragma GCC diagnostic pop
#endif
}

static void gammaray_pre_routine()
{
#ifdef Q_OS_WIN
    if (qApp) // DllMain will do a better job at this, we are too early here and might not even have our staticMetaObject properly resolved
        return;
#endif
    new ProbeCreator(ProbeCreator::Create | ProbeCreator::FindExistingObjects);
}
Q_COREAPP_STARTUP_FUNCTION(gammaray_pre_routine)

// previously installed Qt hooks, for daisy-chaining
static void (*gammaray_next_startup_hook)() = nullptr;
static void (*gammaray_next_addObject)(QObject *) = nullptr;
static void (*gammaray_next_removeObject)(QObject *) = nullptr;

extern "C" Q_DECL_EXPORT void gammaray_startup_hook()
{
    Probe::startupHookReceived();
    new ProbeCreator(ProbeCreator::Create);

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

static void installQHooks()
{
    Q_ASSERT(qtHookData[QHooks::HookDataVersion] >= 1);
    Q_ASSERT(qtHookData[QHooks::HookDataSize] >= 6);

    gammaray_next_addObject
        = reinterpret_cast<QHooks::AddQObjectCallback>(qtHookData[QHooks::AddQObject]);
    gammaray_next_removeObject
        = reinterpret_cast<QHooks::RemoveQObjectCallback>(qtHookData[QHooks::RemoveQObject]);
    gammaray_next_startup_hook
        = reinterpret_cast<QHooks::StartupCallback>(qtHookData[QHooks::Startup]);

    qtHookData[QHooks::AddQObject] = reinterpret_cast<quintptr>(&gammaray_addObject);
    qtHookData[QHooks::RemoveQObject] = reinterpret_cast<quintptr>(&gammaray_removeObject);
    qtHookData[QHooks::Startup] = reinterpret_cast<quintptr>(&gammaray_startup_hook);
}

bool Hooks::hooksInstalled()
{
    return qtHookData[QHooks::AddQObject] == reinterpret_cast<quintptr>(&gammaray_addObject);
}

void Hooks::installHooks()
{
    if (hooksInstalled())
        return;

    installQHooks();
}

extern "C" Q_DECL_EXPORT void gammaray_probe_inject()
{
    if (!qApp) {
        return;
    }
    Hooks::installHooks();
    log_injection("gammaray_probe_inject()\n");
    new ProbeCreator(ProbeCreator::Create | ProbeCreator::FindExistingObjects);
}

extern "C" Q_DECL_EXPORT void gammaray_probe_attach()
{
    if (!qApp) {
        return;
    }
    log_injection("gammaray_probe_attach()\n");
    new ProbeCreator(ProbeCreator::Create |
                     ProbeCreator::FindExistingObjects |
                     ProbeCreator::ResendServerAddress);
}

extern "C" Q_DECL_EXPORT void gammaray_install_hooks()
{
    Hooks::installHooks();
}
