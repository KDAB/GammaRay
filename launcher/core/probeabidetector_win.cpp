/*
  probeabidetector_win.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <config-gammaray.h>

#include "probeabidetector.h"
#include "probeabi.h"
#include "pefile.h"

#include <common/commonutils.h>

#include <compat/qasconst.h>

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QString>
#include <QStringList>

#include <qt_windows.h>
#include <tlhelp32.h>

using namespace GammaRay;

// see https://msdn.microsoft.com/en-us/library/ms682586%28v=vs.85%29.aspx
static QStringList dllSearchPaths(const QString &exePath)
{
    QStringList paths;

    // (1) directory containing the executable
    QFileInfo fi(exePath);
    paths.push_back(fi.absolutePath());

    /* We don't want use wchar because Qt on Windows with MSVC can be built
       optionally with or without /Zc:wchar_t. And we can't know which.
       https://stackoverflow.com/questions/4521252/qt-msvc-and-zcwchar-t-i-want-to-blow-up-the-world
       has a long discussion and options. We chose option3: don't use wchar.
    */

    /* This code assumes UNICODE is defined */

    // (2) system directory
    TCHAR syspath[256];
    UINT len = GetSystemDirectoryW(syspath, sizeof(syspath));
    Q_ASSERT(len <= sizeof(syspath) && len > 0);
    paths.push_back(QString::fromUtf16(reinterpret_cast<const ushort *>(syspath), len));

    // (3) windows directory
    TCHAR dirpath[256];
    len = GetWindowsDirectoryW(dirpath, sizeof(dirpath));
    Q_ASSERT(len <= sizeof(dirpath));
    paths.push_back(QString::fromUtf16(reinterpret_cast<const ushort *>(dirpath), len));

    // (4) current working dir
    paths.push_back(QDir::currentPath());

    // (5) PATH
    const auto envPaths = QString::fromLocal8Bit(qgetenv("PATH"));
    paths += envPaths.split(';');

    return paths;
}

/** Resolves imports given a list of search paths. */
static QString resolveImport(const QString &import, const QStringList &searchPaths, const QString &arch)
{
    for (const auto &path : searchPaths) {
        const QString absPath = path + '/' + import;
        if (!QFile::exists(absPath)) {
            continue;
        }
        PEFile f(absPath);
        if (!f.isValid() || f.architecture() != arch) {
            continue;
        }
        return absPath;
    }
    qDebug() << "Could not resolve import" << import << "in" << searchPaths;
    return QString();
}
struct Version
{
    Version(int major, int minor)
        : major(major)
        , minor(minor)
    {
    }
    int major;
    int minor;
};

static Version fileVersion(const QString &path)
{
    // version
    DWORD pointlessHandle;
    DWORD fileVersionInfoSize = GetFileVersionInfoSize(
        reinterpret_cast<LPCWSTR>(path.utf16()), &pointlessHandle);
    if (fileVersionInfoSize) {
        QScopedArrayPointer<BYTE> buffer(new BYTE[fileVersionInfoSize]);
        if (GetFileVersionInfoW(reinterpret_cast<LPCWSTR>(path.utf16()), pointlessHandle,
                                fileVersionInfoSize, buffer.data())) {
            void *versionInfoData;
            unsigned int versionInfoSize;
            if (VerQueryValue(buffer.data(), TEXT("\\"), &versionInfoData,
                              &versionInfoSize)
                && versionInfoSize) {
                VS_FIXEDFILEINFO *versionInfo = reinterpret_cast<VS_FIXEDFILEINFO *>(versionInfoData);
                if (versionInfo->dwSignature == VS_FFI_SIGNATURE)
                    return Version(versionInfo->dwFileVersionMS >> 16, versionInfo->dwFileVersionMS & 0xFFFF);
            }
        }
    }
    return Version(-1, -1);
}

QString absoluteExecutablePath(const QString &path)
{
    if (QFile::exists(path)) {
        return path;
    }

    // see if Qt can find the executable (this can still fail for relative paths without extension)
    const auto searchedPath = QStandardPaths::findExecutable(path);
    if (!searchedPath.isEmpty()) {
        return searchedPath;
    }

    // attempt to appends missing .exe extensions
    const auto pathExt = QString::fromLocal8Bit(qgetenv("PATHEXT")).toLower().split(QLatin1Char(';'));
    for (const auto &ext : pathExt) {
        const auto extendedPath = path + ext;
        if (QFile::exists(extendedPath)) {
            return extendedPath;
        }
    }

    return path;
}

