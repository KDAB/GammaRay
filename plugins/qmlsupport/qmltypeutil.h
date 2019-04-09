/*
  qmltypeutil.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef QMLTYPEUTIL_H
#define QMLTYPEUTIL_H

#include <core/objectinstance.h>

#include <private/qqmlmetatype_p.h>
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 2)
#define GAMMARAY_QQMLTYPE_IS_VALUE_TYPE
#else
#undef GAMMARAY_QQMLTYPE_IS_VALUE_TYPE
#endif

#ifdef GAMMARAY_QQMLTYPE_IS_VALUE_TYPE
Q_DECLARE_METATYPE(QQmlType)
#endif

namespace GammaRay {

/** Helper functions to deal with QQmlType changing from pointer to value type in Qt 5.9.2 */
namespace QmlType {

inline bool isValid(QQmlType *type)
{
    return type;
}

#ifdef GAMMARAY_QQMLTYPE_IS_VALUE_TYPE
inline bool isValid(const QQmlType &type)
{
    return type.isValid();
}
#endif

inline QQmlType* callable(QQmlType *type)
{
    return type;
}

inline const QQmlType* callable(const QQmlType &type)
{
    return &type;
}

inline ObjectInstance toObjectInstance(QQmlType *type)
{
    return ObjectInstance(type, "QQmlType");
}

#ifdef GAMMARAY_QQMLTYPE_IS_VALUE_TYPE
inline ObjectInstance toObjectInstance(const QQmlType &type)
{
    return ObjectInstance(QVariant::fromValue(type));
}
#endif

}

}

#endif
