/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
//krazy:excludeall=null since the WinAPI likes to use NULL

#include "winfunctionoverwriter.h"

#ifdef Q_OS_WIN

#include <iostream>

using namespace std;
using namespace GammaRay;

WinFunctionOverwriter::WinFunctionOverwriter():oldProtect(0)
{
}

bool WinFunctionOverwriter::unprotectMemory(void *mem, size_t size)
{
  BOOL ret = VirtualProtect(mem, size, PAGE_EXECUTE_READWRITE, &oldProtect);
  return ret;
}

bool WinFunctionOverwriter::reprotectMemory(void *mem, size_t size)
{
  BOOL ret = VirtualProtect(mem, size, oldProtect, &oldProtect);
  return ret;
}

bool WinFunctionOverwriter::getAddressRange(intptr_t &min, intptr_t &max)
{
  SYSTEM_INFO si;
  GetSystemInfo(&si);

  min = reinterpret_cast<intptr_t>(si.lpMinimumApplicationAddress);
  max = reinterpret_cast<intptr_t>(si.lpMaximumApplicationAddress);

  return true;
}

bool WinFunctionOverwriter::isMemoryFree(void * const mem, size_t size)
{
  Q_UNUSED(size);
  static MEMORY_BASIC_INFORMATION mi = { 0 };

  VirtualQuery(mem, &mi, sizeof(mi));
  if (mi.State != MEM_FREE) {
    return false;
  }

  return true;
}

void *WinFunctionOverwriter::reserveMemory(void *mem, size_t size)
{
  void *retmem = 0;

  retmem  = VirtualAlloc(mem, size, MEM_RESERVE, PAGE_EXECUTE_READ);

  return retmem;
}

bool WinFunctionOverwriter::commitMemory(void *mem, size_t size)
{
  void *retmem = 0;

  retmem = VirtualAlloc(mem, size, MEM_COMMIT, PAGE_EXECUTE_READ);

  return retmem != 0;
}

void *WinFunctionOverwriter::qtCoreFunctionLookup(const QString &function)
{
  static HMODULE qtCoreDllHandle = GetModuleHandle(L"QtCore4");
  if (qtCoreDllHandle == NULL) {
    qtCoreDllHandle = GetModuleHandle(L"QtCored4");
  }

  if (qtCoreDllHandle == NULL) {
    cerr << "no handle for QtCore found!" << endl;
    return 0;
  }

  FARPROC qtfuncaddr = GetProcAddress(qtCoreDllHandle, function.toLatin1());

  return (void*)qtfuncaddr;
}

long WinFunctionOverwriter::pagesize() const
{
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  return si.dwPageSize;
}

#endif // Q_OS_WIN
