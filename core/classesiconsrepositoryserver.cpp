/*
  classesiconsrepositoryserver.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "classesiconsrepositoryserver.h"

#include <QHash>
#include "common/classesiconsindex_data.cpp"

using namespace GammaRay;

ClassesIconsRepositoryServer* ClassesIconsRepositoryServer::s_instance = nullptr;

ClassesIconsRepositoryServer::ClassesIconsRepositoryServer(QObject *parent)
    : ClassesIconsRepository(parent)
{
    if (ClassesIconsIndex::s_keyToNumber.isEmpty()) {
        ClassesIconsIndex::initializeClassesIconsIndex();
    }

    QVector<QString> index;
    index.resize(ClassesIconsIndex::s_numberToKey.size());

    for (auto it = ClassesIconsIndex::s_numberToKey.constBegin(),
         end = ClassesIconsIndex::s_numberToKey.constEnd(); it != end; ++it) {
        index[it.key()] = it.value();
    }

    setIndex(index);
}

ClassesIconsRepositoryServer::~ClassesIconsRepositoryServer()
{
    s_instance = nullptr;
}

void ClassesIconsRepositoryServer::create(QObject *parent)
{
    Q_ASSERT(!s_instance);
    s_instance = new ClassesIconsRepositoryServer(parent);
}

void ClassesIconsRepositoryServer::requestIndex()
{
    emit indexResponse(index());
}
