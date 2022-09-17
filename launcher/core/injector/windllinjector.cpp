/*
  windllinjector.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Patrick Spendrin <patrick.spendrin@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/
// krazy:excludeall=null,captruefalse

#include "windllinjector.h"
#include "basicwindllinjector.h"

#include <config-gammaray.h>
#include <common/paths.h>
#include <common/commonutils.h>

#include <QDebug>
#include <QDir>
#include <QThread>

#include <cstdlib>

namespace GammaRay {
class FinishWaiter : public QThread
{
public:
    FinishWaiter(WinDllInjector *injector)
        : m_injector(injector)
    {
    }
    ~FinishWaiter()
    {
        stop();
    }

    void stop()
    {
        if (isRunning()) {
            terminate();
            wait();
        }
    }

protected:
    void run() override
    {
        WaitForSingleObject(m_injector->m_destProcess, INFINITE);
        DWORD exitCode;
        GetExitCodeProcess(m_injector->m_destProcess, &exitCode);
        QMetaObject::invokeMethod(m_injector, "processExited", Qt::QueuedConnection, Q_ARG(int, exitCode));
    }

private:
    WinDllInjector *m_injector;
};

WinDllInjector::WinDllInjector()
    : mExitCode(-1)
    , mProcessError(QProcess::UnknownError)
    , mExitStatus(QProcess::NormalExit)
    , m_destProcess(INVALID_HANDLE_VALUE)
    , m_injectThread(new FinishWaiter(this))
{
}

WinDllInjector::~WinDllInjector()
{
    stop();
    m_injectThread->wait(1000);
    delete m_injectThread;
}

QString WinDllInjector::name() const
{
    return QString("windll");
}

bool WinDllInjector::launch(const QStringList &programAndArgs, const QString &probeDll,
                            const QString & /*probeFunc*/, const QProcessEnvironment &_env)
{
    // add location of GammaRay DLLs to PATH, so the probe can find them
    auto env = _env;
    env.insert(QStringLiteral("PATH"), env.value(QStringLiteral("PATH")) + QLatin1Char(';') + Paths::binPath());

    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms682425%28v=vs.85%29.aspx
    QByteArray buffer;
    char null[2] = { 0, 0 };
    foreach (const QString &kv, env.toStringList()) {
        buffer.append(( const char * )kv.utf16(), kv.size() * sizeof(ushort));
        buffer.append(null, 2);
    }
    if (!buffer.isEmpty())
        buffer.append(null, 2); // windows needs double \0 at the end of env vars

    DWORD dwCreationFlags = CREATE_NO_WINDOW;
    dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
    dwCreationFlags |= CREATE_SUSPENDED;
    STARTUPINFOW startupInfo = {
        sizeof(STARTUPINFO), nullptr, nullptr, nullptr,
        ( ulong )CW_USEDEFAULT, ( ulong )CW_USEDEFAULT,
        ( ulong )CW_USEDEFAULT, ( ulong )CW_USEDEFAULT,
        0, 0, 0, STARTF_USESTDHANDLES, 0, 0, nullptr,
        GetStdHandle(STD_INPUT_HANDLE),
        GetStdHandle(STD_OUTPUT_HANDLE),
        GetStdHandle(STD_ERROR_HANDLE)
    };
    PROCESS_INFORMATION pid;
    memset(&pid, 0, sizeof(PROCESS_INFORMATION));

    const QString applicationName = programAndArgs.join(QLatin1String(" "));
    WIN_ERROR_ASSERT(CreateProcess(nullptr, ( wchar_t * )applicationName.utf16(),
                                   nullptr, nullptr, TRUE, dwCreationFlags,
                                   buffer.isEmpty() ? nullptr : buffer.data(),
                                   ( wchar_t * )workingDirectory().utf16(),
                                   &startupInfo, &pid),
                     return false);

    m_destProcess = pid.hProcess;
    QString dllPath = fixProbeDllPath(probeDll);
    BasicWinDllInjector::inject(m_destProcess, ( wchar_t * )dllPath.utf16());
    m_injectThread->stop();
    emit started();
    ResumeThread(pid.hThread);
    m_injectThread->start();
    return m_injectThread->isRunning();
}

bool WinDllInjector::attach(int pid, const QString &probeDll, const QString & /*probeFunc*/)
{
    const bool isX64 = probeDll.contains(QLatin1String("x86_64"), Qt::CaseInsensitive);
    QString application = QString(QLatin1String("gammaray-wininjector-%1")).arg(isX64 ? QLatin1String("x86_64") : QLatin1String("i686"));
    QStringList args;
    args << QString::number(pid)
         << QDir::toNativeSeparators(Paths::binPath())
         << fixProbeDllPath(probeDll);
    QProcess p;
    p.setProcessChannelMode(QProcess::ForwardedChannels);
    p.start(application, args);
    p.waitForFinished(-1);
    qDebug() << "Calling:" << application << args.join(" ");
    if (p.error() != QProcess::UnknownError) {
        qDebug() << "Injection failed:" << p.errorString();
        return false;
    }
    if (p.exitCode() != 0) {
        qDebug() << "Injection failed:" << p.readAll();
        qDebug() << "Exit code:" << p.exitCode();
        return false;
    }
    emit started();
    return true;
}

int WinDllInjector::exitCode()
{
    return mExitCode;
}

QProcess::ProcessError WinDllInjector::processError()
{
    return mProcessError;
}

QProcess::ExitStatus WinDllInjector::exitStatus()
{
    return mExitStatus;
}

QString WinDllInjector::errorString()
{
    return mErrorString;
}

void WinDllInjector::stop()
{
    if (m_destProcess != INVALID_HANDLE_VALUE)
        TerminateProcess(m_destProcess, 0xff);
}

void WinDllInjector::processExited(int exitCode)
{
    mExitCode = exitCode;
    emit finished();
}

QString WinDllInjector::fixProbeDllPath(const QString &probeDll)
{
    QString dllPath = QDir::toNativeSeparators(probeDll);
    dllPath.replace(QLatin1String(GAMMARAY_PROBE_BASENAME), QLatin1String("gammaray_winloader"));
    Q_ASSERT_X(QFileInfo::exists(dllPath), Q_FUNC_INFO, qPrintable(QString(QLatin1String("%1 does not exists!")).arg(dllPath)));
    return dllPath;
}

} // namespace GammaRay
