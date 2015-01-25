/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#include "preloadcheck.h"

#if !defined(Q_OS_WIN) && !defined(Q_OS_MAC)

#include <QDebug>
#include <QIODevice>
#include <QFile>
#include <QRegExp>
#include <QString>
#include <QProcess>

#include <dlfcn.h>

static QString findSharedObjectFile(const QString &symbol)
{
  void *sym = dlsym(RTLD_NEXT, qPrintable(symbol));
  if (!sym) {
    return QString();
  }

  Dl_info info;
  dladdr(sym, &info);
  const QString fileName = info.dli_fname;
  return fileName;
}

PreloadCheck::PreloadCheck()
{
}

bool PreloadCheck::test(const QString &symbol)
{
  const QString fileName = findSharedObjectFile(symbol);
  if (fileName.isEmpty()) {
    setErrorString(QObject::tr("Cannot find file containing symbol: %1").arg(symbol));
    return false;
  }

  if (!QFile(fileName).exists()) {
    setErrorString(QObject::tr("Invalid shared object: %1").arg(fileName));
    return false;
  }

  QStringList args;
  args << "--relocs" << "--wide" << fileName;
  QProcess proc;
  proc.setProcessChannelMode(QProcess::MergedChannels);
  proc.start("readelf", args, QIODevice::ReadOnly);
  if (!proc.waitForFinished()) {
    // TODO: Find out if we want to error out if 'readelf' is missing
    // The question is: Do all (major) distributions ship binutils by default?
    // Major distros do, but not custom embedded ones...
    setErrorString(QObject::tr("Failed to run 'readelf' (binutils) binary: %1").
                   arg(QString(proc.errorString())));
    return true;
  }

  if (proc.exitCode() != 0) {
    setErrorString(QObject::tr("Cannot read shared object: %1").arg(QString(proc.readAll())));
    return false;
  }

  // Example line on x86_64:
  //   00000049f3d8  054300000007 R_X86_64_JUMP_SLO 000000000016c930 qt_startup_hook + 0
  // Example line on i386:
  //   002e02f0  00034407 R_386_JUMP_SLOT        00181490   qt_startup_hook
  QRegExp rx("^(?:[^ ]+\\s+){4}([^ ]+)(?:.*)$");
  while (proc.canReadLine()) {
    const QString line = proc.readLine().trimmed();
    if (!rx.exactMatch(line)) {
      continue;
    }

    const QString currentSymbol = rx.cap(1);
    if (currentSymbol == symbol) {
      qDebug() << "Found relocatable symbol in" << fileName << ":" << symbol;
      setErrorString(QString());
      return true;
    }
  }

#ifdef __mips__
  // Mips, besides the plt, has another method of
  // calling functions from .so files, and this method doesn't need JUMP_SLOT
  // relocations (in fact, it doesn't need any relocations). This method uses .got
  // entries and lazy binding stubs.
  if (testMips(symbol, fileName)) {
    qDebug() << "Call of function " << symbol << " will go through lazy binding stub";
    setErrorString(QString());
    return true;
  }
#endif

  setErrorString(QObject::tr("Symbol is not marked as relocatable: %1").arg(symbol));
  return false;
}

#ifdef __mips__
// The way to determine whether the call to function will go
// through .got and lazy binding stub is:
// - find the value of dynamic symbol index of the function with the command
//   "readelf --dyn-syms"
// - find the value of dynamic tag MIPS_GOTSYM with the command "readelf -d"
// - if (dyn_sym_index >= MIPS_GOTSYM) then the function has entry in the global
//   part of the .got, and the call will go through lazy binding stub and be
//   resolved by dynamic linker.
bool PreloadCheck::testMips(const QString &symbol, const QString &fileName)
{
  QProcess proc;
  proc.setProcessChannelMode(QProcess::MergedChannels);
  proc.start("readelf", QStringList() << "--dyn-syms" << "-d" << fileName, QIODevice::ReadOnly);

  if (!proc.waitForFinished()) {
    setErrorString(QObject::tr("Failed to run 'readelf' (binutils) binary: %1").
                   arg(QString(proc.errorString())));
    return false;
  }

  if (proc.exitCode() != 0) {
    setErrorString(QObject::tr("Cannot read shared object: %1").arg(QString(proc.readAll())));
    return false;
  }

  //Example line of dynamic symbol table on mips:
  //3851: 001e66f4     8 FUNC    GLOBAL DEFAULT   11 qt_startup_hook
  QRegExp rxSym("^(\\d+):\\s+(?:[^ ]+\\s+){6}([^ ]+)(?:.*)$");

  //Example line of dynamic tag on mips:
  //0x70000013 (MIPS_GOTSYM)                0xec3
  QRegExp rxGot("^0x[0-9a-fA-F]+\\s+\\((.+)\\)\\s+(0x[0-9a-fA-F]+)(?:.*)$");

  int dyn_sym_index = 0;
  int mips_gotsym = 0;
  bool foundDynSymbol = false;
  bool foundGotTag = false;

  while (proc.canReadLine()) {
    const QString line = proc.readLine().trimmed();
    QString currentMatch;
    const QString tag = "MIPS_GOTSYM";

    if (rxGot.exactMatch(line)) {
      currentMatch = rxGot.cap(1);

      if (currentMatch == tag) {
        bool conversionOk = false;
        int value = rxGot.cap(2).toInt(&conversionOk, 16);
        if (conversionOk){
          mips_gotsym = value;
          foundGotTag = true;
        }
      }
    }
    else if (rxSym.exactMatch(line)) {
      currentMatch = rxSym.cap(2);
      if (currentMatch == symbol) {
        dyn_sym_index = rxSym.cap(1).toInt();
        foundDynSymbol = true;
      }
    }
    if (foundGotTag && foundDynSymbol)
      break;
  }
  if (foundGotTag && foundDynSymbol && (dyn_sym_index >= mips_gotsym)) {
    return true;
  }

  return false;
}
#endif

QString PreloadCheck::errorString() const
{
  return m_errorString;
}

void PreloadCheck::setErrorString(const QString &err)
{
  m_errorString = err;
}

#endif
