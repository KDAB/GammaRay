/*
  networkdiscoverymodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_NETWORKDISCOVERYMODEL_H
#define GAMMARAY_NETWORKDISCOVERYMODEL_H

#include "gammaray_launcher_export.h"

#include <QAbstractItemModel>
#include <QVector>
#include <QDateTime>
#include <QUrl>

QT_BEGIN_NAMESPACE
class QUdpSocket;
QT_END_NAMESPACE

namespace GammaRay {
/** \brief Lists all active server instances found via network autodiscovery. */
class GAMMARAY_LAUNCHER_EXPORT NetworkDiscoveryModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Role
    {
        HostNameRole = Qt::UserRole + 1,
        PortRole,
        CompatibleRole,
        UrlStringRole
    };

    explicit NetworkDiscoveryModel(QObject *parent);
    ~NetworkDiscoveryModel() override;

    QVariant data(const QModelIndex &index, int role) const override;
    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private slots:
    void processPendingDatagrams();
    void expireEntries();

private:
    QUdpSocket *m_socket;

    struct ServerInfo
    {
        bool operator==(const ServerInfo &other) const;
        qint32 version;
        QUrl url;
        QString label;
        QDateTime lastSeen;
    };
    QVector<ServerInfo> m_data;
};
}

#endif // GAMMARAY_NETWORKDISCOVERYMODEL_H
