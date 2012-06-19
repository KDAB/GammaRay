/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_WINFUNCTIONOVERWRITER_H
#define GAMMARAY_WINFUNCTIONOVERWRITER_H

#include <QtGlobal>

#ifdef Q_OS_WIN

#include "abstractfunctionoverwriter.h"
#include <Windows.h>

namespace GammaRay {

class WinFunctionOverwriter : public AbstractFunctionOverwriter
{
  protected:
    virtual bool unprotectMemory(void *mem, size_t size);
    virtual bool reprotectMemory(void *mem, size_t size);

    virtual bool getAddressRange(intptr_t &min, intptr_t &max);

    virtual bool isMemoryFree(void * const mem, size_t size);
    virtual void *reserveMemory(void *mem, size_t size);
    virtual bool commitMemory(void *mem, size_t size);

    virtual void *qtCoreFunctionLookup(const QString &function);
    virtual long pagesize() const;

  private:
    WinFunctionOverwriter();
    DWORD oldProtect;

    friend class FunctionOverwriterFactory;
};

}

#endif // Q_OS_WIN

#endif // WINFUNCTIONOVERWRITER_H
