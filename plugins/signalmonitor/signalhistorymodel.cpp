/*
  signalhistorymodel.cpp

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

#include "signalhistorymodel.h"
#include "relativeclock.h"

#include <core/multisignalmapper.h>
#include <core/probeinterface.h>
#include <core/util.h>

#include <QLocale>
#include <QSet>
#include <QThread>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_DECLARE_METATYPE(QVector<qint64>)
#endif // !Qt5

using namespace GammaRay;

const QString SignalHistoryModel::ITEM_TYPE_NAME_OBJECT = "Object";
const QString SignalHistoryModel::ITEM_TYPE_NAME_EVENT = "Event";

SignalHistoryModel::SignalHistoryModel(ProbeInterface *probe, QObject *parent)
  : QAbstractItemModel(parent)
  , m_signalMapper(new MultiSignalMapper(this))
{
  connect(m_signalMapper, SIGNAL(signalEmitted(QObject*,int,QVector<QVariant>)),
          this, SLOT(onSignalEmitted(QObject*,int)));

  connect(probe->probe(), SIGNAL(objectCreated(QObject*)), this, SLOT(onObjectAdded(QObject*)));
  connect(probe->probe(), SIGNAL(objectDestroyed(QObject*)), this, SLOT(onObjectRemoved(QObject*)));
}

SignalHistoryModel::~SignalHistoryModel()
{
}

int SignalHistoryModel::rowCount(const QModelIndex &parent) const
{
  if (!parent.isValid()) {
    return m_tracedObjects.size();
  }

  if (itemType(parent) == ObjectItem) {
    return m_tracedObjects.at(parent.row())->events.size();
  }

  return 0;
}

int SignalHistoryModel::columnCount(const QModelIndex &) const
{
  return 3;
}

QModelIndex SignalHistoryModel::parent(const QModelIndex &child) const
{
  if (itemType(child) == EventItem) {
    const int row = m_tracedObjects.indexOf(item(child));

    if (row >= 0)
      return index(row, child.column());
  }

  return QModelIndex();
}

QModelIndex SignalHistoryModel::index(int row, int column, const QModelIndex &parent) const
{
  if (!parent.isValid()) {
    if (row >= 0 && row < m_tracedObjects.size() && column >= 0 && column < columnCount()) {
      return createIndex(row, column, itemPointer<ObjectItem>(m_tracedObjects.at(row)));
    }
  } else if (!parent.parent().isValid()) {
    if (row >= 0 && row < item(parent)->events.count() && column >= 0 && column < columnCount()) {
      return createIndex(row, column, itemPointer<EventItem>(item(parent)));
    }
  }

  return QModelIndex();
}

QVariant SignalHistoryModel::data(const QModelIndex &index, int role) const
{
  switch (static_cast<ColumnId>(index.column())) {
    case ObjectColumn:
      switch(itemType(index)) {
        case ObjectItem:
          if (role == Qt::DisplayRole)
            return item(index)->objectName;
          if (role == Qt::ToolTipRole)
            return item(index)->toolTip;
          if (role == Qt::DecorationRole)
            return item(index)->decoration;

          break;

        case EventItem:
          if (role == Qt::DisplayRole) {
            const qint64 t = item(index)->timestamp(index.row());
            return tr("%1 ms").arg(QLocale().toString(t));
          }

          if (role == Qt::TextAlignmentRole)
            return Qt::AlignRight;

          break;
      }

      break;

    case TypeColumn:
      switch(itemType(index)) {
        case ObjectItem:
          if (role == Qt::DisplayRole)
            return item(index)->objectType;
          if (role == Qt::ToolTipRole)
            return item(index)->toolTip;

          break;

        case EventItem:
          if (role == Qt::DisplayRole)
            return item(index)->signalName(index.row());

          break;
      }

      break;

    case EventColumn:
      switch(itemType(index)) {
        case ObjectItem:
          if (role == ItemTypeNameRole)
            return ITEM_TYPE_NAME_OBJECT;
          if (role == EventsRole)
            return QVariant::fromValue(item(index)->events);
          if (role == StartTimeRole)
            return item(index)->startTime;
          if (role == EndTimeRole)
            return item(index)->endTime();

          break;

        case EventItem:
          if (role == ItemTypeNameRole)
            return ITEM_TYPE_NAME_EVENT;
          if (role == Qt::ToolTipRole) {
            const Item *const data = item(index);
            const QString &ts = QLocale().toString(data->timestamp(index.row()));
            return tr("%1 at %2 ms").arg(data->signalName(index.row()), ts);
          }

          break;
      }

      break;
  }

  return QVariant();
}

QVariant SignalHistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
    switch (section) {
      case ObjectColumn:
        return tr("Object");
      case TypeColumn:
        return tr("Type");
      case EventColumn:
        return tr("Events");
    }
  }

  return QVariant();
}

QMap< int, QVariant > SignalHistoryModel::itemData(const QModelIndex& index) const
{
  QMap<int, QVariant> d = QAbstractItemModel::itemData(index);
  d.insert(ItemTypeNameRole, data(index, ItemTypeNameRole));
  d.insert(EventsRole, data(index, EventsRole));
  d.insert(StartTimeRole, data(index, StartTimeRole));
  d.insert(EndTimeRole, data(index, EndTimeRole));
  return d;
}

void SignalHistoryModel::onObjectAdded(QObject* object)
{
  Q_ASSERT(thread() == QThread::currentThread());

  // blacklist event dispatchers
  if (QString(object->metaObject()->className()).startsWith("QPAEventDispatcher"))
    return;

  beginInsertRows(QModelIndex(), m_tracedObjects.size(), m_tracedObjects.size());

  Item *const data = new Item(object);

  for (int i = 0, l = data->metaObject->methodCount(); i < l; ++i) {
    const QMetaMethod &method = data->metaObject->method(i);
    if (method.methodType() == QMetaMethod::Signal)
      m_signalMapper->connectToSignal(data->object, method);
  }
  m_itemIndex.insert(object, m_tracedObjects.size());
  m_tracedObjects.push_back(data);

  endInsertRows();
}

void SignalHistoryModel::onObjectRemoved(QObject* object)
{
  Q_ASSERT(thread() == QThread::currentThread());

  const auto it = m_itemIndex.find(object);
  if (it == m_itemIndex.end())
    return;
  const int itemIndex = *it;
  m_itemIndex.erase(it);

  Item *data = m_tracedObjects.at(itemIndex);
  Q_ASSERT(data->object == object);
  data->object = 0;
  emit dataChanged(index(itemIndex, EventColumn), index(itemIndex, EventColumn));
}

void SignalHistoryModel::onSignalEmitted(QObject *sender, int signalIndex)
{
  const qint64 timestamp = RelativeClock::sinceAppStart()->mSecs();

  const auto it = m_itemIndex.constFind(sender);
  if (it == m_itemIndex.constEnd())
    return;
  const int itemIndex = *it;

  Item *const data = m_tracedObjects.at(itemIndex);
  Q_ASSERT(data->object == sender);
  const int newRow = data->events.size();
  beginInsertRows(index(itemIndex, EventColumn), newRow, newRow);
  data->events.push_back((timestamp << 16) | signalIndex);
  endInsertRows();
  emit dataChanged(index(itemIndex, EventColumn), index(itemIndex, EventColumn));
}

/// Tries to reuse an already existing instances of \param str by checking
/// a global string pool. If no instance of \param str is interned yet the
/// string will be added to the pool.
static QString internString(const QString &str)
{
  static QSet<QString> pool;

  // Check if the pool already contains the string...
  const QSet<QString>::const_iterator it = pool.find(str);

  //  ...and return it if possible.
  if (it != pool.end())
    return *it;

  // Otherwise add the string to the pool.
  pool.insert(str);
  return str;
}

SignalHistoryModel::Item::Item(QObject *obj)
  : object(obj)
  , metaObject(object->metaObject()) // FIXME: how about non-static meta objects?
  , startTime(RelativeClock::sinceAppStart()->mSecs())
{
  objectName = Util::shortDisplayString(object);
  objectType = internString(metaObject->className());
  toolTip = Util::tooltipForObject(object);
  decoration = Util::iconForObject(object).value<QIcon>();
}

qint64 SignalHistoryModel::Item::endTime(/*qint64 now*/) const
{
  const qint64 now = RelativeClock::sinceAppStart()->mSecs(); // FIXME

  if (object)
    return now;
  if (not events.isEmpty())
    return timestamp(events.size() - 1);

  return startTime;
}

