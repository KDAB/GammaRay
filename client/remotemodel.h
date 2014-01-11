/*
  remotemodel.h

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

#ifndef GAMMARAY_REMOTEMODEL_H
#define GAMMARAY_REMOTEMODEL_H

#include <common/protocol.h>

#include <QAbstractItemModel>
#include <QVector>

namespace GammaRay {

class Message;

class RemoteModel : public QAbstractItemModel
{
  Q_OBJECT
  public:
    explicit RemoteModel(const QString &serverObject, QObject *parent = 0);
    ~RemoteModel();

    bool isConnected() const;

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& child) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  public slots:
    void newMessage(const GammaRay::Message &msg);
    void serverRegistered(const QString &objectName, Protocol::ObjectAddress objectAddress);
    void serverUnregistered(const QString& objectName, Protocol::ObjectAddress objectAddress);

  private:
    struct Node { // represents one row
      Node() : parent(0), rowCount(-1), columnCount(-1) {}
      ~Node();
      Node* parent;
      QVector<Node*> children;
      qint32 rowCount;
      qint32 columnCount;
      QHash<int, QHash<int, QVariant> > data; // column -> role -> data
      QHash<int, Qt::ItemFlags> flags;        // column -> flags
    };

    void clear();
    void connectToServer();

    bool checkSyncBarrier(const Message &msg);

    Node* nodeForIndex(const QModelIndex &index) const;
    Node* nodeForIndex(const Protocol::ModelIndex &index) const;
    QModelIndex modelIndexForNode(GammaRay::RemoteModel::Node* node, int column) const;

    void requestRowColumnCount(const QModelIndex &index) const;
    void requestDataAndFlags(const QModelIndex &index) const;
    void requestHeaderData(Qt::Orientation orientation, int section) const;

    Node* m_root;

    mutable QHash<Qt::Orientation, QHash<int, QHash<int, QVariant> > > m_headers; // orientation -> section -> role -> data

    QString m_serverObject;
    Protocol::ObjectAddress m_myAddress;

    qint32 m_currentSyncBarrier, m_targetSyncBarrier;
};

}

#endif
