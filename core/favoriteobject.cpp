/*
  favoriteobject.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Waqar Ahmed <waqar.ahmed@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/
#include "favoriteobject.h"

#include "probe.h"

using namespace GammaRay;

FavoriteObject::FavoriteObject(QObject *parent)
    : FavoriteObjectInterface(parent)
{
}

void FavoriteObject::markObjectAsFavorite(const ObjectId &id)
{
    Probe::instance()->markObjectAsFavorite(id.asQObject());
}

void FavoriteObject::unfavoriteObject(const ObjectId &id)
{
    Probe::instance()->removeObjectAsFavorite(id.asQObject());
}
