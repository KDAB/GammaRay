/*
  classesiconsrepositoryserver.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "classesiconsrepositoryserver.h"

#include <QHash>
#include "common/classesiconsindex_data.cpp"

using namespace GammaRay;

ClassesIconsRepositoryServer *ClassesIconsRepositoryServer::s_instance = nullptr;

ClassesIconsRepositoryServer::ClassesIconsRepositoryServer(QObject *parent)
    : ClassesIconsRepository(parent)
{
    if (ClassesIconsIndex::s_keyToNumber.isEmpty()) {
        ClassesIconsIndex::initializeClassesIconsIndex();
    }

    QVector<QString> index;
    index.resize(ClassesIconsIndex::s_numberToKey.size());

    for (auto it = ClassesIconsIndex::s_numberToKey.constBegin(),
              end = ClassesIconsIndex::s_numberToKey.constEnd();
         it != end; ++it) {
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
