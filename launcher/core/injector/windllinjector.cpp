/*
  windllinjector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Patrick Spendrin <ps_ml@gmx.de>

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
// krazy:excludeall=null,captruefalse

#include "windllinjector.h"

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
        : m_injector(injector) {}
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
    void run()
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
    , m_destProcess(NULL)
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
    char null[2] = {0, 0};
    foreach (const QString &kv, env.toStringList()) {
        buffer.append((const char *)kv.utf16(), kv.size() * sizeof(ushort));
        buffer.append(null, 2);
    }
    if (!buffer.isEmpty())
        buffer.append(null, 2); // windows needs double \0 at the end of env vars

    DWORD dwCreationFlags = CREATE_NO_WINDOW;
    dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
    dwCreationFlags |= CREATE_SUSPENDED;
    STARTUPINFOW startupInfo = {
        sizeof(STARTUPINFO), 0, 0, 0,
        (ulong)CW_USEDEFAULT, (ulong)CW_USEDEFAULT,
        (ulong)CW_USEDEFAULT, (ulong)CW_USEDEFAULT,
        0, 0, 0, STARTF_USESTDHANDLES, 0, 0, 0,
        GetStdHandle(STD_INPUT_HANDLE),
        GetStdHandle(STD_OUTPUT_HANDLE),
        GetStdHandle(STD_ERROR_HANDLE)
    };
    PROCESS_INFORMATION pid;
    memset(&pid, 0, sizeof(PROCESS_INFORMATION));

    const QString applicationName = programAndArgs.join(" ");
    WIN_ERROR_ASSERT(CreateProcess(0, (wchar_t *)applicationName.utf16(),
                                 0, 0, TRUE, dwCreationFlags,
                                 buffer.isEmpty() ? 0 : buffer.data(),
                                 (wchar_t *)workingDirectory().utf16(),
                                 &startupInfo, &pid),
                     return false);

    m_destProcess = pid.hProcess;
    m_dllPath = QDir::toNativeSeparators(probeDll);
    inject();
    m_injectThread->stop();
    emit started();
    ResumeThread(pid.hThread);
    m_injectThread->start();
    return m_injectThread->isRunning();
}

bool WinDllInjector::attach(int pid, const QString &probeDll, const QString & /*probeFunc*/)
{
    m_dllPath = QDir::toNativeSeparators(probeDll);

    m_destProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    if (!m_destProcess) {
        qWarning() << "Failed to open process" << pid << "error code:" << qt_error_string();
        return false;
    }

    addDllDirectory();
    inject();
    emit started();
    m_destProcess = 0;
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

void WinDllInjector::inject()
{
    WIN_ERROR_ASSERT(remoteKernel32Call("LoadLibraryW", m_dllPath), qDebug() << m_dllPath);
}

void WinDllInjector::addDllDirectory()
{
    const auto path = QDir::toNativeSeparators(Paths::binPath());
    WIN_ERROR_ASSERT(remoteKernel32Call("SetDllDirectoryW", path), qDebug() << path);
}

int WinDllInjector::remoteKernel32Call(const char *funcName, const QString &argument)
{
    // resolve function pointer
    auto kernel32handle = GetModuleHandleW(L"Kernel32");
    auto func = GetProcAddress(kernel32handle, funcName);
    if (!func) {
        qWarning() << "Unable to resolve" << funcName << "in kernel32.dll!" << qt_error_string();
        return FALSE;
    }

    // write argument into target process memory
    const int strsize = (argument.size() * 2) + 2;
    void *mem = VirtualAllocEx(m_destProcess, NULL, strsize, MEM_COMMIT, PAGE_READWRITE);
    if (!mem) {
        qWarning() << "Failed to allocate memory in target process!" << qt_error_string();
        return FALSE;
    }
    WIN_ERROR_ASSERT(WriteProcessMemory(m_destProcess, mem, (void*)argument.utf16(), strsize, NULL), return FALSE);

    // call function pointer in remote process
    auto thread = CreateRemoteThread(m_destProcess, NULL, 0, (LPTHREAD_START_ROUTINE)func, mem, 0, NULL);
    if (!thread) {
        qWarning() << "Filed to creare thread in target process!" << qt_error_string();
        return FALSE;
    }
    WaitForSingleObject(thread, INFINITE);

    DWORD result;
    WIN_ERROR_CHECK(GetExitCodeThread(thread, &result));
    // cleanup
    WIN_ERROR_CHECK(VirtualFreeEx(m_destProcess, mem, 0, MEM_RELEASE));
    WIN_ERROR_CHECK(CloseHandle(thread));
    return result;
}

QString WinDllInjector::errorString()
{
    return mErrorString;
}

void WinDllInjector::stop()
{
    if (m_destProcess)
        TerminateProcess(m_destProcess, 0xff);
}

void WinDllInjector::processExited(int exitCode)
{
    mExitCode = exitCode;
    emit finished();
}

}// namespace GammaRay
