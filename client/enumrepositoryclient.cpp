/*
  enumrepositoryclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "enumrepositoryclient.h"

#include <common/endpoint.h>
#include <common/objectbroker.h>

using namespace GammaRay;

EnumRepositoryClient::EnumRepositoryClient(QObject *parent)
    : EnumRepository(parent)
{
    ObjectBroker::registerObject<EnumRepository *>(this);

    connect(this, &EnumRepository::definitionResponse,
            this, &EnumRepositoryClient::definitionReceived);
}

EnumRepositoryClient::~EnumRepositoryClient() = default;

EnumDefinition EnumRepositoryClient::definition(EnumId id) const
{
    const auto def = EnumRepository::definition(id);
    if (!def.isValid())
        const_cast<EnumRepositoryClient *>(this)->requestDefinition(id);
    return def;
}

void EnumRepositoryClient::definitionReceived(const EnumDefinition &def)
{
    addDefinition(def);
    emit definitionChanged(def.id());
}

void EnumRepositoryClient::requestDefinition(EnumId id)
{
    Endpoint::instance()->invokeObject(qobject_interface_iid<EnumRepository *>(), "requestDefinition", QVariantList() << id);
}
