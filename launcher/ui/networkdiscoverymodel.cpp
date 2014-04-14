/*
  networkdiscoverymodel.cpp

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

#include "networkdiscoverymodel.h"

#include <common/endpoint.h>

#include <QDataStream>
#include <QUdpSocket>
#include <QTimer>

#include <algorithm>

using namespace GammaRay;

bool NetworkDiscoveryModel::ServerInfo::operator==(const NetworkDiscoveryModel::ServerInfo& other)
{
  return host == other.host && port == other.port;
}

NetworkDiscoveryModel::NetworkDiscoveryModel(QObject* parent):
  QAbstractTableModel(parent),
  m_socket(new QUdpSocket(this))
{
  m_socket->bind(Endpoint::broadcastPort(), QUdpSocket::ShareAddress);
  connect(m_socket, SIGNAL(readyRead()), SLOT(processPendingDatagrams()));

  QTimer *expireTimer = new QTimer(this);
  expireTimer->setInterval(15 * 1000);
  expireTimer->setSingleShot(false);
  connect(expireTimer, SIGNAL(timeout()), SLOT(expireEntries()));
  expireTimer->start();
}

NetworkDiscoveryModel::~NetworkDiscoveryModel()
{
}

void NetworkDiscoveryModel::processPendingDatagrams()
{
  while (m_socket->hasPendingDatagrams()) {
    QByteArray datagram;
    datagram.resize(m_socket->pendingDatagramSize());
    m_socket->readDatagram(datagram.data(), datagram.size());

    QDataStream stream(datagram);
    qint32 broadcastVersion;
    stream >> broadcastVersion;
    if (broadcastVersion != Protocol::broadcastFormatVersion())
      continue;

    ServerInfo info;
    stream >> info.version >> info.host >> info.port >> info.label;
    info.lastSeen = QDateTime::currentDateTime();

    QVector<ServerInfo>::iterator it = std::find(m_data.begin(), m_data.end(), info);
    if (it == m_data.end()) {
      beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
      m_data.push_back(info);
      endInsertRows();
    } else {
      it->lastSeen = info.lastSeen;
    }
  }
}

void NetworkDiscoveryModel::expireEntries()
{
  const QDateTime threshold = QDateTime::currentDateTime().addSecs(-30);
  for (QVector<ServerInfo>::iterator it = m_data.begin(); it != m_data.end();) {
    if (it->lastSeen >= threshold) {
      ++it;
    } else {
      const int currentRow = std::distance(m_data.begin(), it);
      beginRemoveRows(QModelIndex(), currentRow, currentRow);
      it = m_data.erase(it);
      endRemoveRows();
    }
  }
}

QVariant NetworkDiscoveryModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  const ServerInfo &info = m_data.at(index.row());
  if (role == Qt::DisplayRole) {
    switch (index.column()) {
      case 0: return info.label;
      case 1: return QVariant(info.host + QLatin1Char(':') + QString::number(info.port));
    }
  } else if (role == Qt::ToolTipRole) {
    if (info.version != Protocol::version())
      return tr("Incompatible GammaRay version.");
  } else if (role == HostNameRole) {
    return info.host;
  } else if (role == PortRole) {
    return info.port;
  }

  return QVariant();
}

int NetworkDiscoveryModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 2;
}

int NetworkDiscoveryModel::rowCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return 0;
  return m_data.size();
}

QVariant NetworkDiscoveryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (section) {
      case 0: return tr("Name");
      case 1: return tr("Host");
    }
  }
  return QVariant();
}

Qt::ItemFlags NetworkDiscoveryModel::flags(const QModelIndex& index) const
{
  const Qt::ItemFlags baseFlags = QAbstractItemModel::flags(index);
  if (!index.isValid())
    return baseFlags;

  const ServerInfo &info = m_data.at(index.row());
  if (info.version != Protocol::version())
    return baseFlags & ~(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
  return baseFlags;
}

