/*
  injectorfactory.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "injectorfactory.h"

#include "styleinjector.h"
#ifdef Q_OS_WIN
#include "windllinjector.h"
#else
#include "gdbinjector.h"
#include "lldbinjector.h"
#include "preloadinjector.h"
#endif

#include <launcher/core/probeabi.h>

#include <QString>

namespace GammaRay {
namespace InjectorFactory {
AbstractInjector::Ptr createInjector(const QString &name, const QString &executableOverride)
{
#ifndef Q_OS_WIN
    if (name == QLatin1String("gdb")) {
        return AbstractInjector::Ptr(new GdbInjector(executableOverride));
    }
    if (name == QLatin1String("lldb")) {
        return AbstractInjector::Ptr(new LldbInjector(executableOverride));
    }
#else
    Q_UNUSED(executableOverride);
#endif

    if (name == QLatin1String("style")) {
        return AbstractInjector::Ptr(new StyleInjector);
    }

#ifndef Q_OS_WIN
    if (name == QLatin1String("preload")) {
        return AbstractInjector::Ptr(new PreloadInjector);
    }
#else
    if (name == QLatin1String("windll")) {
        return AbstractInjector::Ptr(new WinDllInjector);
    }
#endif
    return AbstractInjector::Ptr(nullptr);
}

#if !defined(Q_OS_WIN)
static AbstractInjector::Ptr findFirstWorkingInjector(const QStringList &types, QStringList *errorStrings)
{
    for (const QString &type : types) {
        AbstractInjector::Ptr injector = createInjector(type);
        if (injector) {
            if (injector->selfTest()) {
                return injector;
            } else if (errorStrings) {
                *errorStrings << QStringLiteral("%1: %2").arg(type, injector->errorString());
            }
        } else if (errorStrings) {
            *errorStrings << QStringLiteral("No valid injector type: %1").arg(type);
        }
    }

    return AbstractInjector::Ptr(nullptr);
}

#endif

AbstractInjector::Ptr defaultInjectorForLaunch(const ProbeABI &abi, QStringList *errorStrings)
{
#if defined(Q_OS_MAC)
    if (abi.majorQtVersion() >= 5 && abi.minorQtVersion() >= 4) {
        return createInjector(QStringLiteral("preload"));
    }
    return findFirstWorkingInjector(QStringList() << QStringLiteral("lldb")
                                                  << QStringLiteral("gdb"),
                                    errorStrings);
#elif defined(Q_OS_UNIX)
    Q_UNUSED(abi);
    Q_UNUSED(errorStrings);
    return createInjector(QStringLiteral("preload"));
#else
    Q_UNUSED(abi);
    Q_UNUSED(errorStrings);
    return createInjector(QStringLiteral("windll"));
#endif
}

AbstractInjector::Ptr defaultInjectorForAttach(QStringList *errorStrings)
{
#if defined(Q_OS_MAC)
    return findFirstWorkingInjector(QStringList() << QStringLiteral("lldb")
                                                  << QStringLiteral("gdb"),
                                    errorStrings);
#elif !defined(Q_OS_WIN)
    return findFirstWorkingInjector(QStringList() << QStringLiteral("gdb")
                                                  << QStringLiteral("lldb"),
                                    errorStrings);
#else
    Q_UNUSED(errorStrings);
    return createInjector(QStringLiteral("windll"));
#endif
}

QStringList availableInjectors()
{
    QStringList types;
#ifndef Q_OS_WIN
    types << QStringLiteral("preload") << QStringLiteral("gdb") << QStringLiteral("lldb");
#else
    types << QStringLiteral("windll");
#endif
    types << QStringLiteral("style");
    return types;
}
}
}
