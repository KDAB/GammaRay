/*
  quickclientitemmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
