/*
  networkreplymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "networkreplymodel.h"

#include <core/util.h>
#include <common/objectid.h>

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <iostream>
#include <limits>

using namespace GammaRay;

static const auto TopIndex = std::numeric_limits<quintptr>::max();

NetworkReplyModel::NetworkReplyModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    m_time.start();
}

NetworkReplyModel::~NetworkReplyModel() = default;

int NetworkReplyModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return NetworkReplyModelColumn::COLUMN_COUNT;
}

int NetworkReplyModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid()) {
        return m_nodes.size();
    }

    if (parent.internalId() == TopIndex) {
        return m_nodes[parent.row()].replies.size();
    }

    return 0;
}

QVariant NetworkReplyModel::data(const QModelIndex& index, int role) const
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
            case NetworkReplyModelColumn::ObjectColumn: return reply.displayName;
            case NetworkReplyModelColumn::OpColumn: return reply.op;
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
            case NetworkReplyModelColumn::UrlColumn: return reply.url;
        }
    } else if (role == NetworkReplyModelRole::ReplyStateRole && index.column() == NetworkReplyModelColumn::ObjectColumn) {
        return reply.state;
    } else if (role == NetworkReplyModelRole::ReplyErrorRole && index.column() == NetworkReplyModelColumn::ObjectColumn) {
        return reply.errorMsgs;
    } else if (role == NetworkReplyModelRole::ObjectIdRole && index.column() == NetworkReplyModelColumn::ObjectColumn) {
        return QVariant::fromValue(ObjectId(reply.reply));
    }

    return {};
}

QModelIndex NetworkReplyModel::index(int row, int column, const QModelIndex& parent) const
{
    // top-level
    if (!parent.isValid()) {
        return createIndex(row, column, TopIndex);
    }

    return createIndex(row, column, parent.row());
}

QModelIndex NetworkReplyModel::parent(const QModelIndex& child) const
{
    if (child.internalId() == TopIndex) {
        return {};
    }

    return createIndex(child.internalId(), 0, TopIndex);
}

void NetworkReplyModel::objectCreated(QObject *obj)
{
    if (auto nam = qobject_cast<QNetworkAccessManager*>(obj)) {
        beginInsertRows({}, m_nodes.size(), m_nodes.size());
        NAMNode node;
        node.nam = nam;
        node.displayName = Util::displayString(nam);
        m_nodes.push_back(node);
        endInsertRows();
    }

    if (auto reply = qobject_cast<QNetworkReply*>(obj)) {
        auto nam = reply->manager();
        auto namIt = std::find_if(m_nodes.begin(), m_nodes.end(), [nam](const NAMNode &node) {
            return node.nam == nam;
        });

        if (namIt == m_nodes.end()) {
            // TODO
            return;
        }

        // capture nam, as we cannot deref reply anymore when this triggers
        connect(reply, &QNetworkReply::finished, this, [this, reply, nam]() { replyFinished(reply, nam); });
        connect(reply, &QNetworkReply::encrypted, this, [this, reply, nam]() { replyEncrypted(reply, nam); });
        connect(reply, &QNetworkReply::downloadProgress, this, [this, reply, nam](qint64 received, qint64 total) { replyProgress(reply, received, total, nam); });
        connect(reply, &QNetworkReply::uploadProgress, this, [this, reply, nam](qint64 received, qint64 total) { replyProgress(reply, received, total, nam); });
        connect(reply, &QNetworkReply::sslErrors, this, [this, reply, nam](const QList<QSslError> &errors) { replySslErrors(reply, errors, nam); });
        connect(reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, [this, reply, nam]() { replyError(reply, nam); });
        connect(reply, &QNetworkReply::destroyed, this, [this, reply, nam]() { replyDeleted(reply, nam); });
        const auto parentIdx = createIndex(std::distance(m_nodes.begin(), namIt), 0, TopIndex);
        beginInsertRows(parentIdx, (*namIt).replies.size(), (*namIt).replies.size());
        ReplyNode replyNode;
        replyNode.reply = reply;
        replyNode.displayName = Util::displayString(reply);
        replyNode.op = reply->operation();
        replyNode.url = reply->url();
        replyNode.size = 0;
        replyNode.state = reply->url().scheme() == QLatin1String("https") ? NetworkReply::Encrypted : NetworkReply::Running;
        if (reply->isFinished()) {
            replyNode.state |= NetworkReply::Finished;
            replyNode.duration = 0;
        } else {
            replyNode.duration = m_time.elapsed();
        }
        (*namIt).replies.push_back(replyNode);
        endInsertRows();
    }
}

QMap<int, QVariant> NetworkReplyModel::itemData(const QModelIndex& index) const
{
    auto m = QAbstractItemModel::itemData(index);
    if (index.column() == 0) {
        m.insert(NetworkReplyModelRole::ReplyStateRole, data(index, NetworkReplyModelRole::ReplyStateRole));
        m.insert(NetworkReplyModelRole::ReplyErrorRole, data(index, NetworkReplyModelRole::ReplyErrorRole));
        m.insert(NetworkReplyModelRole::ObjectIdRole, data(index, NetworkReplyModelRole::ObjectIdRole));
    }
    return m;
}

void NetworkReplyModel::replyFinished(QNetworkReply* reply, QNetworkAccessManager *nam)
{
    updateReplyNode(reply, nam, [this](QNetworkReply*, ReplyNode &node) {
        node.state |= NetworkReply::Finished;
        node.duration = m_time.elapsed() - node.duration;
    });
}

void NetworkReplyModel::replyError(QNetworkReply* reply, QNetworkAccessManager *nam)
{
    updateReplyNode(reply, nam, [](QNetworkReply *reply, ReplyNode &node) {
        node.state |= NetworkReply::Error;
        node.errorMsgs.push_back(reply->errorString());
    });
}

void NetworkReplyModel::replyEncrypted(QNetworkReply* reply, QNetworkAccessManager *nam)
{
    updateReplyNode(reply, nam, [](QNetworkReply*, ReplyNode &node) {
        node.state |= NetworkReply::Encrypted;
    });
}

void NetworkReplyModel::replyProgress(QNetworkReply* reply, qint64 progress, qint64 total, QNetworkAccessManager *nam)
{
    updateReplyNode(reply, nam, [progress, total](QNetworkReply*, ReplyNode &node) {
        node.size = std::max(node.size, std::max(progress, total));
    });
}

void NetworkReplyModel::replySslErrors(QNetworkReply* reply, const QList<QSslError>& errors, QNetworkAccessManager *nam)
{
    updateReplyNode(reply, nam, [errors](QNetworkReply*, ReplyNode &node) {
        for (const auto &err : errors) {
            node.state |= NetworkReply::Error;
            node.state &= ~NetworkReply::Encrypted;
            node.errorMsgs.push_back(err.errorString());
        }
    });
}

void NetworkReplyModel::replyDeleted(QNetworkReply* reply, QNetworkAccessManager* nam)
{
    updateReplyNode(reply, nam, [](QNetworkReply*, ReplyNode &node) {
        node.state |= NetworkReply::Deleted;
    });
}

void NetworkReplyModel::updateReplyNode(QNetworkReply* reply, QNetworkAccessManager *nam, const std::function<void (QNetworkReply*, NetworkReplyModel::ReplyNode&)>& changeFunc)
{
    const auto namIt = std::find_if(m_nodes.begin(), m_nodes.end(), [nam](const NAMNode &node) {
        return node.nam == nam;
    });

    if (namIt == m_nodes.end()) {
        return;
    }

    const auto replyIt = std::find_if((*namIt).replies.begin(), (*namIt).replies.end(), [reply](const ReplyNode &node) {
        return node.reply == reply;
    });
    if (replyIt == (*namIt).replies.end()) {
        return;
    }

    changeFunc(reply, *replyIt);
    const auto idx = createIndex(std::distance((*namIt).replies.begin(), replyIt), 0, std::distance(m_nodes.begin(), namIt));
    emit dataChanged(idx, idx.sibling(idx.row(), columnCount() - 1));
}
