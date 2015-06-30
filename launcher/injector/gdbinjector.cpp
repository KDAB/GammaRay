/*
  gdbinjector.cpp

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

#include <config-gammaray.h>
#include "gdbinjector.h"

#include <QDebug>
#include <QProcess>
#include <QStringList>
#include <QCoreApplication>

using namespace GammaRay;

static QTextStream cout(stdout);
static QTextStream cerr(stderr);

GdbInjector::GdbInjector()
{
}

QString GdbInjector::debuggerExecutable() const
{
  return QLatin1String("gdb");
}

bool GdbInjector::launch(const QStringList &programAndArgs,
                        const QString &probeDll, const QString &probeFunc)
{
  QStringList gdbArgs;
  gdbArgs.push_back(QLatin1String("--args"));
  gdbArgs.append(programAndArgs);

  if (!startDebugger(gdbArgs)) {
    return -1;
  }

  execCmd("set confirm off");
  waitForMain();
  return injectAndDetach(probeDll, probeFunc);
}

bool GdbInjector::attach(int pid, const QString &probeDll, const QString &probeFunc)
{
  Q_ASSERT(pid > 0);
  if (!startDebugger(QStringList() << QLatin1String("-pid") << QString::number(pid))) {
    return false;
  }
  return injectAndDetach(probeDll, probeFunc);
}

void GdbInjector::execCmd(const QByteArray &cmd, bool waitForWritten)
{
  m_process->write(cmd + '\n');

  if (waitForWritten) {
    m_process->waitForBytesWritten(-1);
  }
}

void GdbInjector::readyReadStandardError()
{
  const QString error = m_process->readAllStandardError();
  cerr << error << flush;

  if (error.startsWith(QLatin1String("Function \"main\" not defined."))) {
    mManualError = true;
    mErrorString = tr("The debuggee application is missing debug symbols which are required\n"
                      "for GammaRay's GDB injector. Please recompile the debuggee.\n\n"
                      "GDB error was: %1").arg(error);
  } else if (error.startsWith(QLatin1String("Can't find member of namespace, class, struct, or union named \"QCoreApplication::exec\""))) {
    mManualError = true;
    mErrorString = tr("Your QtCore library is missing debug symbols which are required\n"
                      "for GammaRay's GDB injector. Please install the required debug symbols.\n\n"
                      "GDB error was: %1").arg(error);
  }

  if (mManualError) {
    m_process->kill();
    disconnect(m_process.data(), SIGNAL(readyReadStandardError()), this, 0);
    disconnect(m_process.data(), SIGNAL(readyReadStandardOutput()), this, 0);
    mProcessError = QProcess::FailedToStart;
    return;
  }
}

void GdbInjector::readyReadStandardOutput()
{
  if (qgetenv("GAMMARAY_UNITTEST") == "1") {
    cout << m_process->readAllStandardOutput() << flush;
  }
}

void GdbInjector::addFunctionBreakpoint(const QByteArray& function)
{
  execCmd("break " + function);
}

void GdbInjector::addMethodBreakpoint(const QByteArray& method)
{
#ifdef Q_OS_MAC
  execCmd("break " + method + "()");
#else
  execCmd("break " + method);
#endif
}

void GdbInjector::loadSymbols(const QByteArray& library)
{
#ifndef Q_OS_MAC
  execCmd("sha " + library);
#else
  Q_UNUSED(library);
#endif
}
