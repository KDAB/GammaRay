/*
  wlcompositorinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
    enum Columns {
        PidColumn,
        CommandColumn,
        EndColumn
    };

    enum Roles {
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
