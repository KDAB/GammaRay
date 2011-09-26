/*
  processmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#include "processmodel.h"

using namespace GammaRay;

ProcessModel::ProcessModel(QObject *parent)
: QAbstractTableModel(parent)
{
}

ProcessModel::~ProcessModel()
{
}

void ProcessModel::addProcess(const ProcData &process)
{
  beginInsertRows(QModelIndex(), m_data.count(), m_data.count());
  m_data << process;
  endInsertRows();
}

void ProcessModel::addProcesses(const QList< ProcData >& processes)
{
  if (processes.isEmpty()) {
    return;
  }
  beginInsertRows(QModelIndex(), m_data.count(), m_data.count() + processes.count() - 1);
  m_data += processes;
  endInsertRows();
}

void ProcessModel::clear()
{
  beginRemoveRows(QModelIndex(), 0, m_data.count());
  m_data.clear();
  endRemoveRows();
}

ProcData ProcessModel::dataForIndex(const QModelIndex &index) const
{
  return m_data.at(index.row());
}

ProcData ProcessModel::dataForRow(int row) const
{
  return m_data.at(row);
}

QVariant ProcessModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
    return QVariant();
  }

  if (section == PIDColumn) {
    return tr("Process ID");
  } else if (section == NameColumn) {
    return tr("Name");
  } else if (section == StateColumn) {
    return tr("State");
  } else if (section == UserColumn) {
    return tr("User");
  }

  return QVariant();
}

QVariant ProcessModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid()) {
    return QVariant();
  }

  const ProcData &data = m_data.at(index.row());

  if (role == Qt::DisplayRole) {
    if (index.column() == PIDColumn) {
      return data.ppid;
    } else if (index.column() == NameColumn) {
      return data.image.isEmpty() ? data.name : data.image;
    } else if (index.column() == StateColumn) {
      return data.state;
    } else if (index.column() == UserColumn) {
      return data.user;
    }
  } else if (role == PIDRole) {
    return data.ppid;
  } else if (role == NameRole) {
    return data.image.isEmpty() ? data.name : data.image;
  } else if (role == StateRole) {
    return data.state;
  } else if (role == UserRole) {
    return data.user;
  }

  return QVariant();
}

int ProcessModel::columnCount(const QModelIndex &parent) const
{
  return parent.isValid() ? 0 : COLUMN_COUNT;
}

int ProcessModel::rowCount(const QModelIndex &parent) const
{
  return parent.isValid() ? 0 : m_data.count();
}

