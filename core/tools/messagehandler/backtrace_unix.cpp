/*
  backtrace_unix.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

  NOTE:  This file is heavily inspired/copied from kdebug.cpp in kdelibs/kdecore/io

  The original license is:

   This file is part of the KDE libraries
    Copyright (C) 1997 Matthias Kalle Dalheimer (kalle@kde.org)
                  2002 Holger Freyther (freyther@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/
//krazy:excludeall=cpp since lots of low-level stuff in here

#include "backtrace.h"

#ifdef IN_KDEVELOP_PARSER
#define HAVE_BACKTRACE
#define HAVE_BACKTRACE_DEMANGLE
#endif

//NOTE: we don't have check_function_exists, so lets just hardcode some OS'es
#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID) && !defined(__UCLIBC__)
#define HAVE_BACKTRACE (1)
#endif
///TODO: what else is supported? what about mac?

///BEGIN kdebug.cpp

#include <QString>
#include <stdlib.h>

#ifdef Q_OS_SOLARIS
// For the purposes of KDebug Solaris has a GNU-libc-compatible
// backtrace() function. This isn't detected by the CMake checks
// normally (check_function_exists fails), but we know it's there.
// For better results, we would use walk_context(), but that's
// a little more code -- see also the crash handler in kcrash.cpp .
#define HAVE_BACKTRACE (1)
#endif

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#ifdef __GNUC__
#define HAVE_BACKTRACE_DEMANGLE
#include <cxxabi.h>
#endif
#endif

#ifdef HAVE_BACKTRACE
static QString maybeDemangledName(char *name)
{
#ifdef HAVE_BACKTRACE_DEMANGLE
  const int len = strlen(name);
  QByteArray in = QByteArray::fromRawData(name, len);
  const int mangledNameStart = in.indexOf("(_");
  if (mangledNameStart >= 0) {
    const int mangledNameEnd = in.indexOf('+', mangledNameStart + 2);
    if (mangledNameEnd >= 0) {
      int status;
      // if we forget about this line and the one that undoes its effect we don't change the
      // internal data of the QByteArray::fromRawData() ;)
      name[mangledNameEnd] = 0;
      char *demangled = abi::__cxa_demangle(name + mangledNameStart + 1, 0, 0, &status);
      name[mangledNameEnd] = '+';
      if (demangled) {
        QString ret = QString::fromLatin1(name, mangledNameStart + 1) +
                      QString::fromLatin1(demangled) +
                      QString::fromLatin1(name + mangledNameEnd, len - mangledNameEnd);
        free(demangled);
        return ret;
      }
    }
  }
#endif
  return QString::fromLatin1(name);
}
#endif

QString kRealBacktrace(int levels)
{
  QString s;
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
  s = QLatin1String("[\n");

  for (int i = 0; i < n; ++i) {
    s += QString::number(i) +
         QLatin1String(": ") +
         maybeDemangledName(strings[i]) +
         QLatin1Char('\n');
  }
  s += QLatin1String("]\n");
  if (strings) {
    free (strings);
  }
#else
  Q_UNUSED(levels);
#endif
  return s;
}

///END kdebug.cpp

Backtrace backtraceList(int levels)
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

  for (int i = 0; i < n; ++i) {
    s << maybeDemangledName(strings[i]);
  }

  if (strings) {
    free(strings);
  }
#else
  Q_UNUSED(levels);
#endif
  return s;
}

Backtrace getBacktrace(int levels)
{
  return backtraceList(levels);
}
