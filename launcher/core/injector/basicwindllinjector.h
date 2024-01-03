/*
  basicwindllinjector.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Hannah von Reth <hannah.vonreth@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef BASICWINDLLINJECTOR_H
#define BASICWINDLLINJECTOR_H

#include <windows.h>
#include <vector>



class BasicWinDllInjector
{
public:
    static BOOL addDllDirectory(HANDLE destProcess, wchar_t *dllDirPath);

    static void inject(HANDLE destProcess, wchar_t *dllPath);

    static BOOL injectProcess(wchar_t *pid, wchar_t *path, wchar_t *probePath);

private:
    static DWORD remoteKernel32Call(HANDLE destProcess, const char *funcName, const wchar_t *argument);

    static void logError(DWORD error = GetLastError());
    static void log(const wchar_t *msg);
};

#endif // BASICWINDLLINJECTOR_H
