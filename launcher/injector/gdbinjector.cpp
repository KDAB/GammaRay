/*
  gdbinjector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QProcess>
#include <QStringList>

using namespace GammaRay;

GdbInjector::GdbInjector(const QString &executableOverride)
    : DebuggerInjector()
{
    setFilePath(executableOverride.isEmpty() ? QStringLiteral("gdb") : executableOverride);
}

QString GdbInjector::name() const
{
    return QStringLiteral("gdb");
}

bool GdbInjector::launch(const QStringList &programAndArgs, const QString &probeDll,
                         const QString &probeFunc, const QProcessEnvironment &env)
{
    QStringList gdbArgs;
    gdbArgs.push_back(QStringLiteral("--args"));
    gdbArgs.append(programAndArgs);

    if (!startDebugger(gdbArgs, env))
        return -1;

    disableConfirmations();
    waitForMain();
    return injectAndDetach(probeDll, probeFunc);
}

bool GdbInjector::attach(int pid, const QString &probeDll, const QString &probeFunc)
{
    Q_ASSERT(pid > 0);
    if (!startDebugger(QStringList() << QStringLiteral("-pid") << QString::number(pid)))
        return false;
    disableConfirmations();
    return injectAndDetach(probeDll, probeFunc);
}

void GdbInjector::disableConfirmations()
{
    execCmd("set confirm off");
}

void GdbInjector::readyReadStandardError()
{
    const QString error = QString::fromLocal8Bit(m_process->readAllStandardError());
    processLog(DebuggerInjector::In, true, error);
    emit stderrMessage(error);

    if (error.startsWith(QLatin1String("Function \"main\" not defined."))) {
        mManualError = true;
        mErrorString = tr("The debuggee application is missing debug symbols which are required\n"
                          "for GammaRay's GDB injector. Please recompile the debuggee.\n\n"
                          "GDB error was: %1").arg(error);
    } else if (error.startsWith(QLatin1String(
                                    "Can't find member of namespace, class, struct, or union named \"QCoreApplication::exec\"")))
    {
        mManualError = true;
        mErrorString = tr("Your QtCore library is missing debug symbols which are required\n"
                          "for GammaRay's GDB injector. Please install the required debug symbols.\n\n"
                          "GDB error was: %1").arg(error);
    } else if (error.startsWith(QLatin1String(
                    "warning: Unable to restore previously selected frame"))) {
        mManualError = true;
        mErrorString = tr("The debuggee application seems to have an invalid stack trace\n"
                          "This can be caused by the executable being updated on disk after launching it.\n\n"
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
    QString message = QString::fromLocal8Bit(m_process->readAllStandardOutput());
    processLog(DebuggerInjector::In, false, message);
    emit stderrMessage(message); // Is this signal emit correct ?? stderr vs stdout
}

void GdbInjector::addFunctionBreakpoint(const QByteArray &function)
{
    execCmd("break " + function);
}

void GdbInjector::addMethodBreakpoint(const QByteArray &method)
{
#ifdef Q_OS_MAC
    execCmd("break " + method + "()");
#else
    execCmd("break " + method);
#endif
}

void GdbInjector::clearBreakpoints()
{
    execCmd("delete");
}

void GdbInjector::printBacktrace()
{
    execCmd("backtrace", false);
}

void GdbInjector::loadSymbols(const QByteArray &library)
{
#ifndef Q_OS_MAC
    execCmd("sha " + library);
#else
    Q_UNUSED(library);
#endif
}
