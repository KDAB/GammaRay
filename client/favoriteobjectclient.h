/*
  favoriteobjectclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Waqar Ahmed <waqar.ahmed@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_FAVORITEOBJECTCLIENT_H
#define GAMMARAY_FAVORITEOBJECTCLIENT_H

#include <common/favoriteobjectinterface.h>

namespace GammaRay {
class FavoriteObjectClient : public FavoriteObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::FavoriteObjectInterface)
public:
    explicit FavoriteObjectClient(QObject *parent = nullptr);

    void markObjectAsFavorite(const GammaRay::ObjectId &id) override;
    void unfavoriteObject(const GammaRay::ObjectId &id) override;
};
}

#endif // GAMMARAY_FAVORITEOBJECTCLIENT_H
