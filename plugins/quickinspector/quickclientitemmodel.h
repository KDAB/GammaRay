/*
  quickclientitemmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKCLIENTITEMMODEL_H
#define GAMMARAY_QUICKINSPECTOR_QUICKCLIENTITEMMODEL_H

#include <ui/clientdecorationidentityproxymodel.h>

namespace GammaRay {
/** UI-dependent (and thus client-side) bits of the item tree model. */
class QuickClientItemModel : public ClientDecorationIdentityProxyModel
{
    Q_OBJECT
public:
    explicit QuickClientItemModel(QObject *parent = nullptr);
    ~QuickClientItemModel() override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};
}

#endif // GAMMARAY_QUICKCLIENTITEMMODEL_H
