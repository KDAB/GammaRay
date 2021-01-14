/*
  classesiconsrepository.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "classesiconsrepository.h"

#include <common/objectbroker.h>

#include <QDebug>

using namespace GammaRay;

ClassesIconsRepository::ClassesIconsRepository(QObject *parent)
    : QObject(parent)
{
    ObjectBroker::registerObject<ClassesIconsRepository *>(this);
}

ClassesIconsRepository::~ClassesIconsRepository() = default;

QString ClassesIconsRepository::filePath(int id) const
{
    if (id >= m_iconsIndex.size() || id < 0)
        return QString();

    return m_iconsIndex.at(id);
}

ClassesIconsRepository::ConstIterator ClassesIconsRepository::constBegin()
{
    return m_iconsIndex.constBegin();
}

ClassesIconsRepository::ConstIterator ClassesIconsRepository::constEnd()
{
    return m_iconsIndex.constEnd();
}

QVector<QString> ClassesIconsRepository::index() const
{
    return m_iconsIndex;
}

void ClassesIconsRepository::setIndex(const QVector<QString> &index)
{
    m_iconsIndex = index;
}
