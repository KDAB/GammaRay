/*
  basicwindllinjector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Hannah von Reth <hannah.vonreth@kdab.com>

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
//krazy:excludeall=null,captruefalse since this file uses the WinAPI

#include "basicwindllinjector.h"

#include <sstream>

#define ERROR_ASSERT(condition, action) if (condition) {} else {\
    std::wstringstream stream;\
    stream << L"Error: " << #condition;\
    BasicWinDllInjector::log(stream.str().c_str());\
    BasicWinDllInjector::logError(); action; } do {} while(false)
#define ERROR_CHECK(condition) ERROR_ASSERT(condition, do {} while(false));

BOOL BasicWinDllInjector::addDllDirectory(HANDLE destProcess, wchar_t *dllDirPath) {
    if (!remoteKernel32Call(destProcess, "SetDllDirectoryW", dllDirPath)) {
        std::wstringstream stream;
        stream << L"Failed to call SetDllDirectoryW" <<  dllDirPath;
        log(stream.str().c_str());
        return FALSE;
    }
    return TRUE;
}

void BasicWinDllInjector::inject(HANDLE destProcess, wchar_t *dllPath) {
    // the probe loader will return false to get unloaded so don't check for success
    remoteKernel32Call(destProcess, "LoadLibraryW", dllPath);
}

BOOL BasicWinDllInjector::injectProcess(wchar_t *pidString, wchar_t *path, wchar_t *probePath)
{
    const int pid = std::stoi(pidString);
    HANDLE destProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    if (!destProcess) {
        std::wstringstream stream;
        stream << L"Failed to open process" << pidString;
        log(stream.str().c_str());
        BasicWinDllInjector::logError();
        return FALSE;
    }
    if(!addDllDirectory(destProcess, path))
        return FALSE;
    inject(destProcess, probePath);
    return TRUE;
}

DWORD BasicWinDllInjector::remoteKernel32Call(HANDLE destProcess, const char *funcName, const wchar_t *argument)
{
    // resolve function pointer
    auto kernel32handle = GetModuleHandleW(L"kernel32");
    auto func = GetProcAddress(kernel32handle, funcName);
    if (!func) {
        log(L"Unable to resolve symbol in kernel32.dll!");
        logError();
        return FALSE;
    }

    // write argument into target process memory
    const int strsize = (static_cast<int>(wcslen(argument)) * 2) + 2;
    void *mem = VirtualAllocEx(destProcess, NULL, strsize, MEM_COMMIT, PAGE_READWRITE);
    if (!mem) {
        log(L"Failed to allocate memory in target process!");
        logError();
        return FALSE;
    }
    ERROR_ASSERT(WriteProcessMemory(destProcess, mem, (void*)argument, strsize, NULL), return FALSE);

    // call function pointer in remote process
    auto thread = CreateRemoteThread(destProcess, NULL, 0, (LPTHREAD_START_ROUTINE)func, mem, 0, NULL);
    if (!thread) {
        log(L"Filed to creare thread in target process!");
        logError();
        return FALSE;
    }
    WaitForSingleObject(thread, INFINITE);

    DWORD result;
    ERROR_CHECK(GetExitCodeThread(thread, &result));
    // cleanup
    ERROR_CHECK(VirtualFreeEx(destProcess, mem, 0, MEM_RELEASE));
    ERROR_CHECK(CloseHandle(thread));
    return result;
}

void BasicWinDllInjector::logError(DWORD error) {
    wchar_t *string = 0;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL,
                  error,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPWSTR)&string,
                  0,
                  NULL);
    log(string);
    LocalFree((HLOCAL)string);
}

void BasicWinDllInjector::log(const wchar_t *msg) {
    OutputDebugStringW(msg);
}
