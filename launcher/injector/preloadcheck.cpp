/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

QString PreloadCheck::findSharedObjectFile(const QString &symbol)
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
    setErrorString(QObject::tr("Failed to run 'readelf' (binutils) binary: %1").
                   arg(QString(proc.errorString())));
    return false;
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

  setErrorString(QObject::tr("Symbol is not marked as relocatable: %1").arg(symbol));
  return false;
}

void PreloadCheck::setErrorString(const QString &err)
{
  m_errorString = err;
}

#endif
