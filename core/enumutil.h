/*
  enumutil.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
namespace EnumUtil {
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
