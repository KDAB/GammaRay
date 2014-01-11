/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#ifndef GAMMARAY_SETTEMPVALUE_H
#define GAMMARAY_SETTEMPVALUE_H

namespace GammaRay {

/**
 * @brief GammaRay utilities.
 */
namespace Util {

  /**
   * Temporarily overwrite a given object with a new value and reset the value
   * when the scope is exited.
   */
  template<class T>
  struct SetTempValue
  {
    SetTempValue(T& obj, T newValue)
    : obj(obj)
    , oldValue(obj)
    {
      obj = newValue;
    }
    ~SetTempValue()
    {
      obj = oldValue;
    }
    T& obj;
    T oldValue;
  };
}

}

#endif // GAMMARAY_SETTEMPVALUE_H
