/*
  classesiconsrepository.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
