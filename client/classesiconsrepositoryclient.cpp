/*
  classesiconsrepositoryclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
