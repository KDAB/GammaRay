/*
  lldbinjector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "lldbinjector.h"

using namespace GammaRay;

LldbInjector::LldbInjector()
{
}

QString LldbInjector::name() const
{
  return QStringLiteral("lldb");
}

QString LldbInjector::debuggerExecutable() const
{
  return QStringLiteral("lldb");
}

void LldbInjector::disableConfirmations()
{
  // Make the debugger synchronous, important so commands are executed in blocking mode
  // This avoid lldb to quit the app just after running it
  execCmd("script lldb.debugger.SetAsync(False)");
  // Enable auto confirmations on commands (like breakpoint delete)
  execCmd("settings set auto-confirm true");
  // Allow to quit without confirmation
  execCmd("settings set interpreter.prompt-on-quit false");
}

void LldbInjector::addFunctionBreakpoint(const QByteArray &function)
{
  execCmd("breakpoint set -b " + function);
}

void LldbInjector::addMethodBreakpoint(const QByteArray &method)
{
  execCmd("breakpoint set -M " + method);
}

void LldbInjector::clearBreakpoints()
{
  execCmd("breakpoint delete");
}

void LldbInjector::printBacktrace()
{
  execCmd("thread backtrace");
}

bool LldbInjector::launch(const QStringList &programAndArgs, const QString &probeDll, const QString &probeFunc, const QProcessEnvironment &env)
{
  QStringList args;
  args.push_back(QStringLiteral("--"));
  args.append(programAndArgs);

  if (!startDebugger(args, env)) {
    return -1;
  }

  disableConfirmations();
  waitForMain();
  return injectAndDetach(probeDll, probeFunc);
}

bool LldbInjector::attach(int pid, const QString &probeDll, const QString &probeFunc)
{
  Q_ASSERT(pid > 0);
  if (!startDebugger(QStringList() << QStringLiteral("-p") << QString::number(pid))) {
    return false;
  }
  disableConfirmations();
  return injectAndDetach(probeDll, probeFunc);
}
