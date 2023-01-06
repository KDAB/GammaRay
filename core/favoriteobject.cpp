/*
  favoriteobject.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Waqar Ahmed <waqar.ahmed@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
