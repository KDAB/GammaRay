/*
  backtracemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "backtracemodel.h"

#include <QDebug>

using namespace GammaRay;



BacktraceModel::BacktraceModel(QObject *parent)
{
}

int BacktraceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_backtrace.count();
}

int BacktraceModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return COLUMN_COUNT;
}

QStringList BacktraceModel::parseStackFrame(QString stackFrame) const
{
  QStringList row;
  for (int i = 0; i < COLUMN_COUNT; ++i)
    row.append(QString());
  QStringList parts = stackFrame.split("::");
  if (parts.count() == 2)
  {
    row[FunctionColumn] = parts.at(1);
    parts = parts.at(0).split(": ");
    if (parts.count() != 2 && parts.count() != 3)
      return row;
    row[ClassColumn] = parts.at(parts.count()-1);
    if (parts.count() == 2)
    {
      parts = parts.at(0).split(" (");
      if (parts.count() != 2)
        return row;
      row[FileColumn] = parts.at(0);
      row[LineColumn] = parts.at(1).left(parts.at(1).count()-1);

    } else
    {
      row[FileColumn] = parts.at(1);
      parts = parts.at(0).split(" (");
      if (parts.count() != 2)
        return row;
      row[AddressColumn] = parts.at(0);
      row[DllColumn] = parts.at(1).left(parts.at(1).count()-1);
    }
  }

  return row;
}

QVariant BacktraceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() > rowCount() || index.column() > columnCount()) {
      return QVariant();
    }
    if (role == Qt::DisplayRole){
        QString stackFrame = m_backtrace.at(index.row());
        QStringList row = parseStackFrame(stackFrame);
        return row.at(index.column());
    }
    return QVariant();
}

QVariant BacktraceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
      if (section == AddressColumn) {
        return tr("Address");
      } else if (section == DllColumn) {
        return tr("Dll");
      } else if (section == FileColumn) {
        return tr("File");
      } else if (section == LineColumn) {
        return tr("Line");
      } else if (section == ClassColumn) {
        return tr("Class");
      } else if (section == FunctionColumn) {
        return tr("Function");
      }
    }

    return QVariant();

}

void BacktraceModel::setBacktrace(QStringList &backtrace)
{
    m_backtrace = backtrace;
    emit layoutChanged ();
}
