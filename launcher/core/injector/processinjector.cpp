/*
  processinjector.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    connect(&m_proc, &QProcess::errorOccurred, this, &ProcessInjector::processFailed);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(&m_proc, &QProcess::finished, this, &ProcessInjector::processFinished);
#else
    connect(&m_proc, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, &ProcessInjector::processFinished);
#endif
    connect(&m_proc, &QProcess::readyReadStandardError, this, &ProcessInjector::readStdErr);
    connect(&m_proc, &QProcess::readyReadStandardOutput, this, &ProcessInjector::readStdOut);
}

ProcessInjector::~ProcessInjector()
{
    stop_impl();
}

void ProcessInjector::stop()
{
    stop_impl();
}

void ProcessInjector::stop_impl()
{
    disconnect(&m_proc, &QProcess::errorOccurred, this, &ProcessInjector::processFailed);
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
        args = std::move(newArgs);
        qDebug() << "Launching with target wrapper:" << args;
    } else if (env.value(QStringLiteral("GAMMARAY_DEBUG")).compare(QStringLiteral("GDB"), Qt::CaseInsensitive) == 0
               || env.value(QStringLiteral("GAMMARAY_GDB")).toInt()) {
        QStringList newArgs;
        newArgs << QStringLiteral("gdb");
#ifndef Q_OS_MAC
        newArgs << QStringLiteral("--eval-command") << QStringLiteral("run");
#endif
        newArgs << QStringLiteral("--args");
        newArgs += args;
        args = std::move(newArgs);
    } else if (env.value(QStringLiteral("GAMMARAY_DEBUG")).compare(QStringLiteral("GDB_NORUN"), Qt::CaseInsensitive) == 0) {
        QStringList newArgs;
        newArgs << QStringLiteral("gdb") << QStringLiteral("--args");
        newArgs += args;
        args = std::move(newArgs);
    } else if (env.value(QStringLiteral("GAMMARAY_DEBUG")).compare(QStringLiteral("RR"), Qt::CaseInsensitive) == 0) {
        QStringList newArgs;
        newArgs << QStringLiteral("rr") << QStringLiteral("record");
        newArgs += args;
        args = std::move(newArgs);
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
