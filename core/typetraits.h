/*
  typetraits.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_TYPETRAITS_H
#define GAMMARAY_TYPETRAITS_H

#include <type_traits>

namespace GammaRay {

/** Dynamic casting trait for supporting dynamic_cast-like functions for pseudo-polymorphic Qt types.
 *  The generic implementation in here just does dynamic_cast, this needs to be specialized where needed.
 */
template<typename Out, typename In>
Out DynamicCast(In *in)
{
    return dynamic_cast<Out>(in);
}

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
