/*
  classesiconsindex.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
