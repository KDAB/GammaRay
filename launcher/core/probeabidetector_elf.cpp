/*
  probeabidetector_elf.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

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
#include "libraryutil.h"

#include <QDebug>
#include <QFile>
#include <QProcess>
#include <QProcessEnvironment>
#include <QString>
#include <QStringList>

#ifdef HAVE_ELF_H
#include <elf.h>
#endif
// on Linux sys/elf.h is not what we want, on QNX we cannot add "sys" to the include dir without messing other stuff up...
#if defined(HAVE_SYS_ELF_H) && !defined(HAVE_ELF_H)
#include <sys/elf.h>
#endif

using namespace GammaRay;

static QString qtCoreFromLdd(const QString &path)
{
    foreach (const auto &lib, LibraryUtil::dependencies(path)) {
        if (ProbeABIDetector::containsQtCore(lib))
            return QString::fromLocal8Bit(lib);
    }

    return QString();
}

QString ProbeABIDetector::qtCoreForExecutable(const QString &path) const
{
    // TODO: add fast version reading the ELF file directly?
    return qtCoreFromLdd(path);
}

static bool qtCoreFromProc(qint64 pid, QString &path)
{
    const QString mapsPath = QStringLiteral("/proc/%1/maps").arg(pid);
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

QString ProbeABIDetector::qtCoreForProcess(quint64 pid) const
{
    QString qtCorePath;
    if (!qtCoreFromProc(pid, qtCorePath))
        qtCorePath = qtCoreFromLsof(pid);
    return qtCorePath;
}

static ProbeABI qtVersionFromFileName(const QString &path)
{
    ProbeABI abi;

    const QStringList parts = path.split('.');
    if (parts.size() < 4 || parts.at(parts.size() - 4) != QLatin1String("so"))
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
    const int pos = line.indexOf("Qt ");
    const QList<QByteArray> version = line.mid(pos + 2).split('.');
    if (version.size() < 3)
        return abi;

    abi.setQtVersion(version.at(0).toInt(), version.at(1).toInt());

    return abi;
}

#ifdef HAVE_ELF
template<typename ElfEHdr>
static QString archFromELFHeader(const uchar *data, quint64 size)
{
    if (size <= sizeof(ElfEHdr))
        return QString();
    const ElfEHdr *hdr = reinterpret_cast<const ElfEHdr *>(data);

    switch (hdr->e_machine) {
    case EM_386:
        return QStringLiteral("i686");
#ifdef EM_X86_64
    case EM_X86_64:
        return QStringLiteral("x86_64");
#endif
    case EM_ARM:
        return QStringLiteral("arm");
#ifdef EM_AARCH64
    case EM_AARCH64:
        return QStringLiteral("aarch64");
#endif
    }

    qWarning() << "Unsupported ELF machine type:" << hdr->e_machine;
    return QString();
}

#endif

static QString archFromELF(const QString &path)
{
#ifdef HAVE_ELF
    QFile f(path);
    if (!f.open(QFile::ReadOnly))
        return QString();

    const uchar *data = f.map(0, f.size());
    if (!data || f.size() < EI_NIDENT)
        return QString();

    if (qstrncmp(reinterpret_cast<const char *>(data), ELFMAG, SELFMAG) != 0) // no ELF signature
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

ProbeABI ProbeABIDetector::detectAbiForQtCore(const QString &path) const
{
    if (path.isEmpty())
        return ProbeABI();

    // try to find the version
    ProbeABI abi = qtVersionFromFileName(path);
    if (!abi.hasQtVersion())
        abi = qtVersionFromExec(path);

    // TODO: architecture detection fallback without elf.h?
    const QString arch = archFromELF(path);
    abi.setArchitecture(arch);

    return abi;
}
