/*
  probeabidetector_win.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "config-gammaray.h"

#include "probeabidetector.h"
#include "probeabi.h"

#include <QDebug>
#include <QFile>
#include <QString>

#include <windows.h>
#include <tlhelp32.h>
#include <winnt.h>

using namespace GammaRay;

ProbeABI ProbeABIDetector::abiForExecutable(const QString& path) const
{
  return ProbeABI::fromString(GAMMARAY_PROBE_ABI);
}

ProbeABI ProbeABIDetector::abiForProcess(qint64 pid) const
{
  MODULEENTRY32 me;
  me.dwSize = sizeof(MODULEENTRY32);
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
  if (snapshot == INVALID_HANDLE_VALUE) {
    return ProbeABI();
  }

  for (bool hasNext = Module32First(snapshot, &me); hasNext; hasNext = Module32Next(snapshot, &me)) {
    const QString module = QString::fromUtf16(reinterpret_cast<const ushort*>(me.szModule));
    if (module.contains("QtCore") || module.contains("Qt5Core")) {
      const QString path = QString::fromUtf16(reinterpret_cast<const ushort*>(me.szExePath));
      CloseHandle(snapshot);
      return abiForQtCore(path);
    }
  }

  CloseHandle(snapshot);
  return ProbeABI();
}


static QString archFromPEHeader(const IMAGE_FILE_HEADER *coffHdr)
{
  switch(coffHdr->Machine) {
    case IMAGE_FILE_MACHINE_I386: return "i686";
    case IMAGE_FILE_MACHINE_AMD64: return "x86_64";
  }

  return QString();
}

ProbeABI ProbeABIDetector::detectAbiForQtCore(const QString& path) const
{
  ProbeABI abi;

  // version
  DWORD pointlessHandle;
  DWORD fileVersionInfoSize = GetFileVersionInfoSize(path.toStdWString().c_str(), &pointlessHandle);
  if (!fileVersionInfoSize)
    return ProbeABI();

  BYTE *buffer = new BYTE[fileVersionInfoSize];
  if (GetFileVersionInfo(path.toStdWString().c_str(), pointlessHandle, fileVersionInfoSize, buffer)) {
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
  QFile f(path);
  if (!f.open(QFile::ReadOnly))
    return ProbeABI();

  const uchar* data = f.map(0, f.size());
  const uchar* end = data + f.size();
  if (!data || f.size() < sizeof(IMAGE_DOS_HEADER))
    return ProbeABI();

  const IMAGE_DOS_HEADER *dosHdr = reinterpret_cast<const IMAGE_DOS_HEADER*>(data);
  if (dosHdr->e_magic != IMAGE_DOS_SIGNATURE)
    return ProbeABI();
  data += dosHdr->e_lfanew;
  if (data + sizeof(quint32) >= end)
    return ProbeABI();

  const quint32 *peHdr = reinterpret_cast<const quint32*>(data);
  if (*peHdr != IMAGE_NT_SIGNATURE) {
    qWarning() << "pe signature" << *peHdr << IMAGE_NT_SIGNATURE;
    return ProbeABI();
  }
  data += sizeof(quint32);
  if (data + sizeof(IMAGE_FILE_HEADER) >= end)
    return ProbeABI();

  // architecture
  const IMAGE_FILE_HEADER* coffHdr = reinterpret_cast<const IMAGE_FILE_HEADER*>(data);
  abi.setArchitecture(archFromPEHeader(coffHdr));

  // TODO: debug/release, compiler

  return abi;
}
