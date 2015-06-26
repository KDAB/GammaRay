/*
  actionmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#include "actionmodel.h"
#include "actionvalidator.h"

#include <core/probe.h>
#include <core/util.h>
#include <core/varianthandler.h>
#include <common/objectmodel.h>

#include <QAction>
#include <QDebug>
#include <QMutex>
#include <QThread>

Q_DECLARE_METATYPE(QAction::Priority)

using namespace GammaRay;

template<class T>
static QString toString(QList<T> list)
{
  QStringList items;
  Q_FOREACH (const T &item, list) {
    items << item;
  }
  return items.join(", ");
}

ActionModel::ActionModel(QObject *parent)
  : QAbstractTableModel(parent),
  m_duplicateFinder(new ActionValidator(this))
{
}

ActionModel::~ActionModel()
{
}

void ActionModel::objectAdded(QObject* object)
{
    // see Probe::objectCreated, that promises a valid object in the main thread
    Q_ASSERT(QThread::currentThread() == thread());
    Q_ASSERT(object);

    QAction* const action = qobject_cast<QAction*>(object);
    if (!action)
        return;

    QVector<QAction*>::iterator it = std::lower_bound(m_actions.begin(), m_actions.end(), action);
    Q_ASSERT(it == m_actions.end() || *it != action);

    const int row = std::distance(m_actions.begin(), it);
    Q_ASSERT(row >= 0 && row <= m_actions.size());

    beginInsertRows(QModelIndex(), row, row);
    m_actions.insert(it, action);
    Q_ASSERT(m_actions.at(row) == action);
    m_duplicateFinder->insert(action);
    endInsertRows();
}

void ActionModel::objectRemoved(QObject* object)
{
    Q_ASSERT(thread() == QThread::currentThread());
    QAction* const action = reinterpret_cast<QAction*>(object); // never dereference this, just use for comparison

    QVector<QAction*>::iterator it = std::lower_bound(m_actions.begin(), m_actions.end(), reinterpret_cast<QAction*>(object));
    if (it == m_actions.end() || *it != action)
        return;

    const int row = std::distance(m_actions.begin(), it);
    Q_ASSERT(row >= 0 && row < m_actions.size());
    Q_ASSERT(m_actions.at(row) == action);

    beginRemoveRows(QModelIndex(), row, row);
    m_actions.erase(it);
    m_duplicateFinder->remove(action);
    endRemoveRows();
}

int ActionModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return ColumnCount;
}

int ActionModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return m_actions.size();
}

QVariant ActionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  Q_ASSERT(section >= 0);

  if (role == Qt::DisplayRole) {
    switch (section) {
    case AddressColumn:
      return tr("Address");
    case NameColumn:
      return tr("Name");
    case CheckablePropColumn:
      return tr("Checkable");
    case CheckedPropColumn:
      return tr("Checked");
    case PriorityPropColumn:
      return tr("Priority");
    case ShortcutsPropColumn:
      return tr("Shortcut(s)");
    default:
      return QVariant();
    }
  }

  return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant ActionModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  QMutexLocker lock(Probe::objectLock());
  QAction *action = m_actions.at(index.row());
  if (!Probe::instance()->isValidObject(action))
    return QVariant();

  const int column = index.column();
  if (role == Qt::DisplayRole) {
    switch (column) {
    case AddressColumn:
      return Util::addressToString(action);
    case NameColumn:
      return action->text();
    case CheckablePropColumn:
      return action->isCheckable();
    case CheckedPropColumn:
      return VariantHandler::displayString(action->isChecked());
    case PriorityPropColumn:
      return Util::enumToString(action->priority(), 0, action);
    case ShortcutsPropColumn:
      return toString(action->shortcuts());
    default:
      return QVariant();
    }
  } else if (role == Qt::DecorationRole) {
    if (column == NameColumn) {
      return action->icon();
    } else if (column == ShortcutsPropColumn && m_duplicateFinder->hasAmbiguousShortcut(action)) {
      QIcon icon = QIcon::fromTheme("dialog-warning");
      if (!icon.isNull()) {
        return icon;
      } else {
        return QColor(Qt::red);
      }
    }
  } else if (role == Qt::ToolTipRole) {
    if (column == ShortcutsPropColumn && m_duplicateFinder->hasAmbiguousShortcut(action)) {
      return tr("Warning: Ambiguous shortcut detected.");
    }
  } else if (role == ObjectModel::ObjectRole) {
    return QVariant::fromValue<QObject*>(action);
  }

  return QVariant();
}
