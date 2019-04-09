/*
  processinjector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "processinjector.h"

#include <common/paths.h>

#include <QDebug>
#include <iostream>

using namespace GammaRay;

ProcessInjector::ProcessInjector()
    : mExitCode(-1)
    , mProcessError(QProcess::UnknownError)
    , mExitStatus(QProcess::NormalExit)
{
    m_proc.setInputChannelMode(QProcess::ForwardedInputChannel);
    connect(&m_proc, static_cast<void(QProcess::*)(QProcess::ProcessError)>(&QProcess::error),
               this, &ProcessInjector::processFailed);
    connect(&m_proc, static_cast<void(QProcess::*)(int)>(&QProcess::finished),
            this, &ProcessInjector::processFinished);
    connect(&m_proc, &QProcess::readyReadStandardError, this, &ProcessInjector::readStdErr);
    connect(&m_proc, &QProcess::readyReadStandardOutput, this, &ProcessInjector::readStdOut);
}

ProcessInjector::~ProcessInjector()
{
    stop();
}

void ProcessInjector::stop()
{
    disconnect(&m_proc, static_cast<void(QProcess::*)(QProcess::ProcessError)>(&QProcess::error),
               this, &ProcessInjector::processFailed);
    if (m_proc.state() != QProcess::Running)
        return;
    m_proc.terminate();
    if (!m_proc.waitForFinished(1000)) // don't wait here.
        m_proc.kill(); // kill it softly
}

bool ProcessInjector::launchProcess(const QStringList &programAndArgs,
                                    const QProcessEnvironment &_env)
{
    auto env = _env;
#ifdef Q_OS_WIN
    // add location of GammaRay DLLs to PATH, so the probe can find them
    env.insert(QStringLiteral("PATH"), env.value(QStringLiteral("PATH")) + QLatin1Char(';') + Paths::binPath());
#endif
    m_proc.setProcessEnvironment(env);
    m_proc.setWorkingDirectory(workingDirectory());

    QStringList args = programAndArgs;

    if (!env.value(QStringLiteral("GAMMARAY_TARGET_WRAPPER")).isEmpty()) {
        const QString fullWrapperCmd = env.value(QStringLiteral("GAMMARAY_TARGET_WRAPPER"));
        // ### TODO properly handle quoted arguments!
        QStringList newArgs = fullWrapperCmd.split(' ');
        newArgs += args;
        args = newArgs;
        qDebug() << "Launching with target wrapper:" << args;
    } else if (env.value(QStringLiteral("GAMMARAY_GDB")).toInt()) {
        QStringList newArgs;
        newArgs << QStringLiteral("gdb");
#ifndef Q_OS_MAC
        newArgs << QStringLiteral("--eval-command") << QStringLiteral("run");
#endif
        newArgs << QStringLiteral("--args");
        newArgs += args;
        args = newArgs;
    }

    const QString program = args.takeFirst();
    m_proc.start(program, args);

    bool ret = m_proc.waitForStarted(-1);
    if (ret)
        emit started();

    return ret;
}

void ProcessInjector::processFailed()
{
    mProcessError = m_proc.error();
    mErrorString = m_proc.errorString();
}

void ProcessInjector::processFinished()
{
    mExitCode = m_proc.exitCode();
    mExitStatus = m_proc.exitStatus();

    if (mProcessError == QProcess::FailedToStart)
        mErrorString.prepend(QStringLiteral("Could not start '%1': ").arg(m_proc.program()));

    emit finished();
}

int ProcessInjector::exitCode()
{
    return mExitCode;
}

QProcess::ExitStatus ProcessInjector::exitStatus()
{
    return mExitStatus;
}

QProcess::ProcessError ProcessInjector::processError()
{
    return mProcessError;
}

QString ProcessInjector::errorString()
{
    return mErrorString;
}

void ProcessInjector::readStdErr()
{
    QString error = m_proc.readAllStandardError();
    std::cerr << qPrintable(error);
    emit stderrMessage(error);
}

void ProcessInjector::readStdOut()
{
    QString message = m_proc.readAllStandardOutput();
    std::cout << qPrintable(message);
    emit stdoutMessage(message);
}
