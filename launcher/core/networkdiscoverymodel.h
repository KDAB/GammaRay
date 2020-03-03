/*
  networkdiscoverymodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
    enum Role {
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

    struct ServerInfo {
        bool operator==(const ServerInfo &other);
        qint32 version;
        QUrl url;
        QString label;
        QDateTime lastSeen;
    };
    QVector<ServerInfo> m_data;
};
}

#endif // GAMMARAY_NETWORKDISCOVERYMODEL_H
