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

#include <QDebug>
#include <QFile>
#include <QString>
#include <QStringList>

#include <windows.h>
#include <tlhelp32.h>
#include <winnt.h>

using namespace GammaRay;

ProbeABI ProbeABIDetector::abiForExecutable(const QString& path) const
{
  Q_UNUSED(path);
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
    if (containsQtCore(module.toUtf8())) {
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

static const IMAGE_SECTION_HEADER* sectionForRVA(const IMAGE_FILE_HEADER *hdr, DWORD rva, const uchar *end)
{
  const uchar *data = reinterpret_cast<const uchar*>(hdr);
  const IMAGE_SECTION_HEADER* sectionHdr = reinterpret_cast<const IMAGE_SECTION_HEADER*>(data + sizeof(IMAGE_FILE_HEADER) + hdr->SizeOfOptionalHeader);
  for (int i = 0; i < hdr->NumberOfSections; ++i, ++sectionHdr) {
    if (reinterpret_cast<const uchar*>(sectionHdr +1) >= end)
      return 0;
    if (rva >= sectionHdr->VirtualAddress && rva < sectionHdr->VirtualAddress + sectionHdr->Misc.VirtualSize)
      return sectionHdr;
  }
  return 0;
}

static const uchar* rvaToFile(const IMAGE_FILE_HEADER *hdr, DWORD rva, const uchar *begin, const uchar *end)
{
  const IMAGE_SECTION_HEADER* sectionHdr = sectionForRVA(hdr, rva, end);
  if (!sectionHdr)
    return 0;
  return begin + rva - sectionHdr->VirtualAddress + sectionHdr->PointerToRawData;
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
  QFile f(path);
  if (!f.open(QFile::ReadOnly))
    return ProbeABI();

  const uchar* const begin = f.map(0, f.size());
  const uchar* data = begin;
  const uchar* const end = begin + f.size();
  if (!data || f.size() < sizeof(IMAGE_DOS_HEADER))
    return ProbeABI();

  const IMAGE_DOS_HEADER *dosHdr = reinterpret_cast<const IMAGE_DOS_HEADER*>(data);
  if (dosHdr->e_magic != IMAGE_DOS_SIGNATURE)
    return ProbeABI();
  data += dosHdr->e_lfanew;
  if (data + sizeof(quint32) >= end)
    return ProbeABI();

  const quint32 *peHdr = reinterpret_cast<const quint32*>(data);
  if (*peHdr != IMAGE_NT_SIGNATURE)
    return ProbeABI();
  data += sizeof(quint32);
  if (data + sizeof(IMAGE_FILE_HEADER) >= end)
    return ProbeABI();

  // architecture
  const IMAGE_FILE_HEADER* coffHdr = reinterpret_cast<const IMAGE_FILE_HEADER*>(data);
  abi.setArchitecture(archFromPEHeader(coffHdr));
  data += sizeof(IMAGE_FILE_HEADER);
  if (data + sizeof(IMAGE_OPTIONAL_HEADER64) >= end)
    return ProbeABI();

  // import table
  const IMAGE_OPTIONAL_HEADER32 *optHdr32 = reinterpret_cast<const IMAGE_OPTIONAL_HEADER32*>(data);
  if (optHdr32->Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    data = rvaToFile(coffHdr, optHdr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress, begin, end);
  } else {
    const IMAGE_OPTIONAL_HEADER64 *optHdr64 = reinterpret_cast<const IMAGE_OPTIONAL_HEADER64*>(data);
    if (optHdr64->Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
      return ProbeABI();
    data = rvaToFile(coffHdr, optHdr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress, begin, end);
  }
  if (data + sizeof(IMAGE_IMPORT_DESCRIPTOR) >= end)
    return ProbeABI();

  const IMAGE_IMPORT_DESCRIPTOR* importDesc = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR*>(data);
  QStringList libs;
  while (importDesc->Name) {
    const char* libraryName = reinterpret_cast<const char*>(rvaToFile(coffHdr, importDesc->Name, begin, end));
    if (libraryName)
      libs.push_back(QString::fromAscii(libraryName));
    importDesc++;
    if (reinterpret_cast<const uchar*>(importDesc) + sizeof(IMAGE_IMPORT_DESCRIPTOR) >= end)
      return ProbeABI();
  }

  // compiler and debug mode
  abi.setCompiler(compilerFromLibraries(libs));
  if (abi.compiler() == "MSVC")
    abi.setIsDebug(isDebugRuntime(libs));

  return abi;
}
