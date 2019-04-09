/*
  metaenum.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <type_traits>

namespace GammaRay {
/** Enum/flag stringification utilities. */
namespace MetaEnum {
template<typename T>
struct Value {
    T value;
    const char * const name;
};

template<typename T, std::size_t N>
std::size_t count(const Value<T>(&)[N])
{
    return N;
}

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
        if (flags & lookupTable[i].value) {
            l.push_back(QString::fromUtf8(lookupTable[i].name));
        }
        handledFlags |= lookupTable[i].value;
    }

    if (flags & ~handledFlags) {
        l.push_back(QStringLiteral("flag 0x") +
                    QString::number(qulonglong(flags & ~handledFlags), 16));
    }

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

// functors for easy use with VariantHandler::registerStringConverter
namespace detail {
template <typename T, std::size_t N>
class enum_to_string_functor
{
public:
    explicit enum_to_string_functor(const MetaEnum::Value<T>(&lookupTable)[N])
        : m_lookupTable(lookupTable)
    {}

    QString operator()(T value)
    {
        return MetaEnum::enumToString(value, m_lookupTable);
    }

private:
    const MetaEnum::Value<T> (&m_lookupTable)[N];

};

template <typename T, std::size_t N>
class flags_to_string_functor
{
public:
    explicit flags_to_string_functor(const MetaEnum::Value<T>(&lookupTable)[N])
        : m_lookupTable(lookupTable)
    {}

#if !defined(Q_CC_MSVC) || _MSC_VER >= 1900 //krazy:exclude=cpp to deal with older MS compilers
    QString operator()(typename std::underlying_type<T>::type value)
#else
    QString operator()(unsigned int value)
#endif
    {
        return MetaEnum::flagsToString(value, m_lookupTable);
    }

private:
    const MetaEnum::Value<T> (&m_lookupTable)[N];

};

}

/** Creates a functor for MetaEnum::enumToString and a specific lookup table. */
template <typename T, std::size_t N>
detail::enum_to_string_functor<T, N> enumToString_fn(const Value<T>(&lookupTable)[N])
{
    return detail::enum_to_string_functor<T, N>(lookupTable);
}

/** Creates a functor for MetaEnum::flagsToString and a specific lookup table. */
template <typename T, std::size_t N>
detail::flags_to_string_functor<T, N> flagsToString_fn(const Value<T>(&lookupTable)[N])
{
    return detail::flags_to_string_functor<T, N>(lookupTable);
}

} // MetaEnum
} // GammaRay

#endif
