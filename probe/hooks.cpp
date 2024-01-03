/*
  hooks.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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

static void log_injection(const char *msg)
{
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
} // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)
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
} // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)

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

    gammaray_next_addObject = reinterpret_cast<QHooks::AddQObjectCallback>(qtHookData[QHooks::AddQObject]);
    gammaray_next_removeObject = reinterpret_cast<QHooks::RemoveQObjectCallback>(qtHookData[QHooks::RemoveQObject]);
    gammaray_next_startup_hook = reinterpret_cast<QHooks::StartupCallback>(qtHookData[QHooks::Startup]);

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
} // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)

extern "C" Q_DECL_EXPORT void gammaray_probe_attach()
{
    if (!qApp) {
        return;
    }
    log_injection("gammaray_probe_attach()\n");
    new ProbeCreator(ProbeCreator::Create | ProbeCreator::FindExistingObjects | ProbeCreator::ResendServerAddress);
} // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)

extern "C" Q_DECL_EXPORT void gammaray_install_hooks()
{
    Hooks::installHooks();
}
