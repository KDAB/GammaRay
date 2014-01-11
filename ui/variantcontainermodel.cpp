/*
  variantcontainermodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#include "variantcontainermodel.h"

using namespace GammaRay;

VariantContainerModel::VariantContainerModel(QObject *parent)
  : QAbstractTableModel(parent)
{
}

void VariantContainerModel::setVariant(const QVariant &variant)
{
  beginResetModel();
  m_variant = variant;
  endResetModel();
}

int VariantContainerModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid() || (!m_variant.canConvert<QVariantList>()
      && !m_variant.canConvert<QVariantHash>())) {
    return 0;
  }
  if (m_variant.canConvert<QVariantList>()) {
    QSequentialIterable iter = m_variant.value<QSequentialIterable>();
    return iter.size();
  }
  QAssociativeIterable iter = m_variant.value<QAssociativeIterable>();
  return iter.size();
}

int VariantContainerModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid() || (!m_variant.canConvert<QVariantList>()
      && !m_variant.canConvert<QVariantHash>())) {
    return 0;
  }
  if (m_variant.canConvert<QVariantList>()) {
    return 1;
  }
  if (m_variant.canConvert<QVariantHash>()) {
    return 2;
  }
  return 0;
}

QVariant VariantContainerModel::data(const QModelIndex &index, int role) const
{
  if ((!m_variant.canConvert<QVariantList>()
      && !m_variant.canConvert<QVariantHash>()) || !index.isValid()) {
    return QVariant();
  }

  if (m_variant.canConvert<QVariantList>()) {
    QSequentialIterable iter = m_variant.value<QSequentialIterable>();

    if (role == Qt::DisplayRole) {
        return iter.at(index.row());
    }
  }
  if (m_variant.canConvert<QVariantHash>()) {
    QAssociativeIterable iter = m_variant.value<QAssociativeIterable>();

    QAssociativeIterable::const_iterator it = iter.begin() + index.row();

    if (role == Qt::DisplayRole) {
        if (index.column() == 0)
          return it.key();
        if (index.column() == 1)
          return it.value();
    }
  }
  return QVariant();
}

