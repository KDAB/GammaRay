/*
  connectionfilterproxymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "connectionfilterproxymodel.h"
#include "connectionmodel.h"

using namespace GammaRay;

ConnectionFilterProxyModel::ConnectionFilterProxyModel(QObject *parent)
  : QSortFilterProxyModel(parent),
    m_receiver(0),
    m_sender(0)
{
  setDynamicSortFilter(true);
}

void ConnectionFilterProxyModel::filterReceiver(QObject *receiver)
{
  m_receiver = receiver;
  invalidateFilter();
}

void ConnectionFilterProxyModel::filterSender(QObject *sender)
{
  m_sender = sender;
  invalidateFilter();
}

bool ConnectionFilterProxyModel::filterAcceptsRow(int source_row,
                                                  const QModelIndex &source_parent) const
{
  const QModelIndex sourceIndex = sourceModel()->index(source_row, 0, source_parent);
  if (m_sender &&
      sourceIndex.data(ConnectionModel::SenderRole).value<QObject*>() != m_sender) {
    return false;
  }
  if (m_receiver &&
      sourceIndex.data(ConnectionModel::ReceiverRole).value<QObject*>() != m_receiver) {
    return false;
  }
  return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

bool ConnectionFilterProxyModel::filterAcceptsColumn(int source_column,
                                                     const QModelIndex &source_parent) const
{
  if (m_sender && source_column == 0) {
    return false;
  }
  if (m_receiver && source_column == 2) {
    return false;
  }
  return QSortFilterProxyModel::filterAcceptsColumn(source_column, source_parent);
}

bool ConnectionFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
  const bool leftValid = left.data(ConnectionModel::ConnectionValidRole).toBool();
  const bool rightValid = right.data(ConnectionModel::ConnectionValidRole).toBool();
  if (leftValid == rightValid) {
    return QSortFilterProxyModel::lessThan(left, right);
  } else {
    return rightValid;
  }
}

#include "connectionfilterproxymodel.moc"
