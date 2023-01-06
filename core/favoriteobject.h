/*
  favoriteobject.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Waqar Ahmed <waqar.ahmed@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/
#ifndef GAMMARAY_FAVORITEOBJECT_H
#define GAMMARAY_FAVORITEOBJECT_H

#include <common/favoriteobjectinterface.h>

namespace GammaRay {
class FavoriteObject : public FavoriteObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::FavoriteObjectInterface)
public:
    explicit FavoriteObject(QObject *parent = nullptr);

    Q_INVOKABLE void markObjectAsFavorite(const GammaRay::ObjectId &id) override;
    Q_INVOKABLE void unfavoriteObject(const GammaRay::ObjectId &id) override;
};
}

#endif // GAMMARAY_FAVORITEOBJECT_H
