/*
  debuggerinjector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "debuggerinjector.h"

#include <QDebug>
#include <QFile>
#include <QProcess>
#include <QTime>
#include <QStandardPaths>

#include <iostream>
#include <cstdlib>
#include <dlfcn.h>

using namespace GammaRay;

DebuggerInjector::~DebuggerInjector()
{
    stop();
}

QString DebuggerInjector::filePath() const
{
    return m_filePath;
}

void DebuggerInjector::setFilePath(const QString &filePath)
{
    m_filePath = filePath;
}

void DebuggerInjector::stop()
{
    if (m_process) {
        if (!mManualError) {
            mManualError = true;
            mErrorString = tr("Process stopped.");
        }
        m_process->terminate();
        if (!m_process->waitForFinished(1000))
            m_process->kill(); // kill it softly
    }
}

QString DebuggerInjector::errorString()
{
    return mErrorString;
}

int DebuggerInjector::exitCode()
{
    return mExitCode;
}

QProcess::ExitStatus DebuggerInjector::exitStatus()
{
    return mExitStatus;
}

QProcess::ProcessError DebuggerInjector::processError()
{
    return mProcessError;
}

void DebuggerInjector::execCmd(const QByteArray &cmd, bool waitForWritten)
{
    processLog(DebuggerInjector::Out, false, QString::fromLatin1(cmd));

    m_process->write(cmd + '\n');

    if (waitForWritten)
        m_process->waitForBytesWritten(-1);
}

void DebuggerInjector::readyReadStandardOutput()
{
    m_process->setReadChannel(QProcess::StandardOutput);
    while (m_process->canReadLine()) {
        const QString output = QString::fromLocal8Bit(m_process->readLine());
        processLog(DebuggerInjector::In, false, output);
        emit stdoutMessage(output);
    }
}

void DebuggerInjector::setManualError(const QString &msg)
{
    mManualError = true;
    mErrorString = msg;

    m_process->kill();
    disconnect(m_process.data(), &QProcess::readyReadStandardError, this, nullptr);
    disconnect(m_process.data(), &QProcess::readyReadStandardOutput, this, nullptr);
    mProcessError = QProcess::FailedToStart;
}

void DebuggerInjector::processFinished()
{
    mExitCode = m_process->exitCode();
    mExitStatus = m_process->exitStatus();
    if (!mManualError) {
        mProcessError = m_process->error();
        if (mProcessError != QProcess::UnknownError)
            mErrorString = m_process->errorString();
        emit attached();
    } else {
        emit finished();
    }
}

void DebuggerInjector::readyReadStandardError()
{
    m_process->setReadChannel(QProcess::StandardError);
    while (m_process->canReadLine()) {
        const auto line = m_process->readLine();
        parseStandardError(line);
        const auto error = QString::fromLocal8Bit(line);
        processLog(DebuggerInjector::In, true, error);
        emit stderrMessage(error);
    }
}

bool DebuggerInjector::startDebugger(const QStringList &args, const QProcessEnvironment &env)
{
    m_process.reset(new QProcess);
    if (!env.isEmpty())
        m_process->setProcessEnvironment(env);
    connect(m_process.data(), &QProcess::readyReadStandardError,
            this, &DebuggerInjector::readyReadStandardError);
    connect(m_process.data(), &QProcess::readyReadStandardOutput,
            this, &DebuggerInjector::readyReadStandardOutput);
    connect(m_process.data(), &QProcess::started,
            this, &AbstractInjector::started);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(m_process.data(), &QProcess::finished, this, &DebuggerInjector::processFinished);
#else
    connect(m_process.data(), static_cast<void(QProcess::*)(int)>(&QProcess::finished),
            this, &DebuggerInjector::processFinished);
#endif
    m_process->setProcessChannelMode(QProcess::SeparateChannels);
    m_process->start(filePath(), args);
    bool status = m_process->waitForStarted(-1);

    if (!status) {
        mExitCode = m_process->exitCode();
        mExitStatus = m_process->exitStatus();
        if (!mManualError) {
            mProcessError = m_process->error();
            mErrorString = m_process->errorString();
        }
    } else {
        emit started();
    }
    return status;
}

bool DebuggerInjector::selfTest()
{
    if (QStandardPaths::findExecutable(filePath()).isEmpty()) {
        mErrorString = tr("The debugger executable '%1' could not be found").arg(filePath());
        return false;
    }

    // check for the Yama prtrace_scope setting, which can prevent attaching to work
    QFile file(QStringLiteral("/proc/sys/kernel/yama/ptrace_scope"));
    if (file.open(QFile::ReadOnly)) {
        if (file.readAll().trimmed() != "0") {
            mErrorString = tr(
                "Yama security extension is blocking runtime attaching, see /proc/sys/kernel/yama/ptrace_scope");
            return false;
        }
    }

    if (startDebugger(QStringList() << QStringLiteral("--version")))
        return m_process->waitForFinished(-1);
    return false;
}

#define STR(x) STR_IMPL(x)
#define STR_IMPL(x) #x

void DebuggerInjector::waitForMain()
{
    addFunctionBreakpoint("main");
    execCmd("run");

    loadSymbols("Qt" STR(QT_VERSION_MAJOR) "Core");
    addMethodBreakpoint("QCoreApplication::exec");
    execCmd("continue");
}

bool DebuggerInjector::injectAndDetach(const QString &probeDll, const QString &probeFunc)
{
    Q_ASSERT(m_process);
    loadSymbols("dl");
    execCmd(QStringLiteral("call (void) dlopen(\"%1\", %2)").
            arg(probeDll).arg(RTLD_NOW).toUtf8());
    loadSymbols(probeDll.toUtf8());
    execCmd(QStringLiteral("call (void) %1()").arg(probeFunc).toUtf8());

    if (qgetenv("GAMMARAY_UNITTEST") != "1") {
        execCmd("detach");
        execCmd("quit");
    } else {
        // delete all breakpoints before we continue, so we don't hit another one and abort there
        clearBreakpoints();
        execCmd("continue");
        // if we hit a crash or anything, print backtrace and quit
        printBacktrace();
        execCmd("quit", false);
    }

    return true;
}

void DebuggerInjector::loadSymbols(const QByteArray &library)
{
    Q_UNUSED(library);
}

void DebuggerInjector::processLog(DebuggerInjector::Orientation orientation, bool isError,
                                  const QString &text)
{
    if (qgetenv("GAMMARAY_UNITTEST") == "1") { // clazy:exclude=qgetenv due to Qt4 support
        const QString output = QString::fromLatin1("%1 [%2] %3: %4")
                               .arg(orientation == DebuggerInjector::In ? "<<<" : ">>>")
                               .arg(QString::fromLatin1(isError ? "ERROR" : "OUTPUT"))
                               .arg(QTime::currentTime().toString(QStringLiteral("HH:mm:ss:zzz")))
                               .arg(text.trimmed());

        if (isError)
            std::cerr << qPrintable(output) << std::endl;
        else
            std::cout << qPrintable(output) << std::endl;
    }
}
