/*
  This file is part of GammaRay, the Qt application inspection and
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

#include "abstractfunctionoverwriter.h"
#include <iostream>

#ifdef ARCH_X86
const long worstSizeForLongJump = 10;
#elif defined(ARCH_64)
const long worstSizeForLongJump = 14;
#else
# error "Unsupported hardware architecture!"
#endif

using namespace GammaRay;

bool AbstractFunctionOverwriter::writeShortJump(void *target, void *const func)
{
  quint8 *cur = (quint8 *) target;

  //E9 relative short jump is 5 bytes long
  bool ret = unprotectMemory(page_align(target), roundToNextPage(5));

  if (!ret) {
    std::cerr << "Failed to unprotect memory: " << page_align(target);
    return false;
  }

  *cur = 0xE9;
  cur++;
  *((quint32 *)cur) = (unsigned long)func - (unsigned long)(cur + 4);

  ret = reprotectMemory(page_align(target), roundToNextPage(5));

  if (!ret) {
    std::cerr << "Failed to reprotect memory: " << page_align(target);
    return false;
  }

  return true;
}

bool AbstractFunctionOverwriter::writeLongJump(void *target, void *const func)
{
  quint8 *cur = (quint8 *) target;

  bool ret = unprotectMemory(page_align(target), roundToNextPage(worstSizeForLongJump));

  if (!ret) {
    std::cerr << "Failed to unprotect memory: " << page_align(target);
    return false;
  }

  *cur = 0xff;
  *(++cur) = 0x25;

#ifdef ARCH_X86
  *((quint32 *) ++cur) = (quint32)(((quint32) cur) + sizeof (quint32));
  cur += sizeof (quint32);
  *((quint32 *)cur) = (quint32)func;
#elif defined(ARCH_64)
  *((quint32 *) ++cur) = 0;
  cur += sizeof (quint32);
  *((quint64*)cur) = (quint64)func;
#else
# error "Unsupported hardware architecture!"
#endif

  ret = reprotectMemory(page_align(target), roundToNextPage(worstSizeForLongJump));

  if (!ret) {
    std::cerr << "Failed to reprotect memory: " << page_align(target);
    return false;
  }

  return true;
}

void *AbstractFunctionOverwriter::getMemoryNearAddress(void *const addr, size_t size)
{
  Q_ASSERT(blocksize() > size);

#if defined(ARCH_64)
  intptr_t minAddr;
  intptr_t maxAddr;

  getAddressRange(minAddr, maxAddr);

  minAddr = std::max<intptr_t>(minAddr, reinterpret_cast<intptr_t>(addr) - 0x20000000);
  maxAddr = std::min<intptr_t>(maxAddr, reinterpret_cast<intptr_t>(addr) + 0x20000000);
#endif

  for (QList<MemorySegment>::Iterator it = memoryPool.begin(); it != memoryPool.end(); ++it) {
    if (it->free >= size) {
#if defined(ARCH_64)
      if (!((intptr_t)it->mem > minAddr && (intptr_t)it->mem < maxAddr)) {
        continue;
      }
#endif
      quint8 *mem = (quint8 *)it->mem + (it->size - it->free);
      it->free -= size;
      return mem;
    }
  }

  void *mem = 0;
#ifdef ARCH_X86
  Q_UNUSED(addr);
  mem = reserveMemory(0, blocksize());
#elif defined(ARCH_64)
  intptr_t min = minAddr / blocksize();
  intptr_t max = maxAddr / blocksize();
  int rel = 0;
  for (int i = 0; i < (max - min + 1); ++i) {
    rel = -rel + (i & 1);
    void* query = reinterpret_cast<void*>(((min + max) / 2 + rel) * blocksize());

    Q_ASSERT(!((size_t)query & (pagesize() - 1)));

    if (isMemoryFree(query, blocksize())) {
      mem = reserveMemory(query, blocksize());
      if (mem != 0 &&
          reinterpret_cast<intptr_t>(mem) > minAddr &&
          reinterpret_cast<intptr_t>(mem) < maxAddr) {
        break;
      }
    }
  }
#else
#error "Unsupported hardware architecture!"
#endif
  if (!mem) {
    std::cerr << "Error could not find memory close to: " << addr;
    return 0;
  }
  if (!commitMemory(mem, blocksize())) {
    return 0;
  }
  MemorySegment memSegment;
  memSegment.mem = mem;
  memSegment.size = blocksize();
  memSegment.free = blocksize() - size;
  memoryPool.append(memSegment);
  return mem;
}

void *AbstractFunctionOverwriter::createTrampoline(void *const func, void *const replacement)
{
  void *mem = getMemoryNearAddress(func, worstSizeForLongJump);
  if (!mem) {
    return 0;
  }
  bool ret = writeLongJump(mem, replacement);
  if (!ret) {
    return 0;
  }
  return mem;
}

AbstractFunctionOverwriter::~AbstractFunctionOverwriter()
{
}

bool AbstractFunctionOverwriter::overwriteFunction(const QString &orignalFunc,
                                                   void * const replacementFunc)
{
  void *func = qtCoreFunctionLookup(orignalFunc);
  if (!func) {
    std::cerr << "Failed to lookup: " << orignalFunc.toLatin1().data();
    return false;
  }
  void *mem = createTrampoline(func, replacementFunc);
  if (!mem) {
    return false;
  }

  bool ret = writeShortJump(func, mem);

  return ret;
}

void *AbstractFunctionOverwriter::page_align(void *addr) const
{
  Q_ASSERT(addr != 0);
  return (void *)((size_t)addr & ~(pagesize() - 1));
}

size_t AbstractFunctionOverwriter::roundToNextPage(size_t addr) const
{
  Q_ASSERT(addr != 0);
  return (size_t)page_align((void*)(addr + (pagesize() - 1)));
}

size_t GammaRay::AbstractFunctionOverwriter::blocksize()
{
  return roundToNextPage(std::max((worstSizeForLongJump * 4), pagesize()));
}
