/*
  pefile.h

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
