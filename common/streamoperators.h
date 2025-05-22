/*
  streamoperators.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_STREAMOPERATORS_H
#define GAMMARAY_STREAMOPERATORS_H

#include <QDataStream>
#include <QMetaType>

namespace GammaRay {
/** Custom QDataStream streaming operators. */
namespace StreamOperators {
/** Call once early during startup. */
void registerOperators();

namespace Internal {
template<typename T>
QDataStream &writeEnum(QDataStream &out, T value)
{
    out << static_cast<qint32>(value);
    return out;
}

template<typename T>
QDataStream &readEnum(QDataStream &in, T &value)
{
    qint32 v;
    in >> v;
    value = static_cast<T>(v);
    return in;
}
}

/** Abstract away stream operator registration across Qt versions. */
template<typename T>
void registerOperators()
{
    qRegisterMetaType<T>();
}

}

#define GAMMARAY_ENUM_STREAM_OPERATORS(enumType)                           \
    QDataStream &operator<<(QDataStream &out, enumType value)              \
    {                                                                      \
        return GammaRay::StreamOperators::Internal::writeEnum(out, value); \
    }                                                                      \
    QDataStream &operator>>(QDataStream &in, enumType &value)              \
    {                                                                      \
        return GammaRay::StreamOperators::Internal::readEnum(in, value);   \
    }
}

#endif // GAMMARAY_STREAMOPERATORS_H
