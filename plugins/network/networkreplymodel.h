/*
  networkreplymodel.h

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

#ifndef GAMMARAY_NETWORKREPLYMODEL_H
#define GAMMARAY_NETWORKREPLYMODEL_H

#include "networkreplymodeldefs.h"

#include <QAbstractItemModel>
#include <QElapsedTimer>
#include <QNetworkAccessManager>
#include <QUrl>

#include <functional>

QT_BEGIN_NAMESPACE
class QNetworkReply;
QT_END_NAMESPACE

namespace GammaRay
{

/** QNetworkReply tracking. */
class NetworkReplyModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit NetworkReplyModel(QObject *parent = nullptr);
    ~NetworkReplyModel();

    int columnCount(const QModelIndex &parent = {}) const override;
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QMap<int, QVariant> itemData(const QModelIndex & index) const override;

    void objectCreated(QObject *obj);

    struct ReplyNode {
        QNetworkReply *reply;
        QString displayName;
        QUrl url;
        QStringList errorMsgs;
        qint64 size = 0;
        quint64 duration = 0;
        QNetworkAccessManager::Operation op;
        int state = NetworkReply::Running;
    };

private:
    struct NAMNode {
        QNetworkAccessManager *nam;
        QString displayName;
        std::vector<ReplyNode> replies;
    };

    void replyFinished(QNetworkReply *reply, QNetworkAccessManager *nam);
    void replyEncrypted(QNetworkReply *reply, QNetworkAccessManager *nam);
    void replyProgress(QNetworkReply *reply, qint64 progress, qint64 total, QNetworkAccessManager *nam);
    void replySslErrors(QNetworkReply *reply, const QList<QSslError> &errors, QNetworkAccessManager *nam);
    void replyDeleted(QNetworkReply *reply, QNetworkAccessManager *nam);

    Q_INVOKABLE void updateReplyNode(QNetworkAccessManager *nam, const GammaRay::NetworkReplyModel::ReplyNode &newNode);

    std::vector<NAMNode> m_nodes;
    QElapsedTimer m_time;
};

}

#endif // GAMMARAY_NETWORKREPLYMODEL_H
