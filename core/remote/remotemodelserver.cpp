/*
  remotemodelserver.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "remotemodelserver.h"
#include "server.h"
#include <core/probeguard.h>
#include <common/protocol.h>
#include <common/message.h>
#include <common/modelevent.h>
#include <common/sourcelocation.h>

#include <compat/qasconst.h>

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QDataStream>
#include <QDebug>
#include <QBuffer>
#include <QIcon>

#include <iostream>

using namespace GammaRay;
using namespace std;

void(*RemoteModelServer::s_registerServerCallback)() = nullptr;

RemoteModelServer::RemoteModelServer(const QString &objectName, QObject *parent)
    : QObject(parent)
    , m_model(nullptr)
    , m_dummyBuffer(new QBuffer(&m_dummyData, this))
    , m_monitored(false)
{
    setObjectName(objectName);
    m_dummyBuffer->open(QIODevice::WriteOnly);
    registerServer();
}

RemoteModelServer::~RemoteModelServer() = default;

QAbstractItemModel *RemoteModelServer::model() const
{
    return m_model;
}

void RemoteModelServer::setModel(QAbstractItemModel *model)
{
    if (model == m_model)
        return;

    if (m_model)
        disconnectModel();

    m_model = model;
    if (m_model && m_monitored)
        connectModel();

    if (m_monitored)
        modelReset();
}

void RemoteModelServer::connectModel()
{
    Q_ASSERT(m_model);
    Model::used(m_model);

    connect(m_model.data(), &QAbstractItemModel::headerDataChanged,
            this, &RemoteModelServer::headerDataChanged);
    connect(m_model.data(), &QAbstractItemModel::rowsInserted,
            this, &RemoteModelServer::rowsInserted);
    connect(m_model.data(), &QAbstractItemModel::rowsAboutToBeMoved,
            this, &RemoteModelServer::rowsAboutToBeMoved);
    connect(m_model.data(), &QAbstractItemModel::rowsMoved,
            this, &RemoteModelServer::rowsMoved);
    connect(m_model.data(), &QAbstractItemModel::rowsRemoved,
            this, &RemoteModelServer::rowsRemoved);
    connect(m_model.data(), &QAbstractItemModel::columnsInserted,
            this, &RemoteModelServer::columnsInserted);
    connect(m_model.data(), &QAbstractItemModel::columnsMoved,
            this, &RemoteModelServer::columnsMoved);
    connect(m_model.data(), &QAbstractItemModel::columnsRemoved,
            this, &RemoteModelServer::columnsRemoved);
    connect(m_model.data(), &QAbstractItemModel::dataChanged,
            this, &RemoteModelServer::dataChanged);
    connect(m_model.data(),
            &QAbstractItemModel::layoutChanged,
            this,
            &RemoteModelServer::layoutChanged);
    connect(m_model.data(), &QAbstractItemModel::modelReset, this, &RemoteModelServer::modelReset);
    connect(m_model.data(), &QObject::destroyed, this, &RemoteModelServer::modelDeleted);
}

void RemoteModelServer::disconnectModel()
{
    Q_ASSERT(m_model);
    Model::unused(m_model);

    disconnect(m_model.data(), &QAbstractItemModel::headerDataChanged,
               this, &RemoteModelServer::headerDataChanged);
    disconnect(m_model.data(), &QAbstractItemModel::rowsInserted,
               this, &RemoteModelServer::rowsInserted);
    disconnect(m_model.data(), &QAbstractItemModel::rowsAboutToBeMoved,
               this, &RemoteModelServer::rowsAboutToBeMoved);
    disconnect(m_model.data(), &QAbstractItemModel::rowsMoved,
               this, &RemoteModelServer::rowsMoved);
    disconnect(m_model.data(), &QAbstractItemModel::rowsRemoved,
               this, &RemoteModelServer::rowsRemoved);
    disconnect(m_model.data(), &QAbstractItemModel::columnsInserted,
               this, &RemoteModelServer::columnsInserted);
    disconnect(m_model.data(), &QAbstractItemModel::columnsMoved,
               this, &RemoteModelServer::columnsMoved);
    disconnect(m_model.data(), &QAbstractItemModel::columnsRemoved,
               this, &RemoteModelServer::columnsRemoved);
    disconnect(m_model.data(), &QAbstractItemModel::dataChanged,
               this, &RemoteModelServer::dataChanged);
    disconnect(m_model.data(), &QAbstractItemModel::layoutChanged,
               this, &RemoteModelServer::layoutChanged);
    disconnect(m_model.data(), &QAbstractItemModel::modelReset, this, &RemoteModelServer::modelReset);
    disconnect(m_model.data(), &QObject::destroyed, this, &RemoteModelServer::modelDeleted);
}

void RemoteModelServer::newRequest(const GammaRay::Message &msg)
{
    if (!m_model && msg.type() != Protocol::ModelSyncBarrier)
        return;

    ProbeGuard g;
    switch (msg.type()) {
    case Protocol::ModelRowColumnCountRequest:
    {
        quint32 size;
        msg >> size;
        Q_ASSERT(size > 0);

        Message reply(m_myAddress, Protocol::ModelRowColumnCountReply);
        reply << size;
        for (quint32 i = 0; i < size; ++i) {
            Protocol::ModelIndex index;
            msg >> index;
            const QModelIndex qmIndex = Protocol::toQModelIndex(m_model, index);

            qint32 rowCount = -1, columnCount = -1;
            if (index.isEmpty() || qmIndex.isValid()) {
                rowCount = m_model->rowCount(qmIndex);
                columnCount = m_model->columnCount(qmIndex);
            }

            reply << index << rowCount << columnCount;
        }
        sendMessage(reply);
        break;
    }

    case Protocol::ModelContentRequest:
    {
        quint32 size;
        msg >> size;
        Q_ASSERT(size > 0);

        QVector<QModelIndex> indexes;
        indexes.reserve(size);
        for (quint32 i = 0; i < size; ++i) {
            Protocol::ModelIndex index;
            msg >> index;
            const QModelIndex qmIndex = Protocol::toQModelIndex(m_model, index);
            if (!qmIndex.isValid())
                continue;
            indexes.push_back(qmIndex);
        }
        if (indexes.isEmpty())
            break;

        Message msg(m_myAddress, Protocol::ModelContentReply);
        msg << quint32(indexes.size());
        for (const auto &qmIndex : qAsConst(indexes))
            msg << Protocol::fromQModelIndex(qmIndex)
                          << filterItemData(m_model->itemData(qmIndex))
                          << qint32(m_model->flags(qmIndex));

        sendMessage(msg);
        break;
    }

    case Protocol::ModelHeaderRequest:
    {
        qint8 orientation;
        qint32 section;
        msg >> orientation >> section;
        Q_ASSERT(orientation == Qt::Horizontal || orientation == Qt::Vertical);
        Q_ASSERT(section >= 0);

        QHash<qint32, QVariant> data;
        // TODO: add all roles
        data.insert(Qt::DisplayRole,
                    m_model->headerData(section, static_cast<Qt::Orientation>(orientation),
                                        Qt::DisplayRole));
        data.insert(Qt::ToolTipRole,
                    m_model->headerData(section, static_cast<Qt::Orientation>(orientation),
                                        Qt::ToolTipRole));

        Message msg(m_myAddress, Protocol::ModelHeaderReply);
        msg << orientation << section << data;
        sendMessage(msg);
        break;
    }

    case Protocol::ModelSetDataRequest:
    {
        Protocol::ModelIndex index;
        int role;
        QVariant value;
        msg >> index >> role >> value;

        m_model->setData(Protocol::toQModelIndex(m_model, index), value, role);
        break;
    }

    case Protocol::ModelSortRequest:
    {
        quint32 column, order;
        msg >> column >> order;
        m_model->sort(column, (Qt::SortOrder)order);
        break;
    }

    case Protocol::ModelSyncBarrier:
    {
        qint32 barrierId;
        msg >> barrierId;
        Message reply(m_myAddress, Protocol::ModelSyncBarrier);
        reply << barrierId;
        sendMessage(reply);
        break;
    }
    }
}

QMap<int, QVariant> RemoteModelServer::filterItemData(QMap<int, QVariant> &&itemData) const
{
    for (auto it = itemData.begin(); it != itemData.end();) {
        if (!it.value().isValid()) {
            it = itemData.erase(it);
        } else if (it.value().userType() == qMetaTypeId<QIcon>()) {
            // see also: https://bugreports.qt-project.org/browse/QTBUG-33321
            const QIcon icon = it.value().value<QIcon>();
            ///TODO: what size to use? icon.availableSizes is empty...
            if (!icon.isNull())
                it.value() = icon.pixmap(QSize(16, 16));
            ++it;
        } else if (canSerialize(it.value())) {
            ++it;
        } else {
// qWarning() << "Cannot serialize QVariant of type" << it.value().typeName();
            it = itemData.erase(it);
        }
    }
    return std::move(itemData);
}

bool RemoteModelServer::canSerialize(const QVariant &value) const
{
    if (qstrcmp(value.typeName(), "QJSValue") == 0 || qstrcmp(value.typeName(), "QJsonObject") == 0 || qstrcmp(value.typeName(), "QJsonValue") == 0 || qstrcmp(value.typeName(), "QJsonArray") == 0) {
        // QJSValue tries to serialize nested elements and asserts if that fails
        // too bad it can contain QObject* as nested element, which obviously can't be serialized...
        // QJsonObject serialization fails due to QTBUG-73437
        return false;
    }

    // recurse into containers
    if (value.canConvert<QVariantList>()) {
        QSequentialIterable it = value.value<QSequentialIterable>();
        for (const QVariant &v : it) {
            if (!canSerialize(v))
                return false;
        }
        // note: do not return true here, the fact we can write every single element
        // does not mean we can write the entire thing, or vice vesa...
    } else if (value.canConvert<QVariantMap>()) {
        auto iterable = value.value<QAssociativeIterable>();
        for (auto it = iterable.begin(); it != iterable.end(); ++it) {
            if (!canSerialize(it.value()) || !canSerialize(it.key()))
                return false;
        }
        // see above
    }

    // whitelist a few expensive to encode types we know we can serialize
    if (value.userType() == qMetaTypeId<QUrl>() || value.userType() == qMetaTypeId<GammaRay::SourceLocation>())
        return true;

    // ugly, but there doesn't seem to be a better way atm to find out without trying
    m_dummyBuffer->seek(0);
    QDataStream stream(m_dummyBuffer);
    return QMetaType::save(stream, value.userType(), value.constData());
}

void RemoteModelServer::modelMonitored(bool monitored)
{
    if (m_monitored == monitored)
        return;
    m_monitored = monitored;
    if (m_model) {
        if (m_monitored)
            connectModel();
        else
            disconnectModel();
    }
}

void RemoteModelServer::dataChanged(const QModelIndex &begin, const QModelIndex &end,
                                    const QVector<int> &roles)
{
    if (!isConnected())
        return;
    Message msg(m_myAddress, Protocol::ModelContentChanged);
    msg << Protocol::fromQModelIndex(begin) << Protocol::fromQModelIndex(end) << roles;
    sendMessage(msg);
}

void RemoteModelServer::headerDataChanged(Qt::Orientation orientation, int first, int last)
{
    if (!isConnected())
        return;
    Message msg(m_myAddress, Protocol::ModelHeaderChanged);
    msg <<  qint8(orientation) << first << last;
    sendMessage(msg);
}

void RemoteModelServer::rowsInserted(const QModelIndex &parent, int start, int end)
{
    sendAddRemoveMessage(Protocol::ModelRowsAdded, parent, start, end);
}

void RemoteModelServer::rowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart,
                                           int sourceEnd, const QModelIndex &destinationParent,
                                           int destinationRow)
{
    Q_UNUSED(sourceStart);
    Q_UNUSED(sourceEnd);
    Q_UNUSED(destinationRow);
    m_preOpIndexes.push_back(Protocol::fromQModelIndex(sourceParent));
    m_preOpIndexes.push_back(Protocol::fromQModelIndex(destinationParent));
}

void RemoteModelServer::rowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd,
                                  const QModelIndex &destinationParent, int destinationRow)
{
    Q_UNUSED(sourceParent);
    Q_UNUSED(destinationParent);
    Q_ASSERT(m_preOpIndexes.size() >= 2);
    const auto destParentIdx = m_preOpIndexes.takeLast();
    const auto sourceParentIdx = m_preOpIndexes.takeLast();
    sendMoveMessage(Protocol::ModelRowsMoved, sourceParentIdx, sourceStart, sourceEnd,
                    destParentIdx, destinationRow);
}

void RemoteModelServer::rowsRemoved(const QModelIndex &parent, int start, int end)
{
    sendAddRemoveMessage(Protocol::ModelRowsRemoved, parent, start, end);
}

void RemoteModelServer::columnsInserted(const QModelIndex &parent, int start, int end)
{
    sendAddRemoveMessage(Protocol::ModelColumnsAdded, parent, start, end);
}

void RemoteModelServer::columnsMoved(const QModelIndex &sourceParent, int sourceStart,
                                     int sourceEnd, const QModelIndex &destinationParent,
                                     int destinationColumn)
{
    sendMoveMessage(Protocol::ModelColumnsMoved,
                    Protocol::fromQModelIndex(sourceParent), sourceStart, sourceEnd,
                    Protocol::fromQModelIndex(destinationParent), destinationColumn);
}

void RemoteModelServer::columnsRemoved(const QModelIndex &parent, int start, int end)
{
    sendAddRemoveMessage(Protocol::ModelColumnsRemoved, parent, start, end);
}


void RemoteModelServer::layoutChanged(const QList<QPersistentModelIndex> &parents,
                                      QAbstractItemModel::LayoutChangeHint hint)
{
    QVector<Protocol::ModelIndex> indexes;
    indexes.reserve(parents.size());
    for (const auto &index : parents)
        indexes.push_back(Protocol::fromQModelIndex(index));
    sendLayoutChanged(indexes, hint);
}


void RemoteModelServer::sendLayoutChanged(const QVector< Protocol::ModelIndex > &parents,
                                          quint32 hint)
{
    if (!isConnected())
        return;
    Message msg(m_myAddress, Protocol::ModelLayoutChanged);
    msg << parents << hint;
    sendMessage(msg);
}

void RemoteModelServer::modelReset()
{
    if (!isConnected())
        return;
    sendMessage(Message(m_myAddress, Protocol::ModelReset));
}

void RemoteModelServer::sendAddRemoveMessage(Protocol::MessageType type, const QModelIndex &parent,
                                             int start, int end)
{
    if (!isConnected())
        return;
    Message msg(m_myAddress, type);
    msg << Protocol::fromQModelIndex(parent) << start << end;
    sendMessage(msg);
}

void RemoteModelServer::sendMoveMessage(Protocol::MessageType type,
                                        const Protocol::ModelIndex &sourceParent, int sourceStart,
                                        int sourceEnd,
                                        const Protocol::ModelIndex &destinationParent,
                                        int destinationIndex)
{
    if (!isConnected())
        return;
    Message msg(m_myAddress, type);
    msg << sourceParent << qint32(sourceStart) << qint32(sourceEnd)
                  << destinationParent << qint32(destinationIndex);
    sendMessage(msg);
}

void RemoteModelServer::modelDeleted()
{
    m_model = nullptr;
    if (m_monitored)
        modelReset();
}

void RemoteModelServer::registerServer()
{
    if (Q_UNLIKELY(s_registerServerCallback)) { // called from the ctor, so we can't rely on virtuals
        s_registerServerCallback();
        return;
    }
    m_myAddress = Server::instance()->registerObject(objectName(), this, Server::ExportProperties);
    Server::instance()->registerMessageHandler(m_myAddress, this, "newRequest");
    Server::instance()->registerMonitorNotifier(m_myAddress, this, "modelMonitored");
    connect(Endpoint::instance(), &Endpoint::disconnected, this, [this] { modelMonitored(); });
}

bool RemoteModelServer::isConnected() const
{
    return Endpoint::isConnected();
}

void RemoteModelServer::sendMessage(const Message &msg) const
{
    Endpoint::send(msg);
}

bool RemoteModelServer::proxyDynamicSortFilter() const
{
    if (auto proxy = qobject_cast<QSortFilterProxyModel *>(m_model))
        return proxy->dynamicSortFilter();
    return false;
}

void RemoteModelServer::setProxyDynamicSortFilter(bool dynamicSortFilter)
{
    if (auto proxy = qobject_cast<QSortFilterProxyModel *>(m_model))
        proxy->setDynamicSortFilter(dynamicSortFilter);
}

Qt::CaseSensitivity RemoteModelServer::proxyFilterCaseSensitivity() const
{
    if (auto proxy = qobject_cast<QSortFilterProxyModel *>(m_model))
        return proxy->filterCaseSensitivity();
    return Qt::CaseSensitive;
}

void RemoteModelServer::setProxyFilterCaseSensitivity(Qt::CaseSensitivity caseSensitivity)
{
    if (auto proxy = qobject_cast<QSortFilterProxyModel *>(m_model))
        proxy->setFilterCaseSensitivity(caseSensitivity);
}

int RemoteModelServer::proxyFilterKeyColumn() const
{
    if (auto proxy = qobject_cast<QSortFilterProxyModel *>(m_model))
        return proxy->filterKeyColumn();
    return 0;
}

void RemoteModelServer::setProxyFilterKeyColumn(int column)
{
    if (auto proxy = qobject_cast<QSortFilterProxyModel *>(m_model))
        proxy->setFilterKeyColumn(column);
}

QRegExp RemoteModelServer::proxyFilterRegExp() const
{
    if (auto proxy = qobject_cast<QSortFilterProxyModel *>(m_model))
        return proxy->filterRegExp();
    return QRegExp();
}

void RemoteModelServer::setProxyFilterRegExp(const QRegExp &regExp)
{
    if (auto proxy = qobject_cast<QSortFilterProxyModel *>(m_model))
        proxy->setFilterRegExp(regExp);
}
