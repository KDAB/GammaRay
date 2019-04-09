/*
  lldbinjector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QRegExp>

using namespace GammaRay;

LldbInjector::LldbInjector(const QString &executableOverride)
    : m_scriptSupportIsRequired(false)
{
    setFilePath(executableOverride.isEmpty() ? QStringLiteral("lldb") : executableOverride);
}

bool LldbInjector::selfTest()
{
    if (!DebuggerInjector::selfTest())
        return false;
#ifndef Q_OS_MAC
    QProcess process;

    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(filePath(), QStringList(QStringLiteral("--version")));

    if (process.waitForStarted(-1)) {
        if (process.waitForFinished(-1)) {
            const QString output = QString::fromLocal8Bit(process.readAll()).trimmed();
            const auto targetMajor = 3;
            const auto targetMinor = 6;
            QRegExp rx(QStringLiteral("\\b([\\d]\\.[\\d]+\\.[\\d]+)\\b")); // lldb version 3.7.0 ( revision )

            if (rx.indexIn(output) == -1) {
                mErrorString = tr("The debugger version can't be read (%1)").arg(output);
                return false;
            }

            const QString version = rx.cap(1);
            const QStringList parts = version.split(QLatin1Char('.'));

            if (parts.count() >= 2) {
                const auto major = parts.at(0).toInt();
                const auto minor = parts.at(1).toInt();
                if (major > targetMajor || (major == targetMajor && minor >= targetMinor))
                    return true;
            }

            mErrorString = tr("The LLDB version is not compatible: %1 (%2.%3 or higher required)")
                .arg(version).arg(targetMajor).arg(targetMinor);
            return false;
        }
    }

    mErrorString = process.errorString();
    return false;
#else
    return true;
#endif
}

QString LldbInjector::name() const
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

bool LldbInjector::launch(const QStringList &programAndArgs, const QString &probeDll,
                          const QString &probeFunc, const QProcessEnvironment &env)
{
    m_scriptSupportIsRequired = true; // launching fails with async command mode, which we can only disable with scripting
    QStringList args;
    args.push_back(QStringLiteral("--"));
    args.append(programAndArgs);

    if (!startDebugger(args, env))
        return false;

    disableConfirmations();
    waitForMain();
    return injectAndDetach(probeDll, probeFunc);
}

bool LldbInjector::attach(int pid, const QString &probeDll, const QString &probeFunc)
{
    Q_ASSERT(pid > 0);
    if (!startDebugger(QStringList() << QStringLiteral("-p") << QString::number(pid)))
        return false;
    disableConfirmations();
    return injectAndDetach(probeDll, probeFunc);
}

void LldbInjector::parseStandardError(const QByteArray& line)
{
    if (m_scriptSupportIsRequired && line.startsWith("error: your copy of LLDB does not support scripting"))
        setManualError(tr("LLDB does not support scripting. Install lldb python support please."));
}
