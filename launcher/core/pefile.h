/*
  pefile.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PEFILE_H
#define GAMMARAY_PEFILE_H

#include <QFile>
#include <QStringList>

#include <qt_windows.h>

namespace GammaRay {
/** Convenience API to deal with extracting information from PE files. */
class PEFile
{
public:
    explicit PEFile(const QString &filePath);
    ~PEFile();

    bool isValid() const;
    QString architecture() const;
    QStringList imports() const;

private:
    Q_DISABLE_COPY(PEFile)
    void close();
    bool parse();

    const uchar *rvaToFile(const IMAGE_FILE_HEADER *hdr, DWORD rva) const;
    const IMAGE_SECTION_HEADER *sectionForRVA(const IMAGE_FILE_HEADER *hdr, DWORD rva) const;

    QFile m_file;
    const uchar *m_begin;
    const uchar *m_end;
    const IMAGE_FILE_HEADER *m_fileHeader;
    const IMAGE_IMPORT_DESCRIPTOR *m_importDesc;
};
}

#endif // GAMMARAY_PEFILE_H
