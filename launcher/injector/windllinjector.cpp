/*
  windllinjector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
//krazy:excludeall=null,captruefalse

#include "windllinjector.h"

#include <QDebug>
#include <QThread>

#ifdef Q_OS_WIN
#include <windows.h>
#include <cstdlib>

namespace GammaRay {

class FinishWaiter : public QThread {
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

        m_injector->mExitCode = exitCode;
        emit m_injector->finished();
    }

private:
    WinDllInjector *m_injector;
};

WinDllInjector::WinDllInjector() :
  mExitCode(-1),
  mProcessError(QProcess::UnknownError),
  mExitStatus(QProcess::NormalExit),
  m_destProcess(NULL),
  m_destThread(NULL),
  m_injectThread(new FinishWaiter(this))
{
}

WinDllInjector::~WinDllInjector()
{
    delete m_injectThread;
}

QString WinDllInjector::name() const
{
  return QString("windll");
}

bool WinDllInjector::launch(const QStringList &programAndArgs,
                            const QString &probeDll, const QString &/*probeFunc*/,
                            const QProcessEnvironment &env)
{
  // https://msdn.microsoft.com/en-us/library/windows/desktop/ms682425%28v=vs.85%29.aspx
  QByteArray buffer;
  char null[2]= {0,0};
  foreach(const QString &kv, env.toStringList()) {
    buffer.append((const char*)kv.utf16(), kv.size() * sizeof(ushort));
    buffer.append(null, 2);
  }
  if (!buffer.isEmpty())
    buffer.append(null, 2); // windows needs double \0 at the end of env vars

  DWORD dwCreationFlags = CREATE_NO_WINDOW;
  dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
  dwCreationFlags |= CREATE_SUSPENDED;
  STARTUPINFOW startupInfo = { sizeof(STARTUPINFO), 0, 0, 0,
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
  BOOL success = CreateProcess(0, (wchar_t *)applicationName.utf16(),
                               0, 0, TRUE, dwCreationFlags,
                               buffer.isEmpty() ? 0 : buffer.data(), 0,
                               &startupInfo, &pid);
  if (!success) {
    return false;
  }

  m_destProcess = pid.hProcess;
  m_destThread = pid.hThread;
  m_dllPath = probeDll;
  m_dllPath.replace('/', '\\');
  inject();
  m_injectThread->stop();
  emit started();
  ResumeThread(pid.hThread);
  m_injectThread->start();
  return m_injectThread->isRunning();
}

bool WinDllInjector::attach(int pid, const QString &probeDll, const QString &/*probeFunc*/)
{
  m_dllPath = probeDll;
  m_dllPath.replace('/', '\\');

  m_destProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

  if (!m_destProcess) {
    return false;
  }

  inject();
  m_injectThread->stop();
  emit started();
  m_injectThread->start();
  return m_injectThread->isRunning();
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
  int strsize = (m_dllPath.size() * 2) + 2;
  void *mem = VirtualAllocEx(m_destProcess, NULL, strsize, MEM_COMMIT, PAGE_READWRITE);
  WriteProcessMemory(m_destProcess, mem, (void*)m_dllPath.utf16(), strsize, NULL);
  HMODULE kernel32handle = GetModuleHandleW(L"Kernel32");
  FARPROC loadLib = GetProcAddress(kernel32handle, "LoadLibraryW");
  m_destThread = CreateRemoteThread(m_destProcess, NULL, 0,
                                    (LPTHREAD_START_ROUTINE)loadLib,
                                    mem, 0, NULL);

  WaitForSingleObject(m_destThread, INFINITE);

  DWORD result;
  GetExitCodeThread(m_destThread, &result);

  CloseHandle(m_destThread);
  VirtualFreeEx(m_destProcess, mem, strsize, MEM_RELEASE);
}

QString WinDllInjector::errorString()
{
  return mErrorString;
}

void WinDllInjector::stop()
{
    TerminateProcess(m_destProcess, 0xff);
}

}// namespace GammaRay
#endif
