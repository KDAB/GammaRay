/*
  enumutil.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_ENUMUTIL_H
#define GAMMARAY_ENUMUTIL_H

#include "gammaray_core_export.h"

#include <qglobal.h>

QT_BEGIN_NAMESPACE
class QMetaEnum;
struct QMetaObject;
class QString;
class QVariant;
QT_END_NAMESPACE

namespace GammaRay {

/*! Enum conversion utility functions. */
namespace EnumUtil
{
/*!
 * Determine the QMetaEnum for a given enum/flag value.
 * @param value A QVariant holding the enum/flag value, numerical or
 * as actual enum/flag.
 * @param typeName A type name hint, in case @p value holds a numeric value.
 * @param metaObject A meta object hint holding the enum definition.
 * @since 2.6
 */
GAMMARAY_CORE_EXPORT QMetaEnum metaEnum(const QVariant &value, const char *typeName = nullptr, const QMetaObject *metaObject = nullptr);

/*!
 * Converts the given enum or flag value into its numerical representation.
 * @param value A QVariant holding the enum/flag value, numerical or
 * as actual enum/flag.
 * @param metaEnum The QMetaEnum defining the enum/flag.
 * @since 2.6
 */
GAMMARAY_CORE_EXPORT int enumToInt(const QVariant &value, const QMetaEnum &metaEnum);

/*!
 * Translates an enum or flag value into a human readable text.
 * @param value The numerical value. Type information from the QVariant
 *              are used to find the corresponding QMetaEnum.
 * @param typeName Use this if the @p value has type int
 *                 (e.g. the case for QMetaProperty::read).
 * @param metaObject QMetaObject possibly containing the definition of the enum.
 *
 * @return a QString containing the string version of the specified @p value.
 * @since 2.6
 */
GAMMARAY_CORE_EXPORT QString enumToString(const QVariant &value, const char *typeName = nullptr,
                                          const QMetaObject *metaObject = nullptr);
}

}

#endif // GAMMARAY_ENUMUTIL_H
