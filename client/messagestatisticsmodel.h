/*
  messagestatisticsmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_MESSAGESTATISTICSMODEL_H
#define GAMMARAY_MESSAGESTATISTICSMODEL_H

#include <common/protocol.h>

#include <QAbstractTableModel>
#include <QLinearGradient>
#include <QVector>

namespace GammaRay {
/** Diagnostics for GammaRay-internal communication. */
class MessageStatisticsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit MessageStatisticsModel(QObject *parent = nullptr);
    ~MessageStatisticsModel() override;

    void clear();
    void addObject(Protocol::ObjectAddress addr, const QString &name);
    void addMessage(Protocol::ObjectAddress addr, Protocol::MessageType msgType, int size);

    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    int countPerType(int msgType) const;
    quint64 sizePerType(int msgType) const;

    struct Info
    {
        Info();
        int totalCount() const;
        quint64 totalSize() const;

        QString name;
        QVector<int> messageCount;
        QVector<quint64> messageSize;
    };
    QVector<Info> m_data;
    int m_totalCount;
    quint64 m_totalSize;
};
}

#endif // GAMMARAY_MESSAGESTATISTICSMODEL_H
