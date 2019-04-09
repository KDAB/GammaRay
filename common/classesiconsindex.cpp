/*
  classesiconsindex.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

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

#include "classesiconsindex.h"

#include "classesiconsindex_data.cpp"

using namespace GammaRay;

ClassesIconsIndex::ConstIterator ClassesIconsIndex::constBegin()
{
    if (s_keyToNumber.isEmpty()) {
        initializeClassesIconsIndex();
    }

    return s_keyToNumber.constBegin();
}

ClassesIconsIndex::ConstIterator ClassesIconsIndex::constEnd()
{
    if (s_keyToNumber.isEmpty()) {
        initializeClassesIconsIndex();
    }

    return s_keyToNumber.constEnd();
}

int ClassesIconsIndex::iconIdForName(const QString &name)
{
    if (s_keyToNumber.isEmpty()) {
        initializeClassesIconsIndex();
    }

    return s_keyToNumber.value(name, -1);
}

QString ClassesIconsIndex::iconNameForId(int id)
{
    if (s_numberToKey.isEmpty()) {
        initializeClassesIconsIndex();
    }

    return s_numberToKey.value(id, QString());
}
