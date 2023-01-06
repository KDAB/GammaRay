/*
  classesiconsindex.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
