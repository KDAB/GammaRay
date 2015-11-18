/*
  remotemodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_REMOTEMODEL_H
#define GAMMARAY_REMOTEMODEL_H

#include "gammaray_client_export.h"
#include <common/objectmodel.h>
#include <common/protocol.h>

#include <QAbstractItemModel>
#include <QRegExp>
#include <QSet>
#include <QTimer>
#include <QVector>

namespace GammaRay {

class Message;

/** @internal Exported for unit test use only. */
class GAMMARAY_CLIENT_EXPORT RemoteModel : public QAbstractItemModel
{
  Q_OBJECT
  Q_PROPERTY(bool dynamicSortFilter READ proxyDynamicSortFilter WRITE setProxyDynamicSortFilter NOTIFY proxyDynamicSortFilterChanged)
  Q_PROPERTY(Qt::CaseSensitivity filterCaseSensitivity READ proxyFilterCaseSensitivity WRITE setProxyFilterCaseSensitivity NOTIFY proxyFilterCaseSensitivityChanged)
  Q_PROPERTY(int filterKeyColumn READ proxyFilterKeyColumn WRITE setProxyFilterKeyColumn NOTIFY proxyFilterKeyColumnChanged)
  Q_PROPERTY(QRegExp filterRegExp READ proxyFilterRegExp WRITE setProxyFilterRegExp NOTIFY proxyFilterRegExpChanged)

  public:
    explicit RemoteModel(const QString &serverObject, QObject *parent = 0);
    ~RemoteModel();

    bool isConnected() const;

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex& child) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) Q_DECL_OVERRIDE;

  public slots:
    void newMessage(const GammaRay::Message &msg);
    void serverRegistered(const QString &objectName, Protocol::ObjectAddress objectAddress);
    void serverUnregistered(const QString& objectName, Protocol::ObjectAddress objectAddress);

  public:
    enum Roles {
      LoadingState = ObjectModel::UserRole + 128 // TODO: Tidy up roles in general (and give loading state a proper id)
    };
    enum NodeState {
      NoState = 0,
      Empty = 1,
      Loading = 2,
      Outdated = 4
    };
    Q_DECLARE_FLAGS(NodeStates, NodeState)

  signals:
    void proxyDynamicSortFilterChanged();
    void proxyFilterCaseSensitivityChanged();
    void proxyFilterKeyColumnChanged();
    void proxyFilterRegExpChanged();

  private:
    struct Node { // represents one row
      Node() : parent(0), rowCount(-1), columnCount(-1) {}
      ~Node();
      Q_DISABLE_COPY(Node)
      // delete all cached children data, but assume row/column count on this level is still accurate
      void clearChildrenData();
      // forget everything we know about our children, including row/column counts
      void clearChildrenStructure();

      Node* parent;
      QVector<Node*> children;
      qint32 rowCount;
      qint32 columnCount;
      QHash<int, QHash<int, QVariant> > data; // column -> role -> data
      QHash<int, Qt::ItemFlags> flags;        // column -> flags
      QHash<int, NodeStates> state;           // column -> state (cache outdated, waiting for data, etc)
    };

    void clear();
    void connectToServer();

    bool checkSyncBarrier(const Message &msg);

    Node* nodeForIndex(const QModelIndex &index) const;
    Node* nodeForIndex(const Protocol::ModelIndex &index) const;
    QModelIndex modelIndexForNode(GammaRay::RemoteModel::Node* node, int column) const;

    /** Checks if @p ancestor is a (grand)parent of @p child. */
    bool isAncestor(Node *ancestor, Node *child) const;

    NodeStates stateForColumn(Node* node, int columnIndex) const;

    void requestRowColumnCount(const QModelIndex &index) const;
    void requestDataAndFlags(const QModelIndex &index) const;
    void requestHeaderData(Qt::Orientation orientation, int section) const;
    /// Reset the loading state for all rows at @p startRow or later.
    /// This is needed when rows have been added or removed before @p startRow, since
    /// pending replies might have a wrong index.
    void resetLoadingState(Node *node, int startRow) const;

    /// execute a insertRows() operation
    void doInsertRows(Node *parentNode, int first, int last);
    /// execute a removeRows() operation
    void doRemoveRows(Node *parentNode, int first, int last);
    /// execute a rowsMoved() operation
    void doMoveRows(Node *sourceParentNode, int sourceStart, int sourceEnd, Node* destParentNode, int destStart);

    // sort/filter proxy model settings
    bool proxyDynamicSortFilter() const;
    void setProxyDynamicSortFilter(bool dynamicSortFilter);
    Qt::CaseSensitivity proxyFilterCaseSensitivity() const;
    void setProxyFilterCaseSensitivity(Qt::CaseSensitivity caseSensitivity);
    int proxyFilterKeyColumn() const;
    void setProxyFilterKeyColumn(int column);
    QRegExp proxyFilterRegExp() const;
    void setProxyFilterRegExp(const QRegExp &regExp);

private slots:
    void doRequestDataAndFlags() const;

private:
    Node* m_root;

    mutable QHash<Qt::Orientation, QHash<int, QHash<int, QVariant> > > m_headers; // orientation -> section -> role -> data

    mutable QVector<Protocol::ModelIndex> m_pendingDataRequests;
    QTimer* m_pendingDataRequestsTimer;

    QString m_serverObject;
    Protocol::ObjectAddress m_myAddress;

    qint32 m_currentSyncBarrier, m_targetSyncBarrier;

    // default data() values for empty cells
    static QVariant s_emptyDisplayValue;
    static QVariant s_emptySizeHintValue;

    // proxy model properties
    bool m_proxyDynamicSortFilter;
    Qt::CaseSensitivity m_proxyCaseSensitivity;
    int m_proxyKeyColumn;
    QRegExp m_proxyFilterRegExp;

    // hooks for unit tests
    static void (*s_registerClientCallback)();
    void registerClient(const QString &serverObject);
    virtual void sendMessage(const Message &msg) const;
    friend class FakeRemoteModel;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(GammaRay::RemoteModel::NodeStates)
Q_DECLARE_METATYPE(GammaRay::RemoteModel::NodeStates)

#endif
