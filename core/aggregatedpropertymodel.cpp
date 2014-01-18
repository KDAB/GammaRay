/*
  aggregatedpropertymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "aggregatedpropertymodel.h"

using namespace GammaRay;

AggregatedPropertyModel::AggregatedPropertyModel(QObject* parent) : QAbstractTableModel(parent)
{
}

AggregatedPropertyModel::~AggregatedPropertyModel()
{
}

void AggregatedPropertyModel::addModel(QAbstractItemModel* model)
{
  beginResetModel(); // FIXME: use insertRows instead
  m_models.append(model);
  connect(model, SIGNAL(modelReset()), this, SLOT(sourceModelReset()));
  // TODO: connect signals
  endResetModel();
}

QVariant AggregatedPropertyModel::data(const QModelIndex& index, int role) const
{
  return QVariant();
}

int AggregatedPropertyModel::columnCount(const QModelIndex& parent) const
{
  if (parent.isValid() || m_models.isEmpty())
    return 0;
  return m_models.first()->columnCount();
}

int AggregatedPropertyModel::rowCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return 0;

  int count = 0;
  foreach (QAbstractItemModel* model, m_models)
    count += model->rowCount();

  return count;
}

Qt::ItemFlags AggregatedPropertyModel::flags(const QModelIndex& index) const
{
  return QAbstractItemModel::flags(index);
}

QVariant AggregatedPropertyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  return QAbstractItemModel::headerData(section, orientation, role);
}

void AggregatedPropertyModel::sourceModelReset()
{
  reset();
}
