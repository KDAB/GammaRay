/*
  aggregatedpropertymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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
  connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(sourceDataChanged(QModelIndex,QModelIndex)));
  // TODO: connect signals
  endResetModel();
}

QVariant AggregatedPropertyModel::data(const QModelIndex& index, int role) const
{
  const QModelIndex sourceIndex = mapToSource(index);
  if (!sourceIndex.isValid())
    return QVariant();

  return sourceIndex.data(role);
}

bool AggregatedPropertyModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  const QModelIndex sourceIndex = mapToSource(index);
  if (!sourceIndex.isValid())
    return false;

  return const_cast<QAbstractItemModel*>(sourceIndex.model())->setData(sourceIndex, value, role);
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
  const QModelIndex sourceIndex = mapToSource(index);
  if (!sourceIndex.isValid())
    return QAbstractTableModel::flags(QModelIndex());

  return sourceIndex.flags();
}

QVariant AggregatedPropertyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (m_models.isEmpty())
    return QVariant();

  return m_models.first()->headerData(section, orientation, role);
}

void AggregatedPropertyModel::sourceModelReset()
{
  reset();
}

void AggregatedPropertyModel::sourceDataChanged(const QModelIndex& sourceTopLeft, const QModelIndex& sourceBottomRight)
{
  const QModelIndex topLeft = mapFromSource(sourceTopLeft);
  const QModelIndex bottomRight = mapFromSource(sourceBottomRight);
  if (topLeft.isValid() && bottomRight.isValid())
    emit dataChanged(topLeft, bottomRight);
}

QModelIndex AggregatedPropertyModel::mapToSource(const QModelIndex& aggregatedIndex) const
{
  if (!aggregatedIndex.isValid())
    return QModelIndex();

  int row = aggregatedIndex.row();
  foreach (QAbstractItemModel *model, m_models) {
    if (row < model->rowCount()) {
      return model->index(row, aggregatedIndex.column());
    } else {
      row -= model->rowCount();
    }
  }

  return QModelIndex();
}

QModelIndex AggregatedPropertyModel::mapFromSource(const QModelIndex& sourceIndex) const
{
  int count = 0;
  foreach (QAbstractItemModel* model, m_models) {
    if (model == sourceIndex.model()) {
      return index(count + sourceIndex.row(), sourceIndex.column());
    } else {
      count += model->rowCount();
    }
  }
  return QModelIndex();
}

QMap<int, QVariant> AggregatedPropertyModel::itemData(const QModelIndex& index) const
{
  const QModelIndex sourceIndex = mapToSource(index);
  if (index.isValid())
    return sourceIndex.model()->itemData(sourceIndex);
  return QMap<int, QVariant>();
}
