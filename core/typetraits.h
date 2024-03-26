/*
  typetraits.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_TYPETRAITS_H
#define GAMMARAY_TYPETRAITS_H

#include <type_traits>
#include <QtGlobal>
// Needed for QDeferredDeleteEvent after Qt 6.7
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
#include <private/qcoreevent_p.h>
#endif

namespace GammaRay {

/** Dynamic casting trait for supporting dynamic_cast-like functions for pseudo-polymorphic Qt types.
 *  The generic implementation in here just does dynamic_cast, this needs to be specialized where needed.
 */
template<typename Out, typename In>
Out DynamicCast(In *in)
{
    return dynamic_cast<Out>(in);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
template<>
inline QDeferredDeleteEvent *DynamicCast<QDeferredDeleteEvent *, QEvent>(QEvent *in)
{
    return static_cast<QDeferredDeleteEvent *>(in);
}
#endif

///@cond internal
template<typename Out>
Out DynamicCast(void *)
{
    return nullptr;
}
///@endcond

/** Specializable variant of std::is_polymorphic, for use with pseudo-polymorphic Qt types. */
template<typename T>
bool IsPolymorphic()
{
    return std::is_polymorphic<T>::value;
}

}

#endif
