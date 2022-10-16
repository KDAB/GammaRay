/*
  eventtypeclientproxymodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2019-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Tim Henning <tim.henning@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/
#ifndef GAMMARAY_EVENTMONITOR_EVENTTYPECLIENTPROXYMODEL_H
#define GAMMARAY_EVENTMONITOR_EVENTTYPECLIENTPROXYMODEL_H

#include <QIdentityProxyModel>

namespace GammaRay {
/** Colors the event counts based on the max events of a single type. */
class EventTypeClientProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit EventTypeClientProxyModel(QObject *parent = nullptr);
    ~EventTypeClientProxyModel() override;

    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};
}

#endif // GAMMARAY_EVENTMONITOR_EVENTTYPECLIENTPROXYMODEL_H
