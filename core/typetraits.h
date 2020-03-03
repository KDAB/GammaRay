/*
  typetraits.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_TYPETRAITS_H
#define GAMMARAY_TYPETRAITS_H

#include <type_traits>

namespace GammaRay {

/** Dynamic casting trait for supporting dynamic_cast-like functions for pseudo-polymorphic Qt types.
 *  The generic implementation in here just does dynamic_cast, this needs to be specialized where needed.
 */
template <typename Out, typename In>
Out DynamicCast(In *in)
{
    return dynamic_cast<Out>(in);
}

///@cond internal
template <typename Out>
Out DynamicCast(void*)
{
    return nullptr;
}
///@endcond

/** Specializable variant of std::is_polymorphic, for use with pseudo-polymorphic Qt types. */
template <typename T>
bool IsPolymorphic()
{
    return std::is_polymorphic<T>::value;
}

}

#endif
