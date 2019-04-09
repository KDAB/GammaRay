/*
  streamoperators.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_STREAMOPERATORS_H
#define GAMMARAY_STREAMOPERATORS_H

#include <QDataStream>

namespace GammaRay {
/** Custom QDataStream streaming operators. */
namespace StreamOperators {
/** Call once early during startup. */
void registerOperators();

namespace Internal {
template<typename T> QDataStream &writeEnum(QDataStream &out, T value)
{
    out << static_cast<qint32>(value);
    return out;
}

template<typename T> QDataStream &readEnum(QDataStream &in, T &value)
{
    qint32 v;
    in >> v;
    value = static_cast<T>(v);
    return in;
}
}
}

#define GAMMARAY_ENUM_STREAM_OPERATORS(enumType) \
    QDataStream &operator<<(QDataStream &out, enumType value) \
    { return GammaRay::StreamOperators::Internal::writeEnum(out, value); } \
    QDataStream &operator>>(QDataStream &in, enumType &value) \
    { return GammaRay::StreamOperators::Internal::readEnum(in, value); }
}

#endif // GAMMARAY_STREAMOPERATORS_H
