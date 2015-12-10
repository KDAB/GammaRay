/*
  probeabidetector_win.cpp

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
#include "pefile.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QStringList>

#include <windows.h>
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
     http://stackoverflow.com/questions/4521252/qt-msvc-and-zcwchar-t-i-want-to-blow-up-the-world
     has a long discussion and options. We chose option3: don't use wchar.
  */

  /* This code assumes UNICODE is defined */

  // (2) system directory
  TCHAR syspath[256];
  UINT len = GetSystemDirectoryW(syspath, sizeof(syspath));
  Q_ASSERT(len <= sizeof(syspath) && len > 0);
  paths.push_back(QString::fromUtf16(reinterpret_cast<const ushort*>(syspath), len));

  // (3) windows directory
  TCHAR dirpath[256];
  len = GetWindowsDirectoryW(dirpath, sizeof(dirpath));
  Q_ASSERT(len <= sizeof(dirpath));
  paths.push_back(QString::fromUtf16(reinterpret_cast<const ushort*>(dirpath), len));

  // (4) current working dir
  paths.push_back(QDir::currentPath());

  // (5) PATH
  const auto envPaths = QString::fromLocal8Bit(qgetenv("PATH"));
  paths += envPaths.split(';');

  return paths;
}

/** Resolves imports given a list of search paths. */
static QString resolveImport(const QString &import, const QStringList &searchPaths)
{
  foreach (const auto &path, searchPaths) {
    const QString absPath = path + '/' + import;
    if (QFile::exists(absPath)) {
      return absPath;
    }
  }
  qDebug() << "Could not resolve import" << import << "in" << searchPaths;
  return QString();
}

QString ProbeABIDetector::qtCoreForExecutable(const QString& path) const
{
  const auto searchPaths = dllSearchPaths(path);
  QStringList resolvedImports = QStringList(path);
  QSet<QString> checkedImports;

  while (!resolvedImports.isEmpty()) {
    foreach (const auto &import, resolvedImports) {
      if (containsQtCore(import.toUtf8()))
        return import;
    }

    QStringList resolvedSubImports;
    foreach (const auto &import, resolvedImports) {
      PEFile f(import);
      if (!f.isValid())
        continue;

      foreach (const auto &import, f.imports()) {
        const auto absPath = resolveImport(import, searchPaths);
        if (!absPath.isEmpty() && !checkedImports.contains(import))
          resolvedSubImports.push_back(absPath);
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
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
  if (snapshot == INVALID_HANDLE_VALUE) {
    return QString();
  }

  for (bool hasNext = Module32First(snapshot, &me); hasNext; hasNext = Module32Next(snapshot, &me)) {
    const QString module = QString::fromUtf16(reinterpret_cast<const ushort*>(me.szModule));
    if (containsQtCore(module.toUtf8())) {
      const QString path = QString::fromUtf16(reinterpret_cast<const ushort*>(me.szExePath));
      CloseHandle(snapshot);
      return path;
    }
  }

  CloseHandle(snapshot);
  return QString();
}

static QString compilerFromLibraries(const QStringList &libraries)
{
  foreach (const QString &lib, libraries) {
    if (lib.toLower().startsWith(QLatin1String("libgcc")))
      return "GNU";
  }

  return "MSVC";
}

static bool isDebugRuntime(const QStringList &libraries)
{
  foreach (const QString &lib, libraries) {
    if (lib.toLower().startsWith(QLatin1String("msvcr")))
      return lib.toLower().endsWith(QLatin1String("d.dll"));
  }
  return false;
}

ProbeABI ProbeABIDetector::detectAbiForQtCore(const QString& path) const
{
  ProbeABI abi;
  if (path.isEmpty())
    return abi;

  // version
  DWORD pointlessHandle;
  DWORD fileVersionInfoSize = GetFileVersionInfoSize(reinterpret_cast<LPCWSTR>(path.utf16()), &pointlessHandle);
  if (!fileVersionInfoSize)
    return ProbeABI();

  BYTE *buffer = new BYTE[fileVersionInfoSize];
  if (GetFileVersionInfoW(reinterpret_cast<LPCWSTR>(path.utf16()), pointlessHandle, fileVersionInfoSize, buffer)) {
    void* versionInfoData;
    unsigned int versionInfoSize;
    if (VerQueryValue(buffer, TEXT("\\"), &versionInfoData, &versionInfoSize) && versionInfoSize) {
      VS_FIXEDFILEINFO *versionInfo = reinterpret_cast<VS_FIXEDFILEINFO*>(versionInfoData);
      if (versionInfo->dwSignature == VS_FFI_SIGNATURE) {
        abi.setQtVersion(versionInfo->dwFileVersionMS >> 16, versionInfo->dwFileVersionMS & 0xffff);
      }
    }
  }
  delete[] buffer;

  // architecture and dependent libraries
  PEFile f(path);
  if (!f.isValid())
    return ProbeABI();

  // architecture
  abi.setArchitecture(f.architecture());
  if (abi.architecture().isEmpty())
    return ProbeABI();

  // compiler and debug mode
  QStringList libs = f.imports();
  abi.setCompiler(compilerFromLibraries(libs));
  if (abi.compiler() == "MSVC")
    abi.setIsDebug(isDebugRuntime(libs));

  return abi;
}
