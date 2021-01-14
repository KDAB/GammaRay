/*
  signalhistorymodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

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

#ifndef GAMMARAY_SIGNALHISTORYMODEL_H
#define GAMMARAY_SIGNALHISTORYMODEL_H

#include <common/objectmodel.h>

#include <QAbstractTableModel>
#include <QHash>
#include <QIcon>
#include <QMetaMethod>
#include <QByteArray>

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

        qint64 timestamp(int i) const { return SignalHistoryModel::timestamp(events.at(i)); }
        int signalIndex(int i) const { return SignalHistoryModel::signalIndex(events.at(i)); }
    };

public:
    enum ColumnId {
        ObjectColumn,
        TypeColumn,
        EventColumn
    };

    enum RoleId {
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

    static qint64 timestamp(qint64 ev) { return ev >> 16; }
    static int signalIndex(qint64 ev) { return ev & 0xffff; }

private:
    Item *item(const QModelIndex &index) const;

private slots:
    void onObjectAdded(QObject *object);
    void onObjectRemoved(QObject *object);
    void onSignalEmitted(QObject *sender, int signalIndex);

private:
    QVector<Item *> m_tracedObjects;
    QHash<QObject *, int> m_itemIndex;
};
} // namespace GammaRay

#endif // GAMMARAY_SIGNALHISTORYMODEL_H
