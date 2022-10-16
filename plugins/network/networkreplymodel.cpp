/*
  networkreplymodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2019-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "networkreplymodel.h"

#include <core/util.h>
#include <common/objectid.h>

#include <QNetworkAccessManager>
#include <QNetworkReply>

// TODO: Should the network module import Qt Private headers? Or should this be somewhere else?
#include <private/qobject_p.h>
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
#include <private/qobject_p_p.h>
#endif

#include <iostream>
#include <limits>

using namespace GammaRay;

static const auto TopIndex = std::numeric_limits<quintptr>::max();

Q_DECLARE_METATYPE(GammaRay::NetworkReplyModel::ReplyNode)

namespace {
bool prioritizeLatestConnection(QObject *sender, const char *normalizedSignalName, QObject *receiver)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    return false;
#else
    const auto senderPrivate = QObjectPrivate::get(sender);
    const auto sigIndex = senderPrivate->signalIndex(normalizedSignalName);
    if (sigIndex < 0) {
        return false;
    }

    const auto connectionData = senderPrivate->connections.loadRelaxed();
    if (!connectionData) {
        return false;
    }

    auto signalsVector = connectionData->signalVector.loadRelaxed();
    if (!signalsVector) {
        return false;
    }

    QObjectPrivate::Connection *ourConn = nullptr;
    for (int i = 0; i < signalsVector->count(); ++i) {
        QObjectPrivate::Connection *conn = signalsVector->at(i).first;
        while (conn) {
            if (conn->signal_index == sigIndex && conn->receiver == receiver) {
                ourConn = conn;
                // We continue because we want to locate the latest connection,
                // i.e. the connection we just made
            }

            conn = conn->nextConnectionList;
        }

        // TODO: The whole thing needs to be atomic
        if (ourConn) {
            if (ourConn == signalsVector->at(i).first) {
                qDebug() << "We are already the first, nothing to do";
                return true;
            }

            qDebug() << "Swapping" << ourConn->receiver << "with"
                     << static_cast<QObjectPrivate::Connection *>(signalsVector->at(i).first)->receiver;
            ourConn->prevConnectionList->nextConnectionList.storeRelaxed(ourConn->nextConnectionList);
            ourConn->nextConnectionList.storeRelaxed(signalsVector->at(i).first);
            signalsVector->at(i).first.storeRelaxed(ourConn);
            return true;
        }
    }

    return false;
#endif
}

NetworkReply::ContentType contentType(const QVariant &v)
{
    if (v.toString().contains(QLatin1String("application/json"))) {
        return NetworkReply::Json;
    }
    return NetworkReply::Unknown;
}

}

NetworkReplyModel::NetworkReplyModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    m_time.start();

    qRegisterMetaType<QNetworkAccessManager *>();
    qRegisterMetaType<GammaRay::NetworkReplyModel::ReplyNode>();
}

NetworkReplyModel::~NetworkReplyModel() = default;

int NetworkReplyModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return NetworkReplyModelColumn::COLUMN_COUNT;
}

int NetworkReplyModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return m_nodes.size();
    }

    if (parent.internalId() == TopIndex) {
        return m_nodes[parent.row()].replies.size();
    }

    return 0;
}

QVariant NetworkReplyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    // top-level
    if (index.internalId() == TopIndex) {
        const auto &nam = m_nodes[index.row()];
        if (index.column() == NetworkReplyModelColumn::ObjectColumn && role == Qt::DisplayRole) {
            return nam.displayName;
        } else if (role == NetworkReplyModelRole::ObjectIdRole && index.column() == NetworkReplyModelColumn::ObjectColumn) {
            return QVariant::fromValue(ObjectId(nam.nam));
        }
        return {};
    }

    // reply level
    const auto &reply = m_nodes[index.internalId()].replies[index.row()];
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case NetworkReplyModelColumn::ObjectColumn:
            return reply.displayName;
        case NetworkReplyModelColumn::OpColumn:
            return reply.op;
        case NetworkReplyModelColumn::SizeColumn:
            if (reply.duration == 0 && reply.size == 0) {
                return {}; // cached reply, we don't have proper data for that
            }
            return reply.size;
        case NetworkReplyModelColumn::TimeColumn:
            if (reply.state & NetworkReply::Finished) {
                return reply.duration;
            }
            return {};
        case NetworkReplyModelColumn::UrlColumn:
            return reply.url;
        }
    } else if (role == NetworkReplyModelRole::ReplyStateRole && index.column() == NetworkReplyModelColumn::ObjectColumn) {
        return reply.state;
    } else if (role == NetworkReplyModelRole::ReplyErrorRole && index.column() == NetworkReplyModelColumn::ObjectColumn) {
        return reply.errorMsgs;
    } else if (role == NetworkReplyModelRole::ObjectIdRole && index.column() == NetworkReplyModelColumn::ObjectColumn) {
        return QVariant::fromValue(ObjectId(reply.reply));
    } else if (role == NetworkReplyModelRole::ReplyResponseRole && index.column() == NetworkReplyModelColumn::ObjectColumn) {
        return reply.response;
    } else if (role == NetworkReplyModelRole::ReplyContentType && index.column() == NetworkReplyModelColumn::ObjectColumn) {
        return reply.contentType;
    }

    return {};
}

QModelIndex NetworkReplyModel::index(int row, int column, const QModelIndex &parent) const
{
    // top-level
    if (!parent.isValid()) {
        return createIndex(row, column, TopIndex);
    }

    return createIndex(row, column, parent.row());
}

QModelIndex NetworkReplyModel::parent(const QModelIndex &child) const
{
    if (child.internalId() == TopIndex) {
        return {};
    }

    return createIndex(child.internalId(), 0, TopIndex);
}

void NetworkReplyModel::objectCreated(QObject *obj)
{
    if (auto nam = qobject_cast<QNetworkAccessManager *>(obj)) {
        beginInsertRows({}, m_nodes.size(), m_nodes.size());
        NAMNode node;
        node.nam = nam;
        node.displayName = Util::displayString(nam);
        m_nodes.push_back(node);
        endInsertRows();

        connect(
            nam, &QNetworkAccessManager::finished, this, [this, nam](QNetworkReply *reply) { replyFinished(reply, nam); }, Qt::DirectConnection);
#ifndef QT_NO_SSL
        connect(
            nam, &QNetworkAccessManager::encrypted, this, [this, nam](QNetworkReply *reply) { replyEncrypted(reply, nam); }, Qt::DirectConnection);
        connect(nam, &QNetworkAccessManager::sslErrors, this, [this, nam](QNetworkReply *reply, const QList<QSslError> &errors) { replySslErrors(reply, errors, nam); });
#endif
    }

    if (auto reply = qobject_cast<QNetworkReply *>(obj)) {
        auto nam = reply->manager();
        auto namIt = std::find_if(m_nodes.begin(), m_nodes.end(), [nam](const NAMNode &node) {
            return node.nam == nam;
        });

        if (namIt == m_nodes.end()) {
            // TODO
            return;
        }

        ReplyNode replyNode;
        replyNode.reply = reply;
        replyNode.displayName = Util::displayString(reply);
        replyNode.op = reply->operation();
        replyNode.url = reply->url();
        if (reply->isFinished()) {
            replyNode.state |= NetworkReply::Finished;
            replyNode.duration = 0;
        } else {
            replyNode.duration = m_time.elapsed();
        }
        replyNode.contentType = contentType(reply->header(QNetworkRequest::ContentTypeHeader));
        updateReplyNode(nam, replyNode);

        if (m_captureResponse) {
            connect(
                reply, &QNetworkReply::downloadProgress, this, [this, reply, nam](qint64 received, qint64 total) { replyProgressSync(reply, received, total, nam); }, Qt::DirectConnection);
            if (!prioritizeLatestConnection(reply, QMetaObject::normalizedSignature("downloadProgress(qint64,qint64)"), this)) {
                qWarning() << "Failed to prioritize our slot, capturing network response might not work";
            }
        }

        // capture nam, as we cannot deref reply anymore when this triggers
        connect(reply, &QNetworkReply::downloadProgress, this, [this, reply, nam](qint64 received, qint64 total) { replyProgress(reply, received, total, nam); });
        connect(reply, &QNetworkReply::uploadProgress, this, [this, reply, nam](qint64 received, qint64 total) { replyProgress(reply, received, total, nam); });
        connect(reply, &QNetworkReply::destroyed, this, [this, reply, nam]() { replyDeleted(reply, nam); });
    }
}

QMap<int, QVariant> NetworkReplyModel::itemData(const QModelIndex &index) const
{
    auto m = QAbstractItemModel::itemData(index);
    if (index.column() == 0) {
        m.insert(NetworkReplyModelRole::ReplyStateRole, data(index, NetworkReplyModelRole::ReplyStateRole));
        m.insert(NetworkReplyModelRole::ReplyErrorRole, data(index, NetworkReplyModelRole::ReplyErrorRole));
        m.insert(NetworkReplyModelRole::ObjectIdRole, data(index, NetworkReplyModelRole::ObjectIdRole));
        m.insert(NetworkReplyModelRole::ReplyResponseRole, data(index, NetworkReplyModelRole::ReplyResponseRole));
        m.insert(NetworkReplyModelRole::ReplyContentType, data(index, NetworkReplyModelRole::ReplyContentType));
    }
    return m;
}

void NetworkReplyModel::replyFinished(QNetworkReply *reply, QNetworkAccessManager *nam)
{
    /// WARNING this runs in the thread of the reply, not the thread of this!
    ReplyNode node;
    node.reply = reply;
    node.displayName = Util::displayString(reply);
    node.url = reply->url();
    node.op = reply->operation();
    node.state |= NetworkReply::Finished;
    node.duration = m_time.elapsed() - node.duration;
    node.contentType = contentType(reply->header(QNetworkRequest::ContentTypeHeader));

    maybePeekResponse(node, reply);

    if (reply->error() != QNetworkReply::NoError) {
        node.state |= NetworkReply::Error;
        node.errorMsgs.push_back(reply->errorString());
    }

    QMetaObject::invokeMethod(this, "updateReplyNode", Qt::AutoConnection, Q_ARG(QNetworkAccessManager *, nam), Q_ARG(GammaRay::NetworkReplyModel::ReplyNode, node));

    if (reply->thread() != thread()) {
        connect(
            reply, &QNetworkReply::destroyed, this, [this, reply, nam]() { replyDeleted(reply, nam); }, Qt::DirectConnection);
    }
}

void NetworkReplyModel::replyProgress(QNetworkReply *reply, qint64 progress, qint64 total, QNetworkAccessManager *nam)
{
    ReplyNode node;
    node.reply = reply;
    node.size = std::max(progress, total);
    updateReplyNode(nam, node);
}

void NetworkReplyModel::replyProgressSync(QNetworkReply *reply, qint64 progress, qint64 total, QNetworkAccessManager *nam)
{
    /// WARNING this runs in the thread of the reply, not the thread of this!
    ReplyNode node;
    node.reply = reply;
    node.size = std::max(progress, total);
    maybePeekResponse(node, reply);

    QMetaObject::invokeMethod(this, "updateReplyNode", Qt::AutoConnection, Q_ARG(QNetworkAccessManager *, nam), Q_ARG(GammaRay::NetworkReplyModel::ReplyNode, node));
}

#ifndef QT_NO_SSL
void NetworkReplyModel::replyEncrypted(QNetworkReply *reply, QNetworkAccessManager *nam)
{
    /// WARNING this runs in the thread of the reply, not the thread of this!
    ReplyNode node;
    node.reply = reply;
    node.displayName = Util::displayString(reply);
    node.url = reply->url();
    node.op = reply->operation();
    node.state |= NetworkReply::Encrypted;

    QMetaObject::invokeMethod(this, "updateReplyNode", Qt::AutoConnection, Q_ARG(QNetworkAccessManager *, nam), Q_ARG(GammaRay::NetworkReplyModel::ReplyNode, node));
}

void NetworkReplyModel::replySslErrors(QNetworkReply *reply, const QList<QSslError> &errors, QNetworkAccessManager *nam)
{
    /// WARNING this runs in the thread of the reply, not the thread of this!
    ReplyNode node;
    node.reply = reply;
    node.displayName = Util::displayString(reply);
    node.url = reply->url();
    node.op = reply->operation();
    node.state |= NetworkReply::Error | NetworkReply::Unencrypted;
    for (const auto &err : errors) {
        node.errorMsgs.push_back(err.errorString());
    }

    QMetaObject::invokeMethod(this, "updateReplyNode", Qt::AutoConnection, Q_ARG(QNetworkAccessManager *, nam), Q_ARG(GammaRay::NetworkReplyModel::ReplyNode, node));
}
#endif

void NetworkReplyModel::replyDeleted(QNetworkReply *reply, QNetworkAccessManager *nam)
{
    /// WARNING this runs in the thread of the reply, not the thread of this!
    ReplyNode node;
    node.reply = reply;
    node.state |= NetworkReply::Deleted;
    QMetaObject::invokeMethod(this, "updateReplyNode", Qt::AutoConnection, Q_ARG(QNetworkAccessManager *, nam), Q_ARG(GammaRay::NetworkReplyModel::ReplyNode, node));
}

void NetworkReplyModel::maybePeekResponse(ReplyNode &node, QNetworkReply *reply) const
{
    if (m_captureResponse) {
        // TODO: Allow whitelisting a set of Content-Type values
        // TODO: Make the max size configurable
        const auto resp = reply->peek(5 * 1024 * 1024); // Read up to 5 MiB
        if (!resp.isEmpty())
            node.response = resp;
    }
}

void NetworkReplyModel::updateReplyNode(QNetworkAccessManager *nam, const NetworkReplyModel::ReplyNode &newNode)
{
    // WARNING reply is no longer safe to deref here!
    const auto namIt = std::find_if(m_nodes.begin(), m_nodes.end(), [nam](const NAMNode &node) {
        return node.nam == nam;
    });

    if (namIt == m_nodes.end()) {
        return;
    }

    auto replyIt = std::find_if((*namIt).replies.rbegin(), (*namIt).replies.rend(), [newNode](const ReplyNode &node) {
        // be careful with considering past deleted nodes, address can be reused
        return node.reply == newNode.reply && ((node.state & NetworkReply::Deleted) == 0 || newNode.state & NetworkReply::Deleted);
    });

    if (replyIt == (*namIt).replies.rend()) {
        const auto parentIdx = createIndex(std::distance(m_nodes.begin(), namIt), 0, TopIndex);
        beginInsertRows(parentIdx, (*namIt).replies.size(), (*namIt).replies.size());
        ReplyNode replyNode = newNode;
        if (replyNode.displayName.isEmpty()) {
            replyNode.displayName = Util::addressToString(newNode.reply);
        }
        (*namIt).replies.push_back(replyNode);
        endInsertRows();
        replyIt = (*namIt).replies.rbegin();
    } else {
        if (!newNode.displayName.isEmpty()) {
            (*replyIt).displayName = newNode.displayName;
        }
        (*replyIt).state |= newNode.state;
        if ((*replyIt).state & NetworkReply::Unencrypted) {
            (*replyIt).state &= ~NetworkReply::Encrypted;
        }
        if (!newNode.url.isEmpty()) {
            (*replyIt).url = newNode.url;
            (*replyIt).op = newNode.op;
        }
        if (!newNode.response.isEmpty()) {
            (*replyIt).response = newNode.response;
        }
        (*replyIt).errorMsgs += newNode.errorMsgs;
        if ((*replyIt).duration > 0 && newNode.duration > 0 && (newNode.state & NetworkReply::Finished)) {
            (*replyIt).duration = newNode.duration > (*replyIt).duration ? newNode.duration - (*replyIt).duration : 0;
        }
        (*replyIt).size = std::max((*replyIt).size, newNode.size);
        if (newNode.contentType != NetworkReply::Unknown)
            (*replyIt).contentType = newNode.contentType;

        const auto idx = createIndex(std::distance(replyIt, (*namIt).replies.rend()) - 1, 0, std::distance(m_nodes.begin(), namIt));
        emit dataChanged(idx, idx.sibling(idx.row(), columnCount() - 1));
    }
}

void NetworkReplyModel::setCaptureResponse(bool newCaptureResponse)
{
    if (m_captureResponse == newCaptureResponse)
        return;
    m_captureResponse = newCaptureResponse;
    emit captureResponseChanged();
}
