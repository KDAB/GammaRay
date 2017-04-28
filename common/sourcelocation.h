/*
  sourcelocation.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_SOURCELOCATION_H
#define GAMMARAY_SOURCELOCATION_H

#include "gammaray_common_export.h"

#include <QMetaType>
#include <QDataStream>
#include <QUrl>

namespace GammaRay {
/**
 * @brief Specifies a source code location.
 *
 * A source location consists of a document and cursor position
 *
 * A Cursor represents a position in a Document through a tuple
 * of two ints, namely the @ref line() and @ref column().
 *
 * Notes
 * - Lines and columns start a 0 (=> zero-based numbering)
 */
class GAMMARAY_COMMON_EXPORT SourceLocation
{
public:
    /**
     * The default constructor creates a (invalid) cursor at position (-1, -1) with an invalid url
     */
    SourceLocation();
    /**
     * This constructor creates a (valid) cursor at position (0, 0) with @p url
     */
    explicit SourceLocation(const QUrl &url, int line = 0, int column = 0);
    ~SourceLocation();

    bool isValid() const;

    QUrl url() const;
    void setUrl(const QUrl &url);

    int line() const;
    void setLine(int line);

    int column() const;
    void setColumn(int column);

    /**
     * Returns a human-readable version of this source location
     *
     * @code
     * SourceLocation loc(QUrl::fromLocalFile("file.cpp", 0, 0);
     * qDebug() << loc.displayString();
     *
     * => Prints: file.cpp:1:1
     * @endcode
     *
     * @note This will use one-based numbering (file.cpp:1:1 instead of file.cpp:0:0)
     */
    QString displayString() const;

private:
    friend GAMMARAY_COMMON_EXPORT QDataStream &operator<<(QDataStream &out,
                                                          const SourceLocation &location);
    friend GAMMARAY_COMMON_EXPORT QDataStream &operator>>(QDataStream &in,
                                                          SourceLocation &location);

    QUrl m_url;
    int m_line;
    int m_column;
};

GAMMARAY_COMMON_EXPORT QDataStream &operator<<(QDataStream &out, const SourceLocation &location);
GAMMARAY_COMMON_EXPORT QDataStream &operator>>(QDataStream &in, SourceLocation &location);
}

Q_DECLARE_METATYPE(GammaRay::SourceLocation)

#endif // GAMMARAY_SOURCELOCATION_H
