/*
  backtracemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Gábor Angyal <angyalgabor@outlook.com>

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


#include <tuple>

using namespace std;
using namespace GammaRay;



BacktraceModel::BacktraceModel(QObject *parent)
{
}

int BacktraceModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_data.count();
}

int BacktraceModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return COLUMN_COUNT;
}

//Parses input in "aaa (bbb)" format into a tuple of "aaa" and "bbb".
tuple<QString, QString> parseParenthesis(QString str)
{
  QStringList parts = str.split(" (");
  if (parts.count() != 2)
    return tuple<QString, QString>();
  QString second = parts.at(1).left(parts.at(1).count()-1);
  return make_tuple(parts.at(0), second);
}

//The input either contains a function name only,
//or is in the ClassName::FunctionName format.
tuple<QString, QString> parseClassAndFunctionName(QString str)
{
  QStringList parts = str.split("::");
  if (parts.count() == 2)
    return make_tuple(parts.at(0), parts.at(1));
  return make_tuple(QString(), str);
}

QStringList BacktraceModel::parseStackFrame(QString &stackFrame) const
{
  QStringList row;
  for (int i = 0; i < COLUMN_COUNT; ++i)
    row.append(QString());

  QStringList parts = stackFrame.split(": ");
  if (parts.count() != 2 && parts.count() != 3)
    return row;

  QString functionStr = parts.at(parts.count()-1);
  tie(row[ClassColumn], row[FunctionColumn]) = parseClassAndFunctionName(functionStr);

  if (parts.count() == 2) {
    tie(row[FileColumn], row[LineColumn]) = parseParenthesis(parts.at(0));
  } else {
    row[FileColumn] = parts.at(1);
    tie(row[AddressColumn], row[DllColumn]) = parseParenthesis(parts.at(0));
  }

  return row;
}

QVariant BacktraceModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid() || index.row() > rowCount() || index.column() > columnCount()) {
    return QVariant();
  }
  if (role == Qt::DisplayRole) {
    return m_data.at(index.row()).at(index.column());
  }
  return QVariant();
}

QVariant BacktraceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
    switch (section) {
      case AddressColumn:
        return tr("Address");
      case DllColumn:
        return tr("Dll");
      case FileColumn:
        return tr("File");
      case LineColumn:
        return tr("Line");
      case ClassColumn:
        return tr("Class");
      case FunctionColumn:
        return tr("Function");
    }
  }

  return QVariant();
}

void BacktraceModel::setBacktrace(Backtrace &backtrace)
{
  m_data.clear();
  foreach (QString stackFrame, backtrace) {
    m_data.push_back(parseStackFrame(stackFrame));
  }

  emit layoutChanged ();
}
