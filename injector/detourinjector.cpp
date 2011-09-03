/*
  detourinjector.cpp

  This file is part of Endoscope, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Andreas Holzammer <andreas.holzammer@kdab.com>

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

#include "detourinjector.h"

#ifdef Q_OS_WIN

#include <windows.h>
#include <detours.h>

using namespace Endoscope;

int DetourInjector::launch(const QStringList& programAndArgs, const QString& probeDll, const QString& probeFunc)
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  ZeroMemory(&pi, sizeof(pi));
  si.cb = sizeof(si);
  DWORD dwFlags = CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED;

  QString dllPath( probeDll );
  dllPath.replace('/', '\\');

  QString detourPath = QCoreApplication::applicationDirPath() + "/detoured.dll";
  detourPath.replace('/', '\\');

  if (!DetourCreateProcessWithDll(0, (wchar_t*)args.join(" ").utf16(),
                                  NULL, NULL, TRUE, dwFlags, NULL, NULL,
                                  &si, &pi, detourPath.toLatin1().data(), dllPath.toLatin1().data(), NULL)) {
      printf("DetourCreateProcessWithDll failed: %d\n", GetLastError());
      ExitProcess(9007);
  }

  ResumeThread(pi.hThread);

  WaitForSingleObject(pi.hProcess, INFINITE);
  DWORD exitCode;
  GetExitCodeProcess(pi.hProcess, &exitCode);
  return exitCode;
}

#endif