QString ProbeABIDetector::qtCoreForExecutable(const QString &path)
{
    const auto exe = absoluteExecutablePath(path);
    const auto searchPaths = dllSearchPaths(exe);
    QStringList resolvedImports = QStringList(exe);
    QSet<QString> checkedImports;

    while (!resolvedImports.isEmpty()) {
        for (const auto &import : qAsConst(resolvedImports)) {
            if (containsQtCore(import.toUtf8()))
                return import;
        }

        QStringList resolvedSubImports;
        for (const auto &import : qAsConst(resolvedImports)) {
            PEFile f(import);
            if (!f.isValid())
                continue;

            foreach (const auto &import, f.imports()) {
                const auto absPath = resolveImport(import, searchPaths, f.architecture());
                if (!absPath.isEmpty() && !checkedImports.contains(import)) {
                    resolvedSubImports.push_back(absPath);
                }
                checkedImports.insert(import);
            }
        }
        resolvedImports = resolvedSubImports;
    }

    return QString();
}

QString ProbeABIDetector::qtCoreForProcess(quint64 pid) const
{
    MODULEENTRY32 me;
    me.dwSize = sizeof(MODULEENTRY32);
    HANDLE snapshot = INVALID_HANDLE_VALUE;
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms682489%28v=vs.85%29.aspx
    // If the function fails with ERROR_BAD_LENGTH, retry the function until it succeeds.
    do {
        snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    } while (GetLastError() == ERROR_BAD_LENGTH);
    if (GetLastError() == ERROR_ACCESS_DENIED) {
        return QString();
    }
    WIN_ERROR_ASSERT(snapshot != INVALID_HANDLE_VALUE, return QString());

    for (bool hasNext = Module32First(snapshot, &me); hasNext;
         hasNext = Module32Next(snapshot, &me)) {
        const QString module = QString::fromUtf16(reinterpret_cast<const ushort *>(me.szModule));
        if (containsQtCore(module.toUtf8())) {
            const QString path = QString::fromUtf16(reinterpret_cast<const ushort *>(me.szExePath));
            CloseHandle(snapshot);
            return path;
        }
    }
    CloseHandle(snapshot);
    return QString();
}

static QString compilerFromLibraries(const QStringList &libraries)
{
    for (const QString &lib : libraries) {
        if (lib.startsWith(QLatin1String("libgcc"), Qt::CaseInsensitive))
            return "GNU";
    }

    return "MSVC";
}

static QString compilerVersionFromLibraries(const QStringList &libraries)
{
    for (const QString &lib : libraries) {
        if (lib.startsWith(QLatin1String("msvcp"), Qt::CaseInsensitive) || lib.startsWith(QLatin1String("vcruntime"), Qt::CaseInsensitive)) {
            return QString::number(fileVersion(lib).major * 10);
        }
    }
    return QString();
}

static bool isDebugBuild(const QString &qtCoreDll)
{
    return qtCoreDll.endsWith(QLatin1String("d.dll"), Qt::CaseInsensitive);
}

QVector<ProbeABI> ProbeABIDetector::detectAbiForQtCore(const QString &path)
{
    ProbeABI abi;
    if (path.isEmpty())
        return {};

    Version version = fileVersion(path);
    if (version.major == -1)
        return {};

    abi.setQtVersion(version.major, version.minor);

    // architecture and dependent libraries
    PEFile f(path);
    if (!f.isValid())
        return {};

    // architecture
    abi.setArchitecture(f.architecture());
    if (abi.architecture().isEmpty())
        return {};

    // compiler and debug mode
    QStringList libs = f.imports();
    abi.setCompiler(compilerFromLibraries(libs));
    abi.setIsDebug(isDebugBuild(path));
    if (abi.compiler() == "MSVC") {
        abi.setCompilerVersion(compilerVersionFromLibraries(libs));
    }

    return { abi };
}
