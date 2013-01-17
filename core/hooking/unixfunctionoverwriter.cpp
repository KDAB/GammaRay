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

#include "unixfunctionoverwriter.h"

#if !defined(Q_OS_WIN)

#include <dlfcn.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/errno.h>

#include <iostream>

using namespace std;
using namespace GammaRay;

UnixFunctionOverwriter::UnixFunctionOverwriter()
{
  m_pagesize = sysconf(_SC_PAGESIZE);
}

bool UnixFunctionOverwriter::unprotectMemory(void *mem, size_t size)
{
  Q_ASSERT(!((size_t)mem & (pagesize() - 1)));
  Q_ASSERT(!((size_t)size & (pagesize() - 1)));
  const bool writable = (mprotect(mem, size, PROT_READ|PROT_WRITE|PROT_EXEC) == 0);
  Q_ASSERT(writable);
  return writable;
}

bool UnixFunctionOverwriter::reprotectMemory(void *mem, size_t size)
{
  Q_ASSERT(!((size_t)mem & (pagesize() - 1)));
  Q_ASSERT(!((size_t)size & (pagesize() - 1)));
  const bool readOnly = (mprotect(mem, size, PROT_READ|PROT_EXEC) == 0);
  Q_ASSERT(readOnly);
  return readOnly;
}

bool UnixFunctionOverwriter::getAddressRange(intptr_t &min, intptr_t &max)
{
  min = INTPTR_MIN;
  max = INTPTR_MAX;

  return true;
}

bool UnixFunctionOverwriter::isMemoryFree(void * const mem, size_t size)
{
  Q_ASSERT(!((size_t)mem & (pagesize() - 1)));
  Q_ASSERT(!((size_t)size & (pagesize() - 1)));
  Q_UNUSED(mem);
  Q_UNUSED(size);

  //under unix there is no easy way to find out if a region is free or not

  return true;
}

void *UnixFunctionOverwriter::reserveMemory(void *mem, size_t size)
{
  Q_ASSERT(!((size_t)mem & (pagesize() - 1)));
  Q_ASSERT(!((size_t)size & (pagesize() - 1)));

  // reserve a memory region with a hint and hope that it is close to the other address
  void *retmem = mmap(mem, size, PROT_READ|PROT_EXEC, MAP_ANON | MAP_PRIVATE, -1, 0);

  if (retmem == MAP_FAILED) {
    return 0;
  }

  return retmem;
}

bool UnixFunctionOverwriter::commitMemory(void *mem, size_t size)
{
  Q_ASSERT(!((size_t)mem & (pagesize() - 1)));
  Q_ASSERT(!((size_t)size & (pagesize() - 1)));
  Q_UNUSED(mem);
  Q_UNUSED(size);

  //under unix we don't have a commit
  return true;
}

void *UnixFunctionOverwriter::qtCoreFunctionLookup(const QString &function)
{
  return dlsym(RTLD_NEXT, function.toLatin1());
}

long GammaRay::UnixFunctionOverwriter::pagesize() const
{
  return m_pagesize;
}

#endif // Q_OS_WIN
