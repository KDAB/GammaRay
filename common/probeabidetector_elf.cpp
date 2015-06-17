/*
  probeabidetector_elf.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  acuordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

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

#include <config-gammaray.h>

#include "probeabidetector.h"
#include "probeabi.h"

#include <QDebug>
#include <QFile>
#include <QProcess>
#include <QString>
#include <QStringList>

#ifdef HAVE_ELF_H
#include <elf.h>
#endif

using namespace GammaRay;

static QString qtCoreFromLdd(const QString &path)
{
  QProcess proc;
  proc.setProcessChannelMode(QProcess::SeparateChannels);
  proc.setReadChannel(QProcess::StandardOutput);
  proc.start("ldd", QStringList() << path);
  proc.waitForFinished();

  forever {
    const QByteArray line = proc.readLine();
    if (line.isEmpty())
      break;

    if (ProbeABIDetector::containsQtCore(line)) {
      const int begin = line.indexOf("=> ");
      const int end = line.lastIndexOf(" (");
      if (begin <= 0 || end <= 0 || end <= begin)
        continue;
      return QString::fromLocal8Bit(line.mid(begin + 3, end - begin - 3).trimmed());
    }
  }

  return QString();
}

ProbeABI ProbeABIDetector::abiForExecutable(const QString& path) const
{
  // TODO: add fast version reading the ELF file directly?
  const QString qtCorePath = qtCoreFromLdd(path);
  if (!qtCorePath.isEmpty())
    return abiForQtCore(qtCorePath);

  return ProbeABI();
}


static bool qtCoreFromProc(qint64 pid, QString &path)
{
  const QString mapsPath = QString("/proc/%1/maps").arg(pid);
  QFile f(mapsPath);
  if (!f.open(QFile::ReadOnly)) {
    path.clear();
    return false;
  }

  forever {
    const QByteArray line = f.readLine();
    if (line.isEmpty())
      break;
    if (ProbeABIDetector::containsQtCore(line)) {
      const int pos = line.indexOf('/');
      if (pos <= 0)
        continue;
      path = QString::fromLocal8Bit(line.mid(pos).trimmed());
      return true;
    }
  }

  path.clear();
  return true;
}

ProbeABI ProbeABIDetector::abiForProcess(qint64 pid) const
{
  QString qtCorePath;
  if (!qtCoreFromProc(pid, qtCorePath))
    qtCorePath = qtCoreFromLsof(pid);

  return abiForQtCore(qtCorePath);
}


static ProbeABI qtVersionFromFileName(const QString &path)
{
  ProbeABI abi;

  const QStringList parts = path.split('.');
  if (parts.size() < 4 || parts.at(parts.size() - 4) != "so")
    return abi;

  abi.setQtVersion(parts.at(parts.size() - 3).toInt(), parts.at(parts.size() - 2).toInt());
  return abi;
}

static ProbeABI qtVersionFromExec(const QString &path)
{
  ProbeABI abi;

  // yep, you can actually execute QtCore.so...
  QProcess proc;
  proc.setReadChannelMode(QProcess::SeparateChannels);
  proc.setReadChannel(QProcess::StandardOutput);
  proc.start(path);
  proc.waitForFinished();
  const QByteArray line = proc.readLine();
  const int pos = line.lastIndexOf(' ');
  const QList<QByteArray> version = line.mid(pos).split('.');
  if (version.size() < 3)
    return abi;

  abi.setQtVersion(version.at(0).toInt(), version.at(1).toInt());

  return abi;
}

#ifdef HAVE_ELF_H
template <typename ElfEHdr>
static QString archFromELFHeader(const uchar *data, quint64 size)
{
  if (size <= sizeof(ElfEHdr))
    return QString();
  const ElfEHdr *hdr = reinterpret_cast<const ElfEHdr*>(data);

  switch (hdr->e_machine) {
    case EM_386: return "i686";
#ifdef EM_X86_64
    case EM_X86_64: return "x86_64";
#endif
    case EM_ARM: return "arm";
  }

  qWarning() << "Unsupported ELF machine type:" << hdr->e_machine;
  return QString();
}
#endif

static QString archFromELF(const QString &path)
{
#ifdef HAVE_ELF_H
  QFile f(path);
  if (!f.open(QFile::ReadOnly))
    return QString();

  const uchar* data = f.map(0, f.size());
  if (!data || f.size() < EI_NIDENT)
    return QString();

  if (qstrncmp(reinterpret_cast<const char*>(data), ELFMAG, SELFMAG) != 0) // no ELF signature
    return QString();

  switch (data[EI_CLASS]) {
    case ELFCLASS32:
      return archFromELFHeader<Elf32_Ehdr>(data, f.size());
    case ELFCLASS64:
      return archFromELFHeader<Elf64_Ehdr>(data, f.size());
  }
#else
  Q_UNUSED(path);
#endif
  return QString();
}

ProbeABI ProbeABIDetector::detectAbiForQtCore(const QString& path) const
{
  // try to find the version
  ProbeABI abi = qtVersionFromFileName(path);
  if (!abi.hasQtVersion())
    abi = qtVersionFromExec(path);

  // TODO: architecture detection fallback without elf.h?
  const QString arch = archFromELF(path);
  abi.setArchitecture(arch);

  return abi;
}
