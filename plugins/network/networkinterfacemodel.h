/*
  networkinterfacemodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_NETWORKINTERFACEMODEL_H
#define GAMMARAY_NETWORKINTERFACEMODEL_H

#include <QAbstractItemModel>
#include <QList>

QT_BEGIN_NAMESPACE
class QNetworkInterface;
QT_END_NAMESPACE

namespace GammaRay {
class NetworkInterfaceModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit NetworkInterfaceModel(QObject *parent = nullptr);
    ~NetworkInterfaceModel() override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;

private:
    QList<QNetworkInterface> m_interfaces;
};
}

#endif // GAMMARAY_NETWORKINTERFACEMODEL_H
