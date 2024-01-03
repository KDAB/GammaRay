/*
  connectionsclientproxymodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_CONNECTIONSCLIENTPROXYMODEL_H
#define GAMMARAY_CONNECTIONSCLIENTPROXYMODEL_H

#include <QSortFilterProxyModel>

namespace GammaRay {
/** Client-side part of the connections model. */
class ConnectionsClientProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ConnectionsClientProxyModel(QObject *parent = nullptr);
    ~ConnectionsClientProxyModel() override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};
}

#endif // GAMMARAY_CONNECTIONSCLIENTPROXYMODEL_H
