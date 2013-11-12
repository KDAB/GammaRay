/*
  actionmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#include <core/varianthandler.h>

#include <QAction>
#include <QDebug>

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
  : ObjectFilterProxyModelBase(parent),
  m_duplicateFinder(new ActionValidator(this))
{
  connect(this, SIGNAL(rowsInserted(QModelIndex,int,int)),
    SLOT(handleRowsInserted(QModelIndex,int,int)));

  connect(this, SIGNAL(rowsRemoved(QModelIndex,int,int)),
    SLOT(handleRowsRemoved(QModelIndex,int,int)));

  connect(this, SIGNAL(modelReset()), SLOT(handleModelReset()));
}

ActionModel::~ActionModel()
{
}

QAction *ActionModel::actionForIndex(const QModelIndex &index) const
{
  QObject *object = index.data(ObjectModel::ObjectRole).value<QObject*>();
  Q_ASSERT(object);
  QAction *action = qobject_cast<QAction*>(object);
  return action;
}

QList<QAction *> ActionModel::actions(const QModelIndex &parent, int start, int end)
{
  QList<QAction *> actions;
  for (int i = start; i < end; ++i) {
    const QModelIndex modelIndex = index(i, 0, parent);
    actions << actionForIndex(modelIndex);
  }
  return actions;
}

void ActionModel::handleModelReset()
{
  m_duplicateFinder->setActions(actions());
}

void ActionModel::handleRowsInserted(const QModelIndex &parent, int start, int end)
{
  Q_FOREACH (QAction *action, actions(parent, start, end)) {
    m_duplicateFinder->insert(action);
  }
}

void ActionModel::handleRowsRemoved(const QModelIndex &parent, int start, int end)
{
  Q_FOREACH (QAction *action, actions(parent, start, end)) {
    m_duplicateFinder->remove(action);
  }
}

QList<QAction*> ActionModel::actions() const
{
  QList<QAction*> actions;
  for (int i = 0; i < rowCount(); ++i) {
    const QModelIndex &modelIndex = index(i, 0);
    QAction *action = actionForIndex(modelIndex);
    actions << action;
  }
  return actions;
}

int ActionModel::sourceColumnCount(const QModelIndex &parent) const
{
  return sourceModel()->columnCount(mapToSource(parent));
}

int ActionModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return ColumnCount;
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

  return ObjectFilterProxyModelBase::headerData(section, orientation, role);
}

QVariant ActionModel::data(const QModelIndex &proxyIndex, int role) const
{
  const QModelIndex sourceIndex = mapToSource(proxyIndex.sibling(proxyIndex.row(), 0));
  QAction *action = actionForIndex(sourceIndex);
  if (!action) {
    return QVariant();
  }

  const int column = proxyIndex.column();
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
      return VariantHandler::displayString(action->priority());
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
  }

  return ObjectFilterProxyModelBase::data(proxyIndex, role);
}

Qt::ItemFlags ActionModel::flags(const QModelIndex &index) const
{
  if (index.column() >= sourceColumnCount(index.parent())) {
    return
      QSortFilterProxyModel::flags(index.sibling(index.row(), 0)) &
      (Qt::ItemIsSelectable | Qt::ItemIsDragEnabled |
       Qt::ItemIsDropEnabled | Qt::ItemIsEnabled);
  }

  return QSortFilterProxyModel::flags(index);
}

QModelIndex ActionModel::index(int row, int column, const QModelIndex &parent) const
{
  if (!hasIndex(row, column, parent)) {
    return QModelIndex();
  }

  int sourceColumn = column;
  if (column >= sourceColumnCount(parent)) {
    sourceColumn = 0;
  }

  QModelIndex i = QSortFilterProxyModel::index(row, sourceColumn, parent);
  return createIndex(i.row(), column, i.internalPointer());
}

bool ActionModel::filterAcceptsObject(QObject *object) const
{
  return qobject_cast<QAction*>(object);
}
