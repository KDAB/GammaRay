/*
  signalhistorymodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_SIGNALHISTORYMODEL_H
#define GAMMARAY_SIGNALHISTORYMODEL_H

#include <common/objectmodel.h>

#include <QAbstractTableModel>
#include <QHash>
#include <QIcon>
#include <QMetaMethod>
#include <QByteArray>
#include <QSet>

namespace GammaRay {
class Probe;

class SignalHistoryModel : public QAbstractTableModel
{
    Q_OBJECT

private:
    struct Item
    {
        Item(QObject *obj);

        QObject *object; // never dereference, might be invalid!
        QHash<int, QByteArray> signalNames;
        QString objectName;
        QByteArray objectType;
        int decorationId;
        QVector<qint64> events;
        const qint64 startTime; // FIXME: make them all methods
        qint64 endTime() const;

        qint64 timestamp(int i) const
        {
            return SignalHistoryModel::timestamp(events.at(i));
        }
        int signalIndex(int i) const
        {
            return SignalHistoryModel::signalIndex(events.at(i));
        }
    };

public:
    enum ColumnId
    {
        ObjectColumn,
        TypeColumn,
        EventColumn
    };

    enum RoleId
    {
        EventsRole = ObjectModel::UserRole + 1,
        StartTimeRole,
        EndTimeRole,
        SignalMapRole
    };

    explicit SignalHistoryModel(Probe *probe, QObject *parent = nullptr);
    ~SignalHistoryModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;

    static qint64 timestamp(qint64 ev)
    {
        return ev >> 16;
    }
    static int signalIndex(qint64 ev)
    {
        return ev & 0xffff;
    }

private:
    Item *item(const QModelIndex &index) const;

private slots:
    void onObjectAdded(QObject *object);
    void onObjectRemoved(QObject *object);
    void onObjectFavorited(QObject *object);
    void onObjectUnfavorited(QObject *object);
    void onSignalEmitted(QObject *sender, int signalIndex);
    void insertPendingObjects();

private:
    QVector<Item *> m_tracedObjects;
    QHash<QObject *, int> m_itemIndex;
    QSet<QObject *> m_favorites;

    QTimer *m_delayInsertTimer;
    QVector<Item *> m_objectsToBeInserted;
};
} // namespace GammaRay

#endif // GAMMARAY_SIGNALHISTORYMODEL_H
