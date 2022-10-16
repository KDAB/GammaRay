/*
  qmltypeutil.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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

inline QQmlType *callable(QQmlType *type)
{
    return type;
}

inline const QQmlType *callable(const QQmlType &type)
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
