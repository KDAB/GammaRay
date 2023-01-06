/*
  classesiconsrepositoryclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "classesiconsrepositoryclient.h"

#include <common/endpoint.h>

using namespace GammaRay;

ClassesIconsRepositoryClient::ClassesIconsRepositoryClient(QObject *parent)
    : ClassesIconsRepository(parent)
{
    connect(this, &ClassesIconsRepository::indexResponse,
            this, &ClassesIconsRepositoryClient::indexReceived);
}

ClassesIconsRepositoryClient::~ClassesIconsRepositoryClient() = default;

QString ClassesIconsRepositoryClient::filePath(int id) const
{
    const QString filePath(ClassesIconsRepository::filePath(id));

    if (filePath.isEmpty() && !m_ready) {
        const_cast<ClassesIconsRepositoryClient *>(this)->requestIndex();
    }

    return filePath;
}

void ClassesIconsRepositoryClient::indexReceived(const QVector<QString> &index)
{
    setIndex(index);
    emit indexChanged();
}

void ClassesIconsRepositoryClient::requestIndex()
{
    m_ready = true;
    Endpoint::instance()->invokeObject(qobject_interface_iid<ClassesIconsRepository *>(), "requestIndex");
}
