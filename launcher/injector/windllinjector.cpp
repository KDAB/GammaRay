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

#ifdef Q_OS_WIN
#include <windows.h>
#include <cstdlib>

using namespace GammaRay;

WinDllInjector::WinDllInjector() :
  mExitCode(-1),
  mProcessError(QProcess::UnknownError),
  mExitStatus(QProcess::NormalExit),
  m_destProcess(NULL),
  m_destThread(NULL)
{
}

bool WinDllInjector::launch(const QStringList &programAndArgs,
                           const QString &probeDll, const QString &/*probeFunc*/)
{
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
                               0, 0,
                               &startupInfo, &pid);
  if (!success) {
    return false;
  }

  m_destProcess = pid.hProcess;
  m_destThread = pid.hThread;
  m_dllPath = probeDll;
  m_dllPath.replace('/', '\\');
  inject();
  ResumeThread(pid.hThread);
  WaitForSingleObject(pid.hProcess, INFINITE);
  DWORD exitCode;
  GetExitCodeProcess(pid.hProcess, &exitCode);

  mExitCode = exitCode;
  //TODO mProcessError = proc.error();
  //TODO mExitStatus = proc.exitStatus();
  //TODO mErrorString = proc.errorString();

  return mExitCode == EXIT_SUCCESS;
}

bool WinDllInjector::attach(int pid, const QString &probeDll, const QString &/*probeFunc*/)
{
  m_dllPath = probeDll;
  m_dllPath.replace('/', '\\');

  m_destProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

  if (!m_destProcess) {
    return false;
  }

  return inject();
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

bool WinDllInjector::inject()
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
  return true;
}

QString WinDllInjector::errorString()
{
  return mErrorString;
}

#endif
