/*
  sourcelocation.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_SOURCELOCATION_H
#define GAMMARAY_SOURCELOCATION_H

#include "gammaray_common_export.h"

#include <QMetaType>
#include <QDataStream>
#include <QUrl>
#include <QVector>

namespace GammaRay {
/*!
 * Specifies a source code location.
 *
 * A source location consists of a document and cursor position
 *
 * A Cursor represents a position in a Document through a tuple
 * of two ints, namely the @ref line() and @ref column().
 *
 * @note Lines and columns start a 0 (=> zero-based numbering).
 */
class GAMMARAY_COMMON_EXPORT SourceLocation
{
public:
    /*!
     * The default constructor creates a (invalid) cursor at position (-1, -1) with an invalid url
     */
    SourceLocation() = default;
    /*!
     * This constructor creates a (valid) cursor at position (0, 0) with @p url
     */
    explicit SourceLocation(const QUrl &url);
    ~SourceLocation();
    static SourceLocation fromZeroBased(const QUrl &url, int line, int column = 0);
    static SourceLocation fromOneBased(const QUrl &url, int line, int column = 1);

    bool operator==(const SourceLocation &other) const;

    bool isValid() const;

    QUrl url() const;
    void setUrl(const QUrl &url);

    /*!
     * Returns the zero-based line number.
     */
    int line() const;
    void setZeroBasedLine(int line);
    void setOneBasedLine(int line);

    /*!
     * Returns the zero-based column number.
     */
    int column() const;
    void setZeroBasedColumn(int column);
    void setOneBasedColumn(int column);

    /*!
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
    /*!
     * This constructor creates a (valid) cursor at given @p line and @p column with @p url.
     * Expects line and column to be zero-based.
     */
    explicit SourceLocation(const QUrl &url, int line, int column);
    friend GAMMARAY_COMMON_EXPORT QDataStream &operator<<(QDataStream &out,
                                                          const SourceLocation &location);
    friend GAMMARAY_COMMON_EXPORT QDataStream &operator>>(QDataStream &in,
                                                          SourceLocation &location);

    QUrl m_url;
    int m_line = -1;
    int m_column = -1;
};

///@cond internal
GAMMARAY_COMMON_EXPORT QDataStream &operator<<(QDataStream &out, const SourceLocation &location);
GAMMARAY_COMMON_EXPORT QDataStream &operator>>(QDataStream &in, SourceLocation &location);
///@endcond
}

Q_DECLARE_METATYPE(GammaRay::SourceLocation)
Q_DECLARE_METATYPE(QVector<GammaRay::SourceLocation>)

#endif // GAMMARAY_SOURCELOCATION_H
