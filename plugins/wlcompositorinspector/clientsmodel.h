/*
  clientsmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_CLIENTSMODEL_H
#define GAMMARAY_CLIENTSMODEL_H

#include <common/modelroles.h>

#include <QAbstractTableModel>
#include <QVector>

QT_BEGIN_NAMESPACE
class QWaylandClient;
QT_END_NAMESPACE

namespace GammaRay {

class ClientsModel : public QAbstractTableModel
{
public:
    enum Columns
    {
        PidColumn,
        CommandColumn,
        EndColumn
    };

    enum Roles
    {
        ObjectIdRole = UserRole + 1
    };

    explicit ClientsModel(QObject *parent);
    ~ClientsModel();

    QWaylandClient *client(int index) const;

    void addClient(QWaylandClient *client);
    void removeClient(QWaylandClient *client);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int) const override;

private:
    QVector<QWaylandClient *> m_clients;
};

}

#endif
