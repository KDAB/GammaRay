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
#include <QDebug>

using namespace GammaRay;

bool operator<(const ProcData &l, const ProcData &r)
{
  return l.ppid < r.ppid;
}

bool operator==(const ProcData &l, const ProcData &r)
{
  return l.ppid == r.ppid;
}

QDebug operator<<(QDebug d, const ProcData &data) {
    d << "ProcData{.ppid=" << data.ppid << ", .name=" << data.name << ", .image=" << data.image
      << ", .state=" << data.state << ", .user=" << data.user << ", .type=" << data.type << "}";
    return d;
}

ProcessModel::ProcessModel(QObject *parent)
: QAbstractTableModel(parent)
{
}

ProcessModel::~ProcessModel()
{
}

void ProcessModel::setProcesses(const ProcDataList &processes)
{
  beginResetModel();
  m_data = processes;
  // sort for merging to work properly
  qStableSort(m_data);
  endResetModel();
}

void ProcessModel::mergeProcesses(const ProcDataList &processes)
{
  // sort like m_data
  ProcDataList sortedProcesses = processes;
  qStableSort(sortedProcesses);

  // iterator over m_data
  int i = 0;

  foreach (const ProcData &newProc, sortedProcesses) {
    bool shouldInsert = true;
    while (i < m_data.count()) {
      const ProcData &oldProc = m_data.at(i);
      if (oldProc < newProc) {
        // remove old proc, seems to be outdated
        beginRemoveRows(QModelIndex(), i, i);
        m_data.removeAt(i);
        endRemoveRows();
        continue;
      } else if (newProc == oldProc) {
        // already contained, hence increment and break
        ++i;
        shouldInsert = false;
        break;
      } else { // newProc < oldProc
        // new entry, break and insert it
        break;
      }
    }
    if (shouldInsert) {
      beginInsertRows(QModelIndex(), i, i);
      m_data.insert(i, newProc);
      endInsertRows();
      // let i point to old element again
      ++i;
    }
  }

  // make sure the new data is properly inserted
  Q_ASSERT(m_data == sortedProcesses);
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

QModelIndex ProcessModel::indexForPid(const QString &pid) const
{
  for (int i = 0; i < m_data.size(); ++i) {
    if (m_data.at(i).ppid == pid) {
      return index(i, 0);
    }
  }
  return QModelIndex();
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

ProcDataList ProcessModel::processes() const
{
  return m_data;
}
