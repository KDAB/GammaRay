/*
  pefile.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "pefile.h"

using namespace GammaRay;

PEFile::PEFile(const QString &filePath)
    : m_file(filePath)
    , m_begin(nullptr)
    , m_end(nullptr)
    , m_fileHeader(nullptr)
    , m_importDesc(nullptr)
{
    if (!m_file.open(QFile::ReadOnly))
        return;

    m_begin = m_file.map(0, m_file.size());
    m_end = m_begin + m_file.size();
    if (!parse())
        close();
}

PEFile::~PEFile()
{
    close();
}

void PEFile::close()
{
    if (!m_begin)
        return;
    m_file.close();
    m_begin = nullptr;
    m_end = nullptr;
    m_fileHeader = nullptr;
    m_importDesc = nullptr;
}

bool PEFile::parse()
{
    const uchar *data = m_begin;
    if (!data || m_file.size() < sizeof(IMAGE_DOS_HEADER))
        return false;

    // check signatures
    const IMAGE_DOS_HEADER *dosHdr = reinterpret_cast<const IMAGE_DOS_HEADER *>(data);
    if (dosHdr->e_magic != IMAGE_DOS_SIGNATURE)
        return false;
    data += dosHdr->e_lfanew;
    if (data + sizeof(quint32) >= m_end)
        return false;

    const quint32 *peHdr = reinterpret_cast<const quint32 *>(data);
    if (*peHdr != IMAGE_NT_SIGNATURE)
        return false;
    data += sizeof(quint32);
    if (data + sizeof(IMAGE_FILE_HEADER) >= m_end)
        return false;

    // file header (for architecture)
    m_fileHeader = reinterpret_cast<const IMAGE_FILE_HEADER *>(data);
    data += sizeof(IMAGE_FILE_HEADER);
    if (data + sizeof(IMAGE_OPTIONAL_HEADER64) >= m_end)
        return false;

    // optional headers (for import descriptor)
    const IMAGE_OPTIONAL_HEADER32 *optHdr32
        = reinterpret_cast<const IMAGE_OPTIONAL_HEADER32 *>(data);
    if (optHdr32->Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
        data = rvaToFile(m_fileHeader,
                         optHdr32->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    } else {
        const IMAGE_OPTIONAL_HEADER64 *optHdr64
            = reinterpret_cast<const IMAGE_OPTIONAL_HEADER64 *>(data);
        if (optHdr64->Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
            return false;
        data = rvaToFile(m_fileHeader,
                         optHdr64->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    }
    if (data + sizeof(IMAGE_IMPORT_DESCRIPTOR) >= m_end)
        return false;
    m_importDesc = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR *>(data);

    return true;
}

bool PEFile::isValid() const
{
    return m_begin != nullptr;
}

QString PEFile::architecture() const
{
    if (!m_fileHeader)
        return QString();

    switch (m_fileHeader->Machine) {
    case IMAGE_FILE_MACHINE_I386:
        return QStringLiteral("i686");
    case IMAGE_FILE_MACHINE_AMD64:
        return QStringLiteral("x86_64");
    }

    return QString();
}

QStringList PEFile::imports() const
{
    QStringList libs;
    if (!m_importDesc)
        return libs;

    auto importDesc = m_importDesc;
    while (importDesc->Name) {
        const char *libraryName
            = reinterpret_cast<const char *>(rvaToFile(m_fileHeader, importDesc->Name));
        if (libraryName)
            libs.push_back(QString::fromLatin1(libraryName));
        importDesc++;
        if (reinterpret_cast<const uchar *>(importDesc) + sizeof(IMAGE_IMPORT_DESCRIPTOR) >= m_end)
            return QStringList();
    }

    return libs;
}

const uchar *PEFile::rvaToFile(const IMAGE_FILE_HEADER *hdr, DWORD rva) const
{
    Q_ASSERT(m_begin);
    Q_ASSERT(m_end);
    Q_ASSERT(hdr);

    const IMAGE_SECTION_HEADER *sectionHdr = sectionForRVA(hdr, rva);
    if (!sectionHdr)
        return nullptr;
    return m_begin + rva - sectionHdr->VirtualAddress + sectionHdr->PointerToRawData;
}

const IMAGE_SECTION_HEADER *PEFile::sectionForRVA(const IMAGE_FILE_HEADER *hdr, DWORD rva) const
{
    Q_ASSERT(hdr);
    Q_ASSERT(m_end);

    const uchar *data = reinterpret_cast<const uchar *>(hdr);
    auto sectionHdr
        = reinterpret_cast<const IMAGE_SECTION_HEADER *>(data + sizeof(IMAGE_FILE_HEADER)
                                                         + hdr->SizeOfOptionalHeader);
    for (int i = 0; i < hdr->NumberOfSections; ++i, ++sectionHdr) {
        if (reinterpret_cast<const uchar *>(sectionHdr +1) >= m_end)
            return nullptr;
        if (rva >= sectionHdr->VirtualAddress
            && rva < sectionHdr->VirtualAddress + sectionHdr->Misc.VirtualSize)
            return sectionHdr;
    }
    return nullptr;
}
