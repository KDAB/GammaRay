/*
  lldbinjector.cpp

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

#include "lldbinjector.h"

#include <iostream>

using namespace GammaRay;

LldbInjector::LldbInjector()
{
}

LldbInjector::~LldbInjector()
{
}

QString LldbInjector::name() const
{
  return QLatin1String("lldb");
}

QString LldbInjector::debuggerExecutable() const
{
  return QLatin1String("lldb");
}

void LldbInjector::execCmd(const QByteArray& cmd, bool waitForWritten)
{
  // wait for the prompt, otherwise LLDB loses the command
  if (!m_process->bytesAvailable())
    m_process->waitForReadyRead(-1);

  if (qgetenv("GAMMARAY_UNITTEST") == "1") {
    std::cout << m_process->readAllStandardOutput().constData();
  }

  m_process->write(cmd + '\n');

  if (waitForWritten) {
    m_process->waitForBytesWritten(-1);
  }
}

void LldbInjector::addFunctionBreakpoint(const QByteArray& function)
{
  execCmd("breakpoint set -b " + function);
}

void LldbInjector::addMethodBreakpoint(const QByteArray& method)
{
  execCmd("breakpoint set -M " + method);
}

bool LldbInjector::launch(const QStringList& programAndArgs, const QString& probeDll, const QString& probeFunc)
{
  QStringList args;
  args.push_back(QLatin1String("--"));
  args.append(programAndArgs);

  if (!startDebugger(args)) {
    return -1;
  }

  waitForMain();
  return injectAndDetach(probeDll, probeFunc);
}

bool LldbInjector::attach(int pid, const QString& probeDll, const QString& probeFunc)
{
  Q_ASSERT(pid > 0);
  if (!startDebugger(QStringList() << QLatin1String("-p") << QString::number(pid))) {
    return false;
  }
  return injectAndDetach(probeDll, probeFunc);
}
