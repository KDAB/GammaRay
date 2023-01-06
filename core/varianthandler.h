/*
  varianthandler.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_VARIANTHANDLER_H
#define GAMMARAY_VARIANTHANDLER_H

#include "gammaray_core_export.h"

#include <QVariant>

namespace GammaRay {
/*! Variant conversion functions, extendable by plugins. */
namespace VariantHandler {
///@cond internal
template<typename RetT>
struct Converter
{
    virtual ~Converter()
    {
    }

    virtual RetT operator()(const QVariant &v) = 0;
};

template<typename RetT, typename InputT,
         typename FuncT>
struct ConverterImpl : public Converter<RetT>
{
    explicit inline ConverterImpl(FuncT converter)
        : f(converter)
    {
    }

    RetT operator()(const QVariant &v) override
    {
        return f(v.value<InputT>());
    }

    FuncT f;
};
///@endcond

/*!
 * Returns a human readable string version of the QVariant value.
 * Converts to the variant type and prints the string value accordingly.
 * @param value is a QVariant.
 *
 * @return a QString containing the human readable string.
 */
GAMMARAY_CORE_EXPORT QString displayString(const QVariant &value);

/*!
 * Returns a human readable string version of the given value.
 * Thihs is a convenience overload of the QVariant-based version above.
 *
 * @return a QString containing the human readable string.
 */
template<typename T>
inline QString displayString(T value)
{
    return displayString(QVariant::fromValue<T>(value));
}

/*!
 * Returns a value representing @p value in a itemview decoration role.
 * @param value is a QVariant.
 *
 * @return a QVariant itemview decoration role.
 */
GAMMARAY_CORE_EXPORT QVariant decoration(const QVariant &value);

/*!
 * Register a string conversion functions for a variant type.
 * @internal
 */
GAMMARAY_CORE_EXPORT void registerStringConverter(int type, Converter<QString> *converter);

/*!
 * Register a string conversion function for a variant type.
 * @tparam T The type for which to use this converter function.
 */
template<typename T, typename FuncT>
inline void registerStringConverter(FuncT f)
{
    Converter<QString> *converter = new ConverterImpl<QString, T, FuncT>(f);
    registerStringConverter(qMetaTypeId<T>(), converter);
}

/*! Callback type for generic string converters. */
typedef QString (*GenericStringConverter)(const QVariant &value, bool *ok);
/*!
 * Register a generic string conversion function for various variant types.
 * This can be used when you have a converter that can dynamically check if
 * it can handle a given variant, and the types it can handle aren't known
 * at compile time (example: QQmlListProperty).
 * @param converter The converter function. It's second parameter is used to
 * indicate if the value could be handled.
 */
GAMMARAY_CORE_EXPORT void registerGenericStringConverter(GenericStringConverter converter);

/*!
 * Converts the given variant into an variant of a different type that can be transferred
 * to another process.
 * This is most prominently needed for pointer types, e.g. const QMatrix4x4* -> QMatrix4x4,
 * primarily for the fancy display delegate on the client side.
 */
GAMMARAY_CORE_EXPORT QVariant serializableVariant(const QVariant &value);

///@cond internal
void clear();
///@endcond
}
}

#endif // GAMMARAY_VARIANTHANDLER_H
