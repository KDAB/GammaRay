/*
  favoriteobjectinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Waqar Ahmed <waqar.ahmed@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_FAVORITEOBJECTINTERFACE_H
#define GAMMARAY_FAVORITEOBJECTINTERFACE_H

#include "objectid.h"

#include <QObject>

namespace GammaRay {
class FavoriteObjectInterface : public QObject
{
    Q_OBJECT

public:
    explicit FavoriteObjectInterface(QObject *parent = nullptr);
    ~FavoriteObjectInterface() override;

    /*! Mark an object as favorited. */
    virtual void markObjectAsFavorite(const GammaRay::ObjectId &id) = 0;

    /*! Mark an object as not favorited */
    virtual void unfavoriteObject(const GammaRay::ObjectId &id) = 0;

private:
    Q_DISABLE_COPY(FavoriteObjectInterface)
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::FavoriteObjectInterface,
                    "com.kdab.GammaRay.FavoriteObjectInterface")
QT_END_NAMESPACE

#endif // GAMMARAY_FAVORITEOBJECTINTERFACE_H
