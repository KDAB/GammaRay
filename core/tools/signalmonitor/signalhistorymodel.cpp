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

#include "multisignalmapper.h"
#include "probeinterface.h"
#include "relativeclock.h"

#include <QLocale>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_DECLARE_METATYPE(QVector<qint64>)
#endif // !Qt5

using namespace GammaRay;

const QString SignalHistoryModel::ITEM_TYPE_NAME_OBJECT = "Object";
const QString SignalHistoryModel::ITEM_TYPE_NAME_EVENT = "Event";

SignalHistoryModel::SignalHistoryModel(ProbeInterface *probe, QObject *parent)
  : QAbstractItemModel(parent)
  , m_objectTreeModel(probe->objectTreeModel())
  , m_signalMapper(new MultiSignalMapper(this))
{
  connect(m_objectTreeModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
          this, SLOT(onRowsInserted(QModelIndex,int,int)));
  connect(m_signalMapper, SIGNAL(signalEmitted(QObject*,int,QVector<QVariant>)),
          this, SLOT(onSignalEmitted(QObject*,int)));
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
            return tr("%1 ms").arg(QLocale::system().toString(t));
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
            const QString &ts = QLocale::system().toString(data->timestamp(index.row()));
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

void SignalHistoryModel::onRowsInserted(const QModelIndex &otherParent, int first, int last)
{
  beginInsertRows(QModelIndex(), m_tracedObjects.size(), m_tracedObjects.size() + last - first);

  for (int i = first; i <= last; ++i) {
    Item *const data = new Item(m_objectTreeModel->index(i, 0, otherParent));

    // blacklist event dispatchers
    if (data->objectType.startsWith("QPAEventDispatcher"))
      continue;

    for (int i = 0, l = data->metaObject->methodCount(); i < l; ++i) {
      const QMetaMethod &method = data->metaObject->method(i);

      if (method.methodType() == QMetaMethod::Signal)
        m_signalMapper->connectToSignal(data->object, method);
    }

    m_tracedObjects.append(data);
  }

  endInsertRows();
}

void SignalHistoryModel::onSignalEmitted(QObject *sender, int signalIndex)
{
  // FIXME: optimize this linear lookup
  const qint64 timestamp = RelativeClock::sinceAppStart()->mSecs();

  for (int i = 0, l = m_tracedObjects.size(); i < l; ++i) {
    Item *const data = m_tracedObjects.at(i);

    if (data->object == sender) {
      const int newRow = data->events.size();
      beginInsertRows(index(i, EventColumn), newRow, newRow);
      data->events.push_back((timestamp << 16) | signalIndex);
      endInsertRows();
      break;
    }
  }
}

static void interned(const QString &input, QString *result)
{
  if (input.constData() != result->constData()) {
    static QHash<QString, QString> pool;

    QHash<QString, QString>::iterator it = pool.find(input);

    if (it == pool.end())
      it = pool.insert(input, input);

    *result = input;
  }
}

SignalHistoryModel::Item::Item(const QModelIndex &index)
  : object(index.model()->data(index, ObjectModel::ObjectRole).value<QObject *>())
  , metaObject(object->metaObject()) // FIXME: how about non-static meta objects?
  , startTime(RelativeClock::sinceAppStart()->mSecs())
{

  updateFromModel(index);
}

void SignalHistoryModel::Item::updateFromModel(const QModelIndex &index)
{
  if (object) {
    const QAbstractItemModel *const model = index.model();

    interned(model->data(model->index(index.row(), 0, index.parent()), Qt::DisplayRole).toString(), &objectName);
    interned(model->data(model->index(index.row(), 1, index.parent()), Qt::DisplayRole).toString(), &objectType);

    toolTip = model->data(model->index(index.row(), 0, index.parent()), Qt::ToolTipRole).toString();
    decoration = model->data(model->index(index.row(), 0, index.parent()), Qt::DecorationRole).value<QIcon>();
  }
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

