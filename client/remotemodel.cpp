/*
  remotemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "remotemodel.h"
#include "client.h"

#include <compat/qasconst.h>

#include <common/message.h>

#include <QApplication>
#include <QDataStream>
#include <QDebug>
#include <QStyle>
#include <QStyleOptionViewItem>

#include <algorithm>
#include <limits>

using namespace GammaRay;

void(*RemoteModel::s_registerClientCallback)() = nullptr;

RemoteModel::Node::~Node()
{
    qDeleteAll(children);
}

void RemoteModel::Node::clearChildrenData()
{
    foreach (auto child, children) {
        child->clearChildrenStructure();
        child->data.clear();
        child->flags.clear();
        child->state.clear();
    }
}

void RemoteModel::Node::clearChildrenStructure()
{
    qDeleteAll(children);
    children.clear();
    rowCount = -1;
    columnCount = -1;
}

void RemoteModel::Node::allocateColumns()
{
    if (hasColumnData() || !parent || parent->columnCount < 0)
        return;
    data.resize(parent->columnCount);
    flags.resize(parent->columnCount);
    flags.fill(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    state.resize(parent->columnCount, RemoteModelNodeState::Empty | RemoteModelNodeState::Outdated);
}

bool RemoteModel::Node::hasColumnData() const
{
    if (!parent)
        return false;
    Q_ASSERT(data.size() == flags.size());
    Q_ASSERT(data.size() == (int)state.size());
    Q_ASSERT(data.isEmpty() || data.size() == parent->columnCount || parent->columnCount < 0);

    return data.size() == parent->columnCount && parent->columnCount > 0;
}

QVariant RemoteModel::s_emptyDisplayValue;
QVariant RemoteModel::s_emptySizeHintValue;

RemoteModel::RemoteModel(const QString &serverObject, QObject *parent)
    : QAbstractItemModel(parent)
    , m_pendingRequestsTimer(new QTimer(this))
    , m_serverObject(serverObject)
    , m_myAddress(Protocol::InvalidObjectAddress)
    , m_currentSyncBarrier(0)
    , m_targetSyncBarrier(0)
    , m_proxyDynamicSortFilter(false)
    , m_proxyCaseSensitivity(Qt::CaseSensitive)
    , m_proxyKeyColumn(0)
{
    if (s_emptyDisplayValue.isNull()) {
        s_emptyDisplayValue = tr("Loading...");
        QStyleOptionViewItem opt;
        opt.features |= QStyleOptionViewItem::HasDisplay;
        opt.text = s_emptyDisplayValue.toString();
        s_emptySizeHintValue = QApplication::style()->sizeFromContents(QStyle::CT_ItemViewItem,
                                                                       &opt, QSize(), nullptr);
    }

    m_root = new Node;

    m_pendingRequestsTimer->setInterval(0);
    m_pendingRequestsTimer->setSingleShot(true);
    connect(m_pendingRequestsTimer, &QTimer::timeout, this, &RemoteModel::doRequests);

    registerClient(serverObject);
    connectToServer();
}

RemoteModel::~RemoteModel()
{
    delete m_root;
}

bool RemoteModel::isConnected() const
{
    return m_myAddress != Protocol::InvalidObjectAddress;
}

QModelIndex RemoteModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!isConnected() || row < 0 || column < 0)
        return {};

    Node *parentNode = nodeForIndex(parent);
    Q_ASSERT(parentNode->children.size() >= parentNode->rowCount);
    if (parentNode->rowCount == -1)
        requestRowColumnCount(parent); // trying to traverse into a branch we haven't loaded yet
    if (parentNode->rowCount <= row || parentNode->columnCount <= column)
        return QModelIndex();
    return createIndex(row, column, parentNode->children.at(row));
}

QModelIndex RemoteModel::parent(const QModelIndex &index) const
{
    Node *currentNode = nodeForIndex(index);
    Q_ASSERT(currentNode);
    if (currentNode == m_root || currentNode->parent == m_root)
        return {};
    Q_ASSERT(currentNode->parent && currentNode->parent->parent);
    Q_ASSERT(currentNode->parent->children.contains(currentNode));
    Q_ASSERT(currentNode->parent->parent->children.contains(currentNode->parent));

    return modelIndexForNode(currentNode->parent, 0);
}

int RemoteModel::rowCount(const QModelIndex &index) const
{
    if (!isConnected() || index.column() > 0)
        return 0;

    Node *node = nodeForIndex(index);
    Q_ASSERT(node);
    if (node->rowCount < 0) {
        if (node->columnCount < 0) // not yet requested vs. in the middle of insertion
            requestRowColumnCount(index);
    }
    return qMax(0, node->rowCount); // if requestRowColumnCount is synchronoous, ie. changes rowCount (as in simple unit test), returning 0 above would cause ModelTest to see inconsistent data
}

int RemoteModel::columnCount(const QModelIndex &index) const
{
    if (!isConnected())
        return 0;

    Node *node = nodeForIndex(index);
    Q_ASSERT(node);
    if (node->columnCount < 0) {
        requestRowColumnCount(index);
        return 0;
    }
    return node->columnCount;
}

QVariant RemoteModel::data(const QModelIndex &index, int role) const
{
    if (!isConnected() || !index.isValid())
        return QVariant();

    Node *node = nodeForIndex(index);
    Q_ASSERT(node);

    const auto state = stateForColumn(node, index.column());
    if (role == RemoteModelRole::LoadingState)
        return QVariant::fromValue(state);

    // for size hint we don't want to trigger loading, as that's largely used for item view layouting
    if (state & RemoteModelNodeState::Empty) {
        if (role == Qt::SizeHintRole)
            return s_emptySizeHintValue;
    }

    if ((state & RemoteModelNodeState::Outdated) && ((state & RemoteModelNodeState::Loading) == 0))
        requestDataAndFlags(index);

    if (state & RemoteModelNodeState::Empty) { // still waiting for data
        if (role == Qt::DisplayRole)
            return s_emptyDisplayValue;
        return QVariant();
    }

    // note .value returns good defaults otherwise
    Q_ASSERT(node->data.size() > index.column());
    return node->data.at(index.column()).value(role);
}

bool RemoteModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!isConnected())
        return false;

    Message msg(m_myAddress, Protocol::ModelSetDataRequest);
    msg << Protocol::fromQModelIndex(index) << role << value;
    sendMessage(msg);
    return false;
}

Qt::ItemFlags RemoteModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Node *node = nodeForIndex(index);
    Q_ASSERT(node);
    if (!node->hasColumnData())
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    Q_ASSERT(node->flags.size() > index.column());
    return node->flags.at(index.column());
}

QVariant RemoteModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (!isConnected() || section < 0)
        return QVariant();
    if (section >= (orientation == Qt::Horizontal ? m_root->columnCount : m_root->rowCount))
        return QVariant();

    auto &headers = orientation == Qt::Horizontal ? m_horizontalHeaders : m_verticalHeaders;
    if (headers.isEmpty()) { // allocate on demand
        const auto count = orientation == Qt::Horizontal ? m_root->columnCount : m_root->rowCount;
        if (count <= 0)
            return QVariant();
        headers.resize(count);
    }
    Q_ASSERT(headers.size() > section);
    if (headers.at(section).isEmpty())
        requestHeaderData(orientation, section);

    return headers.at(section).value(role);
}

void RemoteModel::sort(int column, Qt::SortOrder order)
{
    Message msg(m_myAddress, Protocol::ModelSortRequest);
    msg << (quint32)column << (quint32)order;
    sendMessage(msg);
}

void RemoteModel::newMessage(const GammaRay::Message &msg)
{
    if (!checkSyncBarrier(msg))
        return;

    switch (msg.type()) {
    case Protocol::ModelRowColumnCountReply:
    {
        quint32 size;
        msg >> size;
        Q_ASSERT(size > 0);

        for (quint32 i = 0; i < size; ++i) {
            // We now need to read the complete entries because of the break -> continue change
            Protocol::ModelIndex index;
            msg >> index;
            qint32 rowCount, columnCount;
            msg >> rowCount >> columnCount;

            Node *node = nodeForIndex(index);
            if (!node) {
                // This can happen e.g. when we called a blocking operation from the remote client
                // via the method invocation with a direct connection. Then when the blocking
                // operation creates e.g. a QObject it is directly added/removed to the ObjectTree
                // and we get signals for that. When we then though ask for column counts we will
                // only get responses once the blocking operation has finished, at which point
                // the object may already have been invalidated.
                continue;
            }
            // we get -1/-1 if we requested for an invalid index, e.g. due to not having processed
            // all structure changes yet. This will automatically trigger a retry.
            Q_ASSERT((rowCount >= 0 && columnCount >= 0) || (rowCount == -1 && columnCount == -1));
            if (node->rowCount >= 0 || node->columnCount >= 0) {
                // This can happen in similar racy conditions as below, when we request the row/col count
                // for two different Node* at the same index (one was deleted inbetween and then the other
                // was created). We ignore the new data as the node it is intended for will request it again
                // after processing all structure changes.
                continue;
            }

            if (node->rowCount == -1)
                continue; // we didn't ask for this, probably outdated response for a moved node

            Q_ASSERT(node->rowCount < -1 && node->columnCount == -1);

            const QModelIndex qmi = modelIndexForNode(node, 0);

            if (columnCount > 0) {
                beginInsertColumns(qmi, 0, columnCount - 1);
                node->columnCount = columnCount;
                endInsertColumns();
            } else {
                node->columnCount = columnCount;
            }

            if (rowCount > 0) {
                beginInsertRows(qmi, 0, rowCount - 1);
                node->children.reserve(rowCount);
                for (int i = 0; i < rowCount; ++i) {
                    auto *child = new Node;
                    child->parent = node;
                    node->children.push_back(child);
                }
                node->rowCount = rowCount;
                endInsertRows();
            } else {
                node->rowCount = rowCount;
            }
        }
        break;
    }

    case Protocol::ModelContentReply:
    {
        quint32 size;
        msg >> size;
        Q_ASSERT(size > 0);

        QHash<QModelIndex, QVector<QModelIndex> > dataChangedIndexes;
        for (quint32 i = 0; i < size; ++i) {
            Protocol::ModelIndex index;
            msg >> index;
            Node *node = nodeForIndex(index);
            const auto column = index.last().column;
            const auto state = node ? stateForColumn(node, column) : RemoteModelNodeState::NoState;
            typedef QHash<int, QVariant> ItemData;
            ItemData itemData;
            qint32 flags;
            msg >> itemData >> flags;
            if ((state & RemoteModelNodeState::Loading) == 0)
                continue; // we didn't ask for this, probably outdated response for a moved cell

            if (node) {
                node->allocateColumns();
                Q_ASSERT(node->data.size() > column);
                node->data[column] = itemData;
                node->flags[column] = static_cast<Qt::ItemFlags>(flags);
                node->state[column] = state & ~(RemoteModelNodeState::Loading | RemoteModelNodeState::Empty | RemoteModelNodeState::Outdated);

                if ((flags & Qt::ItemNeverHasChildren) && column == 0) {
                    node->rowCount = 0;
                    node->columnCount = node->data.size();
                }

                // group by parent, and emit dataChange for the bounding rect per hierarchy level
                // as an approximiation of perfect range batching
                const QModelIndex qmi = modelIndexForNode(node, column);
                dataChangedIndexes[qmi.parent()].push_back(qmi);
            }
        }

        for (auto it = dataChangedIndexes.constBegin(); it != dataChangedIndexes.constEnd(); ++it) {
            const auto &indexes = it.value();
            Q_ASSERT(!indexes.isEmpty());
            int r1 = std::numeric_limits<int>::max(), r2 = 0, c1 = std::numeric_limits<int>::max(),
                c2 = 0;
            for (const auto &index : indexes) {
                r1 = std::min(r1, index.row());
                r2 = std::max(r2, index.row());
                c1 = std::min(c1, index.column());
                c2 = std::max(c2, index.column());
            }
            const auto qmi = indexes.at(0);
            emit dataChanged(qmi.sibling(r1, c1), qmi.sibling(r2, c2));
        }
        break;
    }

    case Protocol::ModelHeaderReply:
    {
        qint8 orientation;
        qint32 section;
        QHash<qint32, QVariant> data;
        msg >> orientation >> section >> data;
        Q_ASSERT(orientation == Qt::Horizontal || orientation == Qt::Vertical);
        Q_ASSERT(section >= 0);
        auto &headers = orientation == Qt::Horizontal ? m_horizontalHeaders : m_verticalHeaders;
        if (headers.isEmpty())
            break;
        Q_ASSERT(headers.size() > section);
        headers[section] = data;
        if ((orientation == Qt::Horizontal && m_root->columnCount > section)
            || (orientation == Qt::Vertical && m_root->rowCount > section))
            emit headerDataChanged(static_cast<Qt::Orientation>(orientation), section, section);
        break;
    }

    case Protocol::ModelContentChanged:
    {
        Protocol::ModelIndex beginIndex, endIndex;
        QVector<int> roles;
        msg >> beginIndex >> endIndex >> roles;
        Node *node = nodeForIndex(beginIndex);
        if (!node || node == m_root)
            break;

        Q_ASSERT(beginIndex.last().row <= endIndex.last().row);
        Q_ASSERT(beginIndex.last().column <= endIndex.last().column);

        // mark content as outdated (will be refetched on next request)
        for (int row = beginIndex.last().row; row <= endIndex.last().row; ++row) {
            Node *currentRow = node->parent->children.at(row);
            if (!currentRow->hasColumnData())
                continue;
            for (int col = beginIndex.last().column; col <= endIndex.last().column; ++col) {
                const auto state = stateForColumn(currentRow, col);
                if ((state & RemoteModelNodeState::Outdated) == 0) {
                    Q_ASSERT((int)currentRow->state.size() > col);
                    currentRow->state[col] = state | RemoteModelNodeState::Outdated;
                }
            }
        }

        const QModelIndex qmiBegin = modelIndexForNode(node, beginIndex.last().column);
        const QModelIndex qmiEnd = qmiBegin.sibling(endIndex.last().row, endIndex.last().column);

        emit dataChanged(qmiBegin, qmiEnd, roles);
        break;
    }

    case Protocol::ModelHeaderChanged:
    {
        qint8 ori;
        int first, last;
        msg >> ori >> first >> last;
        const Qt::Orientation orientation = static_cast<Qt::Orientation>(ori);
        auto &headers = orientation == Qt::Horizontal ? m_horizontalHeaders : m_verticalHeaders;

        for (int i = first; i < last && i < headers.size(); ++i)
            headers[i].clear();

        emit headerDataChanged(orientation, first, last);
        break;
    }

    case Protocol::ModelRowsAdded:
    {
        Protocol::ModelIndex parentIndex;
        int first, last;
        msg >> parentIndex >> first >> last;
        Q_ASSERT(last >= first);

        Node *parentNode = nodeForIndex(parentIndex);
        if (!parentNode || parentNode->rowCount < 0)
            return; // we don't know the parent yet, so we don't care about changes to it either
        Q_ASSERT(first <= parentNode->rowCount);
        doInsertRows(parentNode, first, last);
        break;
    }

    case Protocol::ModelRowsRemoved:
    {
        Protocol::ModelIndex parentIndex;
        int first, last;
        msg >> parentIndex >> first >> last;
        Q_ASSERT(last >= first);

        Node *parentNode = nodeForIndex(parentIndex);
        if (!parentNode || parentNode->rowCount < 0)
            return; // we don't know the parent yet, so we don't care about changes to it either
        Q_ASSERT(first < parentNode->rowCount);
        doRemoveRows(parentNode, first, last);
        break;
    }

    case Protocol::ModelRowsMoved:
    {
        Protocol::ModelIndex sourceParentIndex, destParentIndex;
        int sourceFirst, sourceLast, destChild;
        msg >> sourceParentIndex >> sourceFirst >> sourceLast >> destParentIndex
        >> destChild;
        Q_ASSERT(sourceLast >= sourceFirst);

        Node *sourceParent = nodeForIndex(sourceParentIndex);
        Node *destParent = nodeForIndex(destParentIndex);

        const bool sourceKnown = sourceParent && sourceParent->rowCount >= 0;
        const bool destKnown = destParent && destParent->rowCount >= 0;

        // case 1: source and destination not locally cached -> nothing to do
        if (!sourceKnown && !destKnown)
            break;

        // case 2: only source is locally known -> remove
        if (sourceKnown && !destKnown) {
            doRemoveRows(sourceParent, sourceFirst, sourceLast);
            break;
        }

        // case 3: only destination is locally known -> added
        if (!sourceKnown && destKnown) {
            doInsertRows(destParent, destChild, destChild + sourceLast - sourceFirst);
            break;
        }

        // case 4: source and destination are locally known -> move
        if (sourceKnown && destKnown) {
            doMoveRows(sourceParent, sourceFirst, sourceLast, destParent, destChild);
            break;
        }

	break;
    }

    case Protocol::ModelColumnsAdded:
    {
        Protocol::ModelIndex parentIndex;
        int first, last;
        msg >> parentIndex >> first >> last;
        Q_ASSERT(last >= first);

        Node *parentNode = nodeForIndex(parentIndex);
        if (!parentNode || parentNode->rowCount < 0)
            return; // we don't know the parent yet, so we don't care about changes to it either

        doInsertColumns(parentNode, first, last);
        break;
    }

    case Protocol::ModelColumnsRemoved:
    {
        Protocol::ModelIndex parentIndex;
        int first, last;
        msg >> parentIndex >> first >> last;
        Q_ASSERT(last >= first);

        Node *parentNode = nodeForIndex(parentIndex);
        if (!parentNode || parentNode->rowCount < 0)
            return; // we don't know the parent yet, so we don't care about changes to it either

        doRemoveColumns(parentNode, first, last);
        break;
    }

    case Protocol::ModelColumnsMoved:
        // TODO
        qWarning() << Q_FUNC_INFO << "not implemented yet" << msg.type() << m_serverObject;
        clear();
        break;

    case Protocol::ModelLayoutChanged:
    {
        QVector<Protocol::ModelIndex> parents;
        quint32 hint;
        msg >> parents >> hint;

        if (parents.isEmpty()) { // everything changed (or Qt4)
            emit layoutAboutToBeChanged();
            foreach (const auto &persistentIndex, persistentIndexList())
                changePersistentIndex(persistentIndex, QModelIndex());
            if (hint == 0)
                m_root->clearChildrenStructure();
            else
                m_root->clearChildrenData();
            emit layoutChanged();
            break;
        }

        QVector<Node *> parentNodes;
        parentNodes.reserve(parents.size());
        for (const auto &p : qAsConst(parents)) {
            auto node = nodeForIndex(p);
            if (!node)
                continue;
            parentNodes.push_back(node);
        }
        if (parentNodes.isEmpty())
            break; // no currently loaded node changed, nothing to do

        emit layoutAboutToBeChanged(); // TODO Qt5 support with exact sub-trees
        foreach (const auto &persistentIndex, persistentIndexList()) {
            auto persistentNode = nodeForIndex(persistentIndex);
            Q_ASSERT(persistentNode);
            for (auto node : qAsConst(parentNodes)) {
                if (!isAncestor(node, persistentNode))
                    continue;
                changePersistentIndex(persistentIndex, QModelIndex());
                break;
            }
        }
        for (auto node : qAsConst(parentNodes)) {
            if (hint == 0)
                node->clearChildrenStructure();
            else
                node->clearChildrenData();
        }
        emit layoutChanged(); // TODO Qt5 support with exact sub-trees
        break;
    }

    case Protocol::ModelReset:
        clear();
        break;
    }
}

void RemoteModel::serverRegistered(const QString &objectName, Protocol::ObjectAddress objectAddress)
{
    if (m_serverObject == objectName) {
        m_myAddress = objectAddress;
        connectToServer();
    }
}

void RemoteModel::serverUnregistered(const QString &objectName,
                                     Protocol::ObjectAddress objectAddress)
{
    Q_UNUSED(objectName);
    if (m_myAddress == objectAddress) {
        m_myAddress = Protocol::InvalidObjectAddress;
        clear();
    }
}

RemoteModel::Node *RemoteModel::nodeForIndex(const QModelIndex &index) const
{
    if (!index.isValid())
        return m_root;
    return reinterpret_cast<Node *>(index.internalPointer());
}

RemoteModel::Node *RemoteModel::nodeForIndex(const Protocol::ModelIndex &index) const
{
    Node *node = m_root;
    for (auto i : index) {
        if (node->children.size() <= i.row)
            return nullptr;
        node = node->children.at(i.row);
        node->rowHint = i.row;
    }
    return node;
}

QModelIndex RemoteModel::modelIndexForNode(Node *node, int column) const
{
    Q_ASSERT(node);
    if (node == m_root)
        return {};

    int row = -1;
    if (node->rowHint > 0 && node->rowHint < node->parent->children.size()) {
        if (node->parent->children.at(node->rowHint) == node)
            row = node->rowHint;
    }
    if (row < 0) {
        row = node->parent->children.indexOf(node);
    }

    return createIndex(row, column, node);
}

bool RemoteModel::isAncestor(RemoteModel::Node *ancestor, RemoteModel::Node *child) const
{
    Q_ASSERT(ancestor);
    Q_ASSERT(child);
    Q_ASSERT(m_root);

    if (child == m_root)
        return false;
    Q_ASSERT(child->parent);
    if (child->parent == ancestor)
        return true;
    return isAncestor(ancestor, child->parent);
}

RemoteModelNodeState::NodeStates RemoteModel::stateForColumn(RemoteModel::Node *node, int columnIndex) const
{
    Q_ASSERT(node);
    if (!node->hasColumnData())
        return RemoteModelNodeState::Empty | RemoteModelNodeState::Outdated;
    Q_ASSERT((int)node->state.size() > columnIndex);
    return node->state[columnIndex];
}

void RemoteModel::requestRowColumnCount(const QModelIndex &index) const
{
    Node *node = nodeForIndex(index);
    Q_ASSERT(node);
    Q_ASSERT(node->rowCount < 0 && node->columnCount < 0);

    if (node->rowCount < -1) // already requesting
        return;
    node->rowCount = -2;

    auto &indexes = m_pendingRequests[RowColumnCount];
    indexes.push_back(Protocol::fromQModelIndex(index));
    if (indexes.size() > 100) {
        m_pendingRequestsTimer->stop();
        doRequests();
    } else {
        m_pendingRequestsTimer->start();
    }
}

void RemoteModel::requestDataAndFlags(const QModelIndex &index) const
{
    Node *node = nodeForIndex(index);
    Q_ASSERT(node);

    const auto state = stateForColumn(node, index.column());
    Q_ASSERT((state & RemoteModelNodeState::Loading) == 0);

    node->allocateColumns();
    Q_ASSERT((int)node->state.size() > index.column());
    node->state[index.column()] = state | RemoteModelNodeState::Loading; // mark pending request

    auto &indexes = m_pendingRequests[DataAndFlags];
    indexes.push_back(Protocol::fromQModelIndex(index));
    if (indexes.size() > 100) {
        m_pendingRequestsTimer->stop();
        doRequests();
    } else {
        m_pendingRequestsTimer->start();
    }
}

void RemoteModel::doRequests() const
{
    QMutableMapIterator<RequestType, QVector<Protocol::ModelIndex>> it(m_pendingRequests);

    while (it.hasNext()) {
        it.next();

        Q_ASSERT(!it.value().isEmpty());
        const auto &indexes = it.value();

        switch (it.key()) {
        case RowColumnCount: {
            Message msg(m_myAddress, Protocol::ModelRowColumnCountRequest);
            msg << quint32(indexes.size());
            for (const auto &index : indexes)
                msg << index;
            sendMessage(msg);
            break;
        }

        case DataAndFlags: {
            Message msg(m_myAddress, Protocol::ModelContentRequest);
            msg << quint32(indexes.size());
            for (const auto &index : indexes)
                msg << index;
            sendMessage(msg);
            break;
        }
        }

        it.remove();
    }
}

void RemoteModel::requestHeaderData(Qt::Orientation orientation, int section) const
{
    Q_ASSERT(section >= 0);
    auto &headers = orientation == Qt::Horizontal ? m_horizontalHeaders : m_verticalHeaders;
    Q_ASSERT(!headers.isEmpty());
    Q_ASSERT(headers.at(section).isEmpty());
    headers[section][Qt::DisplayRole] = s_emptyDisplayValue;

    Message msg(m_myAddress, Protocol::ModelHeaderRequest);
    msg << qint8(orientation) << qint32(section);
    sendMessage(msg);
}

void RemoteModel::clear()
{
    beginResetModel();

    if (isConnected()) {
        Message msg(m_myAddress, Protocol::ModelSyncBarrier);
        msg << ++m_targetSyncBarrier;
        sendMessage(msg);
    }

    delete m_root;
    m_root = new Node;
    m_horizontalHeaders.clear();
    m_verticalHeaders.clear();
    endResetModel();
}

void RemoteModel::connectToServer()
{
    if (m_myAddress == Protocol::InvalidObjectAddress)
        return;

    beginResetModel();
    Client::instance()->registerObject(m_serverObject, this);
    Client::instance()->registerMessageHandler(m_myAddress, this, "newMessage");
    endResetModel();
}

bool RemoteModel::checkSyncBarrier(const Message &msg)
{
    if (msg.type() == Protocol::ModelSyncBarrier)
        msg >> m_currentSyncBarrier;

    return m_currentSyncBarrier == m_targetSyncBarrier;
}

void RemoteModel::resetLoadingState(RemoteModel::Node *node, int startRow) const
{
    if (node->rowCount < 0) {
        node->rowCount = -1; // reset row count loading state
        return;
    }

    Q_ASSERT(node->children.size() == node->rowCount);
    for (int row = startRow; row < node->rowCount; ++row) {
        Node *child = node->children.at(row);
        for (auto it = child->state.begin(); it != child->state.end(); ++it) {
            if ((*it) & RemoteModelNodeState::Loading)
                (*it) = (*it) & ~RemoteModelNodeState::Loading;
        }
        resetLoadingState(child, 0);
    }
}

void RemoteModel::doInsertRows(RemoteModel::Node *parentNode, int first, int last)
{
    Q_ASSERT(parentNode->rowCount == parentNode->children.size());

    const QModelIndex qmiParent = modelIndexForNode(parentNode, 0);
    beginInsertRows(qmiParent, first, last);

    // if necessary, update vertical headers
    if (parentNode == m_root && !m_verticalHeaders.isEmpty())
        m_verticalHeaders.insert(first, last - first + 1, QHash<int, QVariant>());

    // allocate rows in the right spot
    parentNode->children.insert(first, last - first + 1, nullptr);

    // create nodes for the new rows
    for (int i = first; i <= last; ++i) {
        auto *child = new Node;
        child->parent = parentNode;
        parentNode->children[i] = child;
    }

    // adjust row count
    parentNode->rowCount += last - first + 1;
    Q_ASSERT(parentNode->rowCount == parentNode->children.size());

    endInsertRows();
    resetLoadingState(parentNode, last);
}

void RemoteModel::doRemoveRows(RemoteModel::Node *parentNode, int first, int last)
{
    Q_ASSERT(parentNode->rowCount == parentNode->children.size());

    const QModelIndex qmiParent = modelIndexForNode(parentNode, 0);
    beginRemoveRows(qmiParent, first, last);

    // if necessary update vertical headers
    if (parentNode == m_root && !m_verticalHeaders.isEmpty())
        m_verticalHeaders.remove(first, last - first + 1);

    // delete nodes
    for (int i = first; i <= last; ++i)
        delete parentNode->children.at(i);
    parentNode->children.remove(first, last - first + 1);

    // adjust row count
    parentNode->rowCount -= last - first + 1;
    Q_ASSERT(parentNode->rowCount == parentNode->children.size());

    endRemoveRows();
    resetLoadingState(parentNode, first);
}

void RemoteModel::doMoveRows(RemoteModel::Node *sourceParentNode, int sourceStart, int sourceEnd,
                             RemoteModel::Node *destParentNode, int destStart)
{
    Q_ASSERT(sourceParentNode->rowCount == sourceParentNode->children.size());
    Q_ASSERT(destParentNode->rowCount == destParentNode->children.size());
    Q_ASSERT(sourceEnd >= sourceStart);
    Q_ASSERT(sourceParentNode->rowCount > sourceEnd);

    const int destEnd = destStart + sourceEnd - sourceStart;
    const int amount = sourceEnd - sourceStart + 1;

    const QModelIndex qmiSourceParent = modelIndexForNode(sourceParentNode, 0);
    const QModelIndex qmiDestParent = modelIndexForNode(destParentNode, 0);
    beginMoveRows(qmiSourceParent, sourceStart, sourceEnd, qmiDestParent, destStart);

    // make room in the destination
    destParentNode->children.insert(destStart, amount, nullptr);

    // move nodes
    for (int i = 0; i < amount; ++i) {
        Node *node = sourceParentNode->children.at(sourceStart + i);
        node->parent = destParentNode;
        destParentNode->children[destStart + i] = node;
    }

    // shrink source
    sourceParentNode->children.remove(sourceStart, amount);

    // adjust row count
    sourceParentNode->rowCount -= amount;
    destParentNode->rowCount += amount;
    Q_ASSERT(sourceParentNode->rowCount == sourceParentNode->children.size());
    Q_ASSERT(destParentNode->rowCount == destParentNode->children.size());

    // FIXME: we could insert/remove just the affected rows, but this is currently not hit anyway
    // update vertical headers if we move to/from top-level
    if (sourceParentNode == m_root || destParentNode == m_root)
        m_verticalHeaders.clear();

    endMoveRows();
    resetLoadingState(sourceParentNode, sourceStart);
    resetLoadingState(destParentNode, destEnd);
}

void RemoteModel::doInsertColumns(RemoteModel::Node *parentNode, int first, int last)
{
    const auto newColCount = last - first + 1;
    const QModelIndex qmiParent = modelIndexForNode(parentNode, 0);
    beginInsertColumns(qmiParent, first, last);

    // if necessary, update horizontal headers
    if (parentNode == m_root && !m_horizontalHeaders.isEmpty())
        m_horizontalHeaders.insert(first, newColCount, QHash<int, QVariant>());

    // adjust column data in all child nodes, if available
    for (auto node : qAsConst(parentNode->children)) {
        if (!node->hasColumnData())
            continue;

        // allocate new columns
        node->data.insert(first, newColCount, QHash<int, QVariant>());
        node->flags.insert(first, newColCount, Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        node->state.insert(node->state.begin() + first, newColCount, RemoteModelNodeState::Empty | RemoteModelNodeState::Outdated);
    }

    // adjust column count
    parentNode->columnCount += newColCount;

    endInsertColumns();
}

void RemoteModel::doRemoveColumns(RemoteModel::Node *parentNode, int first, int last)
{
    const auto delColCount = last - first + 1;
    const QModelIndex qmiParent = modelIndexForNode(parentNode, 0);
    beginRemoveColumns(qmiParent, first, last);

    // if necessary update vertical headers
    if (parentNode == m_root && !m_horizontalHeaders.isEmpty())
        m_horizontalHeaders.remove(first, delColCount);

    // adjust column data in all child nodes, if available
    for (auto node : qAsConst(parentNode->children)) {
        if (!node->hasColumnData())
            continue;
        node->data.remove(first, delColCount);
        node->flags.remove(first, delColCount);
        node->state.erase(node->state.begin() + first, node->state.begin() + last);
    }

    // adjust column count
    parentNode->columnCount -= delColCount;

    endRemoveColumns();
}

void RemoteModel::registerClient(const QString &serverObject)
{
    if (Q_UNLIKELY(s_registerClientCallback)) { // called from ctor, so we can't use virtuals here
        s_registerClientCallback();
        return;
    }
    m_myAddress = Endpoint::instance()->objectAddress(serverObject);
    connect(Endpoint::instance(), &Endpoint::objectRegistered,
            this, &RemoteModel::serverRegistered);
    connect(Endpoint::instance(), &Endpoint::objectUnregistered,
            this, &RemoteModel::serverUnregistered);
}

void RemoteModel::sendMessage(const Message &msg) const
{
    Endpoint::send(msg);
}

bool RemoteModel::proxyDynamicSortFilter() const
{
    return m_proxyDynamicSortFilter;
}

void RemoteModel::setProxyDynamicSortFilter(bool dynamicSortFilter)
{
    if (m_proxyDynamicSortFilter == dynamicSortFilter)
        return;
    m_proxyDynamicSortFilter = dynamicSortFilter;
    emit proxyDynamicSortFilterChanged();
}

Qt::CaseSensitivity RemoteModel::proxyFilterCaseSensitivity() const
{
    return m_proxyCaseSensitivity;
}

void RemoteModel::setProxyFilterCaseSensitivity(Qt::CaseSensitivity caseSensitivity)
{
    if (m_proxyCaseSensitivity == caseSensitivity)
        return;
    m_proxyCaseSensitivity = caseSensitivity;
    emit proxyFilterCaseSensitivityChanged();
}

int RemoteModel::proxyFilterKeyColumn() const
{
    return m_proxyKeyColumn;
}

void RemoteModel::setProxyFilterKeyColumn(int column)
{
    if (m_proxyKeyColumn == column)
        return;
    m_proxyKeyColumn = column;
    emit proxyFilterKeyColumnChanged();
}

RemoteModel::RegExpT RemoteModel::proxyFilterRegExp() const
{
    return m_proxyFilterRegExp;
}

void RemoteModel::setProxyFilterRegExp(const RegExpT &regExp)
{
    if (m_proxyFilterRegExp == regExp)
        return;
    m_proxyFilterRegExp = regExp;
    emit proxyFilterRegExpChanged();
}
