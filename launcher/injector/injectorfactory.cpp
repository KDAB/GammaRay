/*
  injectorfactory.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "injectorfactory.h"

#include "gdbinjector.h"
#include "lldbinjector.h"
#include "preloadinjector.h"
#include "styleinjector.h"
#include "windllinjector.h"

#include <launcher/probeabi.h>

#include <QString>

namespace GammaRay {

namespace InjectorFactory {

AbstractInjector::Ptr createInjector(const QString &name)
{
#ifndef Q_OS_WIN
  if (name == QLatin1String("gdb")) {
    return AbstractInjector::Ptr(new GdbInjector);
  }
  if (name == QLatin1String("lldb")) {
    return AbstractInjector::Ptr(new LldbInjector);
  }
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
  return AbstractInjector::Ptr(0);
}

#if !defined(Q_OS_WIN)
static AbstractInjector::Ptr findFirstWorkingInjector(const QStringList &types)
{
  foreach (const QString &type, types) {
    AbstractInjector::Ptr injector = createInjector(type);
    if (injector->selfTest())
      return injector;
  }
  return AbstractInjector::Ptr(0);
}
#endif

AbstractInjector::Ptr defaultInjectorForLaunch(const ProbeABI &abi)
{
#if defined(Q_OS_MAC)
  if (abi.majorQtVersion() >= 5 && abi.minorQtVersion() >= 4)
    return createInjector(QStringLiteral("preload"));
  return findFirstWorkingInjector(QStringList() << QStringLiteral("lldb") << QStringLiteral("gdb"));
#elif defined(Q_OS_UNIX)
  Q_UNUSED(abi);
  return createInjector(QStringLiteral("preload"));
#else
  Q_UNUSED(abi);
  return createInjector(QStringLiteral("windll"));
#endif
}

AbstractInjector::Ptr defaultInjectorForAttach()
{
#if defined(Q_OS_MAC)
  return findFirstWorkingInjector(QStringList() << QStringLiteral("lldb") << QStringLiteral("gdb"));
#elif !defined(Q_OS_WIN)
  return findFirstWorkingInjector(QStringList() << QStringLiteral("gdb") << QStringLiteral("lldb"));
#else
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
