/*
  backtrace_unix.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

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
//krazy:excludeall=cpp since lots of low-level stuff in here

#include <config-gammaray.h>
#include "backtrace.h"

#include <QString>
#include <stdlib.h>

#ifdef HAVE_BACKTRACE
#include <execinfo.h>

#ifdef HAVE_CXA_DEMANGLE
#include <cxxabi.h>
#endif

static QString maybeDemangleName(char *name)
{
#ifdef HAVE_CXA_DEMANGLE
  auto mangledNameStart = strstr(name, "(_Z");
  if (mangledNameStart) {
    ++mangledNameStart;
    const auto mangledNameEnd = strstr(mangledNameStart, "+");
    if (mangledNameEnd) {
      int status;
      *mangledNameEnd = 0;
      auto demangled = abi::__cxa_demangle(mangledNameStart, 0, 0, &status);
      *mangledNameEnd = '+';
      if (status == 0 && demangled) {
        QString ret = QString::fromLatin1(name, mangledNameStart - name) +
                      QString::fromLatin1(demangled) +
                      QString::fromLatin1(mangledNameEnd);
        free(demangled);
        return ret;
      }
    }
  }
#endif
  return QString::fromLatin1(name);
}
#endif

Backtrace getBacktrace(int levels)
{
  QStringList s;
#ifdef HAVE_BACKTRACE
  void *trace[256];
  int n = backtrace(trace, 256);
  if (!n) {
    return s;
  }
  char **strings = backtrace_symbols(trace, n);

  if (levels != -1) {
    n = qMin(n, levels);
  }

  s.reserve(n);
  for (int i = 0; i < n; ++i) {
    s << maybeDemangleName(strings[i]);
  }

  if (strings) {
    free(strings);
  }
#else
  Q_UNUSED(levels);
#endif
  return s;
}
