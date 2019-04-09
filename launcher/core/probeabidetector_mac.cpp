/*
  probeabidetector_mac.cpp

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
// krazy:excludeall=null since used by Darwin internals

#include <config-gammaray.h>

#include "probeabidetector.h"
#include "probeabi.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QString>

#include <CoreFoundation/CoreFoundation.h>
#include <mach-o/loader.h>

using namespace GammaRay;

template<typename T>
static QString readMachOHeader(const uchar *data, quint64 size, quint32 &offset, qint32 &ncmds,
                               qint32 &cmdsize);

static QString resolveBundlePath(const QString &bundlePath)
{
    const QFileInfo fi(bundlePath);
    if (!fi.isBundle())
        return bundlePath;

    const QByteArray utf8Bundle = fi.absoluteFilePath().toUtf8();
    CFURLRef bundleUrl
        = CFURLCreateFromFileSystemRepresentation(NULL,
                                                  reinterpret_cast<const UInt8 *>(utf8Bundle.data()),
                                                  utf8Bundle.length(), true);
    CFBundleRef bundle = CFBundleCreate(NULL, bundleUrl);
    if (bundle) {
        CFURLRef url = CFBundleCopyExecutableURL(bundle);
        char executableFile[FILENAME_MAX];
        CFURLGetFileSystemRepresentation(url, true, reinterpret_cast<UInt8 *>(executableFile),
                                         FILENAME_MAX);
        CFRelease(url);
        CFRelease(bundle);
        CFRelease(bundleUrl);
        return QString::fromUtf8(executableFile);
    }
    CFRelease(bundle);
    CFRelease(bundleUrl);

    return bundlePath;
}

static QString qtCoreFromOtool(const QString &path)
{
    QProcess proc;
    proc.setProcessChannelMode(QProcess::SeparateChannels);
    proc.setReadChannel(QProcess::StandardOutput);
    proc.start("otool", QStringList() << "-L" << path);
    proc.waitForFinished();

    forever {
        const QByteArray line = proc.readLine();
        if (line.isEmpty())
            break;

        if (ProbeABIDetector::containsQtCore(line)) {
            const int pos = line.lastIndexOf(" (");
            if (pos <= 0)
                continue;
            return QString::fromLocal8Bit(line.left(pos).trimmed());
        }
    }

    return QString();
}

static QStringList readRPaths(const QString &path)
{
    QStringList rpaths;
    QFile f(path);
    if (!f.open(QFile::ReadOnly))
        return rpaths;

    auto size = f.size();
    const uchar *data = f.map(0, size);
    if (!data || (uint)size <= sizeof(quint32))
        return rpaths;

    quint32 offset = 0;
    qint32 ncmds = 0;
    qint32 cmdsize = 0;

    const quint32 magic = *reinterpret_cast<const quint32 *>(data);
    switch (magic) {
    case MH_MAGIC:
        readMachOHeader<mach_header>(data, size, offset, ncmds, cmdsize);
        break;
    case MH_MAGIC_64:
        readMachOHeader<mach_header_64>(data, size, offset, ncmds, cmdsize);
        break;
    }

    if (offset >= size || ncmds <= 0 || cmdsize <= 0 || size <= offset + cmdsize)
        return rpaths;

    // read load commands
    const auto pathBase = QFileInfo(path).absolutePath();
    for (int i = 0; i < ncmds; ++i) {
        const load_command *cmd = reinterpret_cast<const load_command *>(data + offset);
        if (cmd->cmd == LC_RPATH) {
            const rpath_command *rpcmd = reinterpret_cast<const rpath_command *>(data + offset);
            auto rpath = QString::fromUtf8(reinterpret_cast<const char *>(rpcmd) + rpcmd->path.offset);
            rpath.replace(QStringLiteral("@executable_path"), pathBase);
            rpath.replace(QStringLiteral("@loader_path"), pathBase);
            rpaths.push_back(rpath);
        }
        offset += cmd->cmdsize;
    }

    return rpaths;
}

static QString resolveRPath(const QString &path, const QStringList &rpaths)
{
    for (const auto &rpath : rpaths) {
        auto resolvedPath = path;
        resolvedPath.replace("@rpath", rpath);
        if (QFile::exists(resolvedPath))
            return resolvedPath;
    }
    return path;
}

QString ProbeABIDetector::qtCoreForExecutable(const QString &path) const
{
    auto qtCorePath = qtCoreFromOtool(resolveBundlePath(path));
    qtCorePath = resolveRPath(qtCorePath, readRPaths(path));
    return qtCorePath;
}

QString ProbeABIDetector::qtCoreForProcess(quint64 pid) const
{
    return qtCoreFromLsof(pid);
}

template<typename T>
static QString readMachOHeader(const uchar *data, quint64 size, quint32 &offset, qint32 &ncmds,
                               qint32 &cmdsize)
{
    if (size <= sizeof(T))
        return QString();
    const T *header = reinterpret_cast<const T *>(data);

    offset = sizeof(T);
    ncmds = header->ncmds;
    cmdsize = header->sizeofcmds;

    switch (header->cputype) {
    case CPU_TYPE_I386:
        return "i686";
    case CPU_TYPE_X86_64:
        return "x86_64";
    }

    return QString();
}

static ProbeABI abiFromMachO(const QString &path, const uchar *data, qint64 size)
{
    ProbeABI abi;
    const quint32 magic = *reinterpret_cast<const quint32 *>(data);

    quint32 offset = 0;
    qint32 ncmds = 0;
    qint32 cmdsize = 0;

    switch (magic) {
    case MH_MAGIC:
        abi.setArchitecture(readMachOHeader<mach_header>(data, size, offset, ncmds, cmdsize));
        break;
    case MH_MAGIC_64:
        abi.setArchitecture(readMachOHeader<mach_header_64>(data, size, offset, ncmds, cmdsize));
        break;
    }

    if (offset >= size || ncmds <= 0 || cmdsize <= 0 || size <= offset + cmdsize)
        return ProbeABI();

    // read load commands
    for (int i = 0; i < ncmds; ++i) {
        const load_command *cmd = reinterpret_cast<const load_command *>(data + offset);
        if (cmd->cmd == LC_ID_DYLIB) {
            const dylib_command *dlcmd = reinterpret_cast<const dylib_command *>(data + offset);
            const int majorVersion = (dlcmd->dylib.current_version & 0x00ff0000) >> 16;
            const int minorVersion = (dlcmd->dylib.current_version & 0x0000ff00) >> 8;
            abi.setQtVersion(majorVersion, minorVersion);
        }
        offset += cmd->cmdsize;
    }

    if (QFileInfo(path).baseName().endsWith(QStringLiteral("_debug"), Qt::CaseInsensitive)) {
        // We can probably also look for a S_ATTR_DEBUG segment, in the data, but that might not proove it's a debug
        // build as we can add debug symbols to release builds.
        abi.setIsDebug(true);
    }

    return abi;
}

ProbeABI ProbeABIDetector::detectAbiForQtCore(const QString &path) const
{
    if (path.isEmpty())
        return ProbeABI();

    QFile f(path);
    if (!f.open(QFile::ReadOnly))
        return ProbeABI();

    const uchar *data = f.map(0, f.size());
    if (!data || (uint)f.size() <= sizeof(quint32))
        return ProbeABI();
    return abiFromMachO(path, data, f.size());
}
