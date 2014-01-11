/*
  networkdiscoverymodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_NETWORKDISCOVERYMODEL_H
#define GAMMARAY_NETWORKDISCOVERYMODEL_H

#include <QAbstractItemModel>
#include <QVector>
#include <QDateTime>

class QUdpSocket;

namespace GammaRay {

/** Lists all active server instances found via network autodiscovery. */
class NetworkDiscoveryModel : public QAbstractTableModel
{
  Q_OBJECT
public:
  enum Role {
    HostNameRole = Qt::UserRole + 1,
    PortRole
  };

  explicit NetworkDiscoveryModel(QObject* parent);
  ~NetworkDiscoveryModel();

  QVariant data(const QModelIndex& index, int role) const;
  int columnCount(const QModelIndex& parent) const;
  int rowCount(const QModelIndex& parent) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  Qt::ItemFlags flags(const QModelIndex& index) const;

private slots:
  void processPendingDatagrams();
  void expireEntries();

private:
  QUdpSocket *m_socket;

  struct ServerInfo {
    bool operator==(const ServerInfo &other);
    qint32 version;
    QString host;
    quint16 port;
    QString label;
    QDateTime lastSeen;
  };
  QVector<ServerInfo> m_data;

};

}

#endif // GAMMARAY_NETWORKDISCOVERYMODEL_H
