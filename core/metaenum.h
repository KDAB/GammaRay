/*
  metaenum.h

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

#ifndef GAMMARAY_METAENUM_H
#define GAMMARAY_METAENUM_H

#include <QStringList>

namespace GammaRay {
/** Enum/flag stringification utilities. */
namespace MetaEnum {
template<typename T>
struct Value {
    T value;
    const char * const name;
};

template<typename T, std::size_t N>
QString enumToString(T value, const Value<T>(&lookupTable)[N])
{
    for (std::size_t i = 0; i < N; ++i) {
        if (lookupTable[i].value == value)
            return QString::fromUtf8(lookupTable[i].name);
    }
    return QStringLiteral("unknown (") + QString::number(value) + ')';
}

template<typename T, typename F, std::size_t N>
QString flagsToString(T flags, const Value<F>(&lookupTable)[N])
{
    QStringList l;
    T handledFlags = T();

    for (std::size_t i = 0; i < N; ++i) {
        if (flags & lookupTable[i].value)
            l.push_back(QString::fromUtf8(lookupTable[i].name));
        handledFlags |= lookupTable[i].value;
    }

    if (flags & ~handledFlags)
        l.push_back(QStringLiteral("flag 0x")
                    + QString::number(qulonglong(flags & ~handledFlags), 16));

    if (l.isEmpty()) {
        // check if we have a special 0-value
        for (std::size_t i = 0; i < N; ++i) {
            if (lookupTable[i].value == 0)
                return QString::fromUtf8(lookupTable[i].name);
        }
        return QStringLiteral("<none>");
    }
    return l.join(QStringLiteral("|"));
}
}
}

#endif
