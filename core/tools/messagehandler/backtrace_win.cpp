/*
  backtrace_win.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Andreas Holzammer <andreas.holzammer@kdab.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "backtrace.h"
#include "StackWalker.h"

class StackWalkerToQStringList : public StackWalker
{
  public:
    QStringList getStackWalkerBacktrace()
    {
      m_stackTrace.clear();
      ShowCallstack();
      return m_stackTrace;
    }

  protected:
    virtual void OnOutput(LPCSTR szText)
    {
      m_stackTrace << szText;
    }

    virtual void OnSymInit(LPCSTR szSearchPath, DWORD symOptions, LPCSTR szUserName)
    {
    }

    virtual void OnLoadModule(LPCSTR img, LPCSTR mod,
                              DWORD64 baseAddr, DWORD size, DWORD result,
                              LPCSTR symType, LPCSTR pdbName,
                              ULONGLONG fileVersion)
    {
    }

    virtual void OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr)
    {
    }

  private:
    QStringList m_stackTrace;
};

static StackWalkerToQStringList *stackWalkerToQStringList = 0;

Backtrace getBacktrace(int/*levels*/)
{
  //FIXME: Perhaps take the levels into account
  if (!stackWalkerToQStringList) {
    stackWalkerToQStringList = new StackWalkerToQStringList();
  }
  return stackWalkerToQStringList->getStackWalkerBacktrace();
}
