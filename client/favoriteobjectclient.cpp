/*
  favoriteobjectclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Waqar Ahmed <waqar.ahmed@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/
#include "favoriteobjectclient.h"

#include <common/endpoint.h>
#include <common/objectbroker.h>

using namespace GammaRay;

FavoriteObjectClient::FavoriteObjectClient(QObject *parent)
    : FavoriteObjectInterface(parent)
{
    ObjectBroker::registerObject<FavoriteObjectInterface *>(this);
}

void FavoriteObjectClient::markObjectAsFavorite(const ObjectId &id)
{
    Endpoint::instance()->invokeObject(objectName(), "markObjectAsFavorite", QVariantList() << QVariant::fromValue(id));
}

void FavoriteObjectClient::unfavoriteObject(const ObjectId &id)
{
    Endpoint::instance()->invokeObject(objectName(), "unfavoriteObject", QVariantList() << QVariant::fromValue(id));
}
