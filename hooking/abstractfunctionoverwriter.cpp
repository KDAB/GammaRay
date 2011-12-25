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

#ifdef ARCH_X86
    const int worstSizeForLongJump = 10;
#elif defined(ARCH_64)
    const int worstSizeForLongJump = 14;
#else
# error "Unsupported hardware architecture!"
#endif

const int blockSize = 4 * worstSizeForLongJump; // normally we want to overwrite 4 functions

using namespace GammaRay;

bool AbstractFunctionOverwriter::writeShortJump(void *target, void *const func)
{
    quint8 *cur = (quint8 *) target;

    //E9 relative short jump is 5 bytes long
    bool ret = unprotectMemory(target, 5);

    if (!ret)
        return false;

    *cur = 0xE9;
    cur++;
    *((quint32 *)cur) = (unsigned long)func - (unsigned long)(cur + 4);

    ret = reprotectMemory(target, 5);

    if (!ret)
        return false;

    return true;
}

bool AbstractFunctionOverwriter::writeLongJump(void *target, void *const func)
{
    quint8 *cur = (quint8 *) target;

    bool ret = unprotectMemory(target, worstSizeForLongJump);

    if (!ret)
        return false;

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

    ret = reprotectMemory(target, worstSizeForLongJump);

    if (!ret)
        return false;

    return true;
}

void *AbstractFunctionOverwriter::getMemoryNearAddress(void *const addr, int size)
{
    Q_ASSERT(blockSize > size);

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
            if (!((intptr_t)it->mem > minAddr && (intptr_t)it->mem < maxAddr))
                continue;
#endif
            quint8 *mem = (quint8 *)it->mem + (it->size - it->free);
            it->free -= size;
            return mem;
        }
    }

    void *mem = 0;
#ifdef ARCH_X86
    Q_UNUSED(addr)
    mem = reserveMemory(0, blockSize);
#elif defined(ARCH_64)
    intptr_t min = minAddr / blockSize;
    intptr_t max = maxAddr / blockSize;
    int rel = 0;
    for (int i = 0; i < (max - min + 1); ++i)
    {
        rel = -rel + (i & 1);
        void* query = reinterpret_cast<void*>(((min + max) / 2 + rel) * blockSize);

        if (isMemoryFree(query, blockSize)) {
            mem = reserveMemory(query, blockSize);
            if (mem != 0)
            {
                mem = query;
                break;
            }
        }
    }
#else
#error "Unsupported hardware architecture!"
#endif
    if (!commitMemory(mem, size))
        return 0;
    MemorySegment memSegment;
    memSegment.mem = mem;
    memSegment.size = blockSize;
    memSegment.free = blockSize - size;
    memoryPool.append(memSegment);
    return mem;
}

void *AbstractFunctionOverwriter::createTrampoline(void *const func, void *const replacement)
{
    void *mem = getMemoryNearAddress(func, worstSizeForLongJump);
    if (!mem)
        return 0;
    bool ret = writeLongJump(mem, replacement);
    if (!ret)
        return 0;
    return mem;
}

AbstractFunctionOverwriter::~AbstractFunctionOverwriter()
{
}

bool AbstractFunctionOverwriter::overwriteFunction(const QString &orignalFunc, void * const replacementFunc)
{
    void *func = qtCoreFunctionLookup(orignalFunc);
    void *mem = createTrampoline(func, replacementFunc);
    if (!mem)
        return false;

    bool ret = writeShortJump(func, mem);

    return ret;
}
