/*
  probeabidetector_mac.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
#include <mach-o/fat.h>
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
    CFURLRef bundleUrl = CFURLCreateFromFileSystemRepresentation(NULL,
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
    if (!data || ( uint )size <= sizeof(quint32))
        return rpaths;

    quint32 offset = 0;
    qint32 ncmds = 0;
    qint32 cmdsize = 0;
    qint32 arch_header_offset = 0;

    const quint32 magic = *reinterpret_cast<const quint32 *>(data);
    switch (magic) {
    case FAT_CIGAM: {
        const fat_header *header = reinterpret_cast<const fat_header *>(data);
        for (unsigned long i = 0; i < OSSwapInt32(header->nfat_arch); ++i) {
            const fat_arch *arch_header = reinterpret_cast<const fat_arch *>(data + sizeof(fat_header) + sizeof(fat_arch) * i);
            if (OSSwapInt32(arch_header->cputype) & CPU_ARCH_ABI64) {
                arch_header_offset = OSSwapInt32(arch_header->offset);
                readMachOHeader<mach_header_64>(data + arch_header_offset, size, offset, ncmds, cmdsize);
                break;
            }
        }
        break;
    }
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
        const load_command *cmd = reinterpret_cast<const load_command *>(data + arch_header_offset + offset);
        if (cmd->cmd == LC_RPATH) {
            const rpath_command *rpcmd = reinterpret_cast<const rpath_command *>(data + arch_header_offset + offset);
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

QString ProbeABIDetector::qtCoreForExecutable(const QString &path)
{
    auto qtCorePath = qtCoreFromOtool(resolveBundlePath(path));
    qtCorePath = resolveRPath(qtCorePath, readRPaths(path));
    return qtCorePath;
}

QString ProbeABIDetector::qtCoreForProcess(quint64 pid) const
{
    return qtCoreFromLsof(pid);
}

static QString archFromCpuType(cpu_type_t cputype)
{
    switch (cputype) {
    case CPU_TYPE_I386:
        return "i686";
    case CPU_TYPE_X86_64:
        return "x86_64";
    case CPU_TYPE_ARM64:
        return "arm64";
    }

    printf("archFromCpuType: Unknown cpu_type_t value: %d\n", ( int )cputype);
    return QString();
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

    return archFromCpuType(header->cputype);
}

template<typename T>
static bool readAbiFromMachOHeader(const uchar *data, quint64 size, ProbeABI *abi)
{
    quint32 offset = 0;
    qint32 ncmds = 0;
    qint32 cmdsize = 0;

    abi->setArchitecture(readMachOHeader<T>(data, size, offset, ncmds, cmdsize));

    if (offset >= size || ncmds <= 0 || cmdsize <= 0 || size <= offset + cmdsize)
        return false;

    // read load commands
    for (int i = 0; i < ncmds; ++i) {
        const load_command *cmd = reinterpret_cast<const load_command *>(data + offset);
        if (cmd->cmd == LC_ID_DYLIB) {
            const dylib_command *dlcmd = reinterpret_cast<const dylib_command *>(data + offset);
            const int majorVersion = (dlcmd->dylib.current_version & 0x00ff0000) >> 16;
            const int minorVersion = (dlcmd->dylib.current_version & 0x0000ff00) >> 8;
            abi->setQtVersion(majorVersion, minorVersion);
        }
        offset += cmd->cmdsize;
    }

    return true;
}

static QVector<ProbeABI> abiFromMachO(const QString &path, const uchar *data, qint64 size)
{
    QVector<ProbeABI> result;
    const quint32 magic = *reinterpret_cast<const quint32 *>(data);

    switch (magic) {
    case FAT_CIGAM: {
        const fat_header *header = reinterpret_cast<const fat_header *>(data);
        for (unsigned long i = 0; i < OSSwapInt32(header->nfat_arch); ++i) {
            const fat_arch *arch_header = reinterpret_cast<const fat_arch *>(data + sizeof(fat_header) + sizeof(fat_arch) * i);
            if (OSSwapInt32(arch_header->cputype) & CPU_ARCH_ABI64) {
                ProbeABI abi;
                if (readAbiFromMachOHeader<mach_header_64>(data + OSSwapInt32(arch_header->offset), size, &abi)) {
                    result << abi;
                }
                abi.setArchitecture(archFromCpuType(OSSwapInt32(arch_header->cputype)));
            }
        }
        break;
    }
    case MH_MAGIC: {
        ProbeABI abi;
        if (readAbiFromMachOHeader<mach_header>(data, size, &abi)) {
            result << abi;
        }
        break;
    }
    case MH_MAGIC_64: {
        ProbeABI abi;
        if (readAbiFromMachOHeader<mach_header_64>(data, size, &abi)) {
            result << abi;
        }
        break;
    }
    }

    if (QFileInfo(path).baseName().endsWith(QStringLiteral("_debug"), Qt::CaseInsensitive)) {
        // We can probably also look for a S_ATTR_DEBUG segment, in the data, but that might not prove it's a debug
        // build as we can add debug symbols to release builds.
        for (ProbeABI &abi : result) {
            abi.setIsDebug(true);
        }
    }

    return result;
}

QVector<ProbeABI> ProbeABIDetector::detectAbiForQtCore(const QString &path)
{
    if (path.isEmpty())
        return {};

    QFile f(path);
    if (!f.open(QFile::ReadOnly))
        return {};

    const uchar *data = f.map(0, f.size());
    if (!data || ( uint )f.size() <= sizeof(quint32))
        return {};
    return abiFromMachO(path, data, f.size());
}
