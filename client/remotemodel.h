/*
  remotemodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_REMOTEMODEL_H
#define GAMMARAY_REMOTEMODEL_H

#include "gammaray_client_export.h"
#include <common/objectmodel.h>
#include <common/protocol.h>
#include <common/remotemodelroles.h>

#include <QAbstractItemModel>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QRegExp>
#else
#include <QRegularExpression>
#endif
#include <QSet>
#include <QTimer>
#include <QVector>

namespace GammaRay {
class Message;

/** @internal Exported for unit test use only. */
class GAMMARAY_CLIENT_EXPORT RemoteModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(
        bool dynamicSortFilter READ proxyDynamicSortFilter WRITE setProxyDynamicSortFilter NOTIFY proxyDynamicSortFilterChanged)
    Q_PROPERTY(
        Qt::CaseSensitivity filterCaseSensitivity READ proxyFilterCaseSensitivity WRITE setProxyFilterCaseSensitivity NOTIFY proxyFilterCaseSensitivityChanged)
    Q_PROPERTY(
        int filterKeyColumn READ proxyFilterKeyColumn WRITE setProxyFilterKeyColumn NOTIFY proxyFilterKeyColumnChanged)
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Q_PROPERTY(
        QRegExp filterRegExp READ proxyFilterRegExp WRITE setProxyFilterRegExp NOTIFY proxyFilterRegExpChanged)
    using RegExpT = QRegExp;
#else
    Q_PROPERTY(
        QRegularExpression filterRegularExpression READ proxyFilterRegExp WRITE setProxyFilterRegExp NOTIFY proxyFilterRegExpChanged)
    using RegExpT = QRegularExpression;
#endif

public:
    explicit RemoteModel(const QString &serverObject, QObject *parent = nullptr);
    ~RemoteModel() override;

    bool isConnected() const;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &index = QModelIndex()) const override;
    int columnCount(const QModelIndex &index = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

public slots:
    void newMessage(const GammaRay::Message &msg);
    void serverRegistered(const QString &objectName, Protocol::ObjectAddress objectAddress);
    void serverUnregistered(const QString &objectName, Protocol::ObjectAddress objectAddress);

signals:
    void proxyDynamicSortFilterChanged();
    void proxyFilterCaseSensitivityChanged();
    void proxyFilterKeyColumnChanged();
    void proxyFilterRegExpChanged();

    void declarationCreationLocationsReceived(const QVariant &v, const QVariant &v2);

private:
    struct Node
    { // represents one row
        Node() = default;
        ~Node();
        Q_DISABLE_COPY(Node)
        // delete all cached children data, but assume row/column count on this level is still accurate
        void clearChildrenData();
        // forget everything we know about our children, including row/column counts
        void clearChildrenStructure();

        // resize the initialize the column vectors
        void allocateColumns();
        // returns whether columns are allocated
        bool hasColumnData() const;

        Node *parent = nullptr;
        QVector<Node *> children;
        qint32 rowCount = -1;
        qint32 columnCount = -1;
        QVector<QHash<int, QVariant>> data; // column -> role -> data
        QVector<Qt::ItemFlags> flags; // column -> flags
        std::vector<RemoteModelNodeState::NodeStates> state; // column -> state (cache outdated, waiting for data, etc)

        int rowHint = -1; // for internal use by modelIndexForNode
    };

    void clear();
    void connectToServer();

    bool checkSyncBarrier(const Message &msg);

    Node *nodeForIndex(const QModelIndex &index) const;
    Node *nodeForIndex(const Protocol::ModelIndex &index) const;
    QModelIndex modelIndexForNode(GammaRay::RemoteModel::Node *node, int column) const;

    /** Checks if @p ancestor is a (grand)parent of @p child. */
    bool isAncestor(Node *ancestor, Node *child) const;

    static RemoteModelNodeState::NodeStates stateForColumn(Node *node, int columnIndex);

    void requestRowColumnCount(const QModelIndex &index) const;
    void requestDataAndFlags(const QModelIndex &index) const;
    void requestHeaderData(Qt::Orientation orientation, int section) const;
    /// Reset the loading state for all rows at @p startRow or later.
    /// This is needed when rows have been added or removed before @p startRow, since
    /// pending replies might have a wrong index.
    void resetLoadingState(Node *node, int startRow) const;

    QVariant requestCreationDeclarationLocation(const QModelIndex &, int role) const;

    /// execute a insertRows() operation
    void doInsertRows(Node *parentNode, int first, int last);
    /// execute a removeRows() operation
    void doRemoveRows(Node *parentNode, int first, int last);
    /// execute a rowsMoved() operation
    void doMoveRows(Node *sourceParentNode, int sourceStart, int sourceEnd, Node *destParentNode,
                    int destStart);

    /// execute a insertColumns() operation
    void doInsertColumns(Node *parentNode, int first, int last);
    /// execute a removeColumns() operation
    void doRemoveColumns(Node *parentNode, int first, int last);

    // sort/filter proxy model settings
    bool proxyDynamicSortFilter() const;
    void setProxyDynamicSortFilter(bool dynamicSortFilter);
    Qt::CaseSensitivity proxyFilterCaseSensitivity() const;
    void setProxyFilterCaseSensitivity(Qt::CaseSensitivity caseSensitivity);
    int proxyFilterKeyColumn() const;
    void setProxyFilterKeyColumn(int column);
    RegExpT proxyFilterRegExp() const;
    void setProxyFilterRegExp(const RegExpT &regExp);

private slots:
    void doRequests() const;

private:
    Node *m_root;

    mutable QVector<QHash<int, QVariant>> m_horizontalHeaders; // section -> role -> data
    mutable QVector<QHash<int, QVariant>> m_verticalHeaders; // section -> role -> data

    enum RequestType
    {
        RowColumnCount,
        DataAndFlags
    };

    mutable QMap<RequestType, QVector<Protocol::ModelIndex>> m_pendingRequests;
    QTimer *m_pendingRequestsTimer;

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
    RegExpT m_proxyFilterRegExp;

    // hooks for unit tests
    static void (*s_registerClientCallback)();
    void registerClient(const QString &serverObject);
    virtual void sendMessage(const Message &msg) const;
    friend class FakeRemoteModel;
};
}

#endif
