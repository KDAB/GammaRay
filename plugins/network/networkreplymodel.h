/*
  networkreplymodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2019 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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

namespace GammaRay {

/** QNetworkReply tracking. */
class NetworkReplyModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(bool captureResponse READ captureResponse WRITE setCaptureResponse NOTIFY captureResponseChanged)
public:
    explicit NetworkReplyModel(QObject *parent = nullptr);
    ~NetworkReplyModel();

    int columnCount(const QModelIndex &parent = {}) const override;
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;

    void objectCreated(QObject *obj);

    struct ReplyNode
    {
        QNetworkReply *reply = nullptr;
        QString displayName;
        QUrl url;
        QStringList errorMsgs;
        qint64 size = 0;
        quint64 duration = 0;
        QByteArray response;
        QNetworkAccessManager::Operation op = QNetworkAccessManager::UnknownOperation;
        int state = NetworkReply::Running;
        NetworkReply::ContentType contentType = NetworkReply::Unknown;
    };

    bool captureResponse() const
    {
        return m_captureResponse;
    }
    void setCaptureResponse(bool newCaptureResponse);

signals:
    void captureResponseChanged();

private:
    struct NAMNode
    {
        QNetworkAccessManager *nam;
        QString displayName;
        std::vector<ReplyNode> replies;
    };

    void replyFinished(QNetworkReply *reply, QNetworkAccessManager *nam);
    void replyProgress(QNetworkReply *reply, qint64 progress, qint64 total, QNetworkAccessManager *nam);
    void replyProgressSync(QNetworkReply *reply, qint64 progress, qint64 total, QNetworkAccessManager *nam);
#ifndef QT_NO_SSL
    void replyEncrypted(QNetworkReply *reply, QNetworkAccessManager *nam);
    void replySslErrors(QNetworkReply *reply, const QList<QSslError> &errors, QNetworkAccessManager *nam);
#endif
    void replyDeleted(QNetworkReply *reply, QNetworkAccessManager *nam);

    void maybePeekResponse(ReplyNode &node, QNetworkReply *reply) const;
    Q_INVOKABLE void updateReplyNode(QNetworkAccessManager *nam, const GammaRay::NetworkReplyModel::ReplyNode &newNode);

    std::vector<NAMNode> m_nodes;
    QElapsedTimer m_time;
    bool m_captureResponse = false;
};
}

#endif // GAMMARAY_NETWORKREPLYMODEL_H
