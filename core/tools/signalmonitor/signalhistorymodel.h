/*
  signalhistorymodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

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

#include <QAbstractItemModel>
#include <QIcon>
#include <QMetaMethod>
#include <QPointer>

namespace GammaRay {

class MultiSignalMapper;
class ProbeInterface;

class SignalHistoryModel : public QAbstractItemModel
{
  Q_OBJECT

  private:
    struct Item
    {
      Item(const QModelIndex &index);
      void updateFromModel(const QModelIndex &index);

      const QPointer<QObject> object;
      const QMetaObject *const metaObject;
      QString objectName;
      QString objectType;
      QString toolTip;
      QIcon decoration;
      QVector<qint64> events;
      const qint64 startTime; // FIXME: make them all methods
      qint64 endTime(/*qint64 now*/) const;

      qint64 timestamp(int i) const { return SignalHistoryModel::timestamp(events.at(i)); }
      int signalIndex(int i) const { return SignalHistoryModel::signalIndex(events.at(i)); }
      QString signalName(int i) const { return metaObject->method(signalIndex(i)).methodSignature(); }
    };

  public:
    static const QString ITEM_TYPE_NAME_OBJECT;
    static const QString ITEM_TYPE_NAME_EVENT;

    enum ItemType {
      ObjectItem,
      EventItem
    };

    enum ColumnId {
      ObjectColumn,
      TypeColumn,
      EventColumn
    };

    enum RoleId {
      ItemTypeNameRole = ObjectModel::UserRole + 1,
      EventsRole,
      StartTimeRole,
      EndTimeRole
    };

    explicit SignalHistoryModel(ProbeInterface *probe, QObject *parent = 0);
    ~SignalHistoryModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    QModelIndex parent(const QModelIndex &child) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    static qint64 timestamp(qint64 ev) { return ev >> 16; }
    static int signalIndex(qint64 ev) { return ev & 0xffff; }

  private:
    template<ItemType type> static void * itemPointer(void *p);
    static ItemType itemType(const QModelIndex &index);
    static Item *item(const QModelIndex &index);

  private slots:
    void onRowsInserted(const QModelIndex &otherParent, int first, int last);
    void onSignalEmitted(QObject *sender, int signalIndex);

  private:
    const qint64 m_startTime;
    QAbstractItemModel *const m_objectTreeModel;
    MultiSignalMapper *const m_signalMapper;
    QVector<Item *> m_tracedObjects;
};

template<SignalHistoryModel::ItemType type>
inline void * SignalHistoryModel::itemPointer(void *p)
{
  const qintptr q = reinterpret_cast<qintptr>(p) & ~static_cast<qintptr>(3);
  return reinterpret_cast<void *>(q | type);
}

inline SignalHistoryModel::ItemType SignalHistoryModel::itemType(const QModelIndex &index)
{
  return static_cast<ItemType>(index.internalId() & 3);
}

inline SignalHistoryModel::Item * SignalHistoryModel::item(const QModelIndex &index)
{
  return reinterpret_cast<Item *>(index.internalId() & ~static_cast<qintptr>(3));
}

} // namespace GammaRay

#endif // GAMMARAY_SIGNALHISTORYMODEL_H
