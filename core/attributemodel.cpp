/*
  attributemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "attributemodel.h"

using namespace GammaRay;

AbstractAttributeModel::AbstractAttributeModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

AbstractAttributeModel::~AbstractAttributeModel() = default;

void AbstractAttributeModel::setAttributeType(const char *name)
{
    beginResetModel();
    const auto idx = staticQtMetaObject.indexOfEnumerator(name);
    Q_ASSERT(idx >= 0);
    m_attrs = staticQtMetaObject.enumerator(idx);
    endResetModel();
}

int AbstractAttributeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

int AbstractAttributeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !m_attrs.isValid())
        return 0;
    return m_attrs.keyCount() - 1; // skip AttributeCount
}

QVariant AbstractAttributeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_attrs.isValid())
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        return QString::fromLatin1(m_attrs.valueToKey(m_attrs.value(index.row()))).mid(3);
    case Qt::CheckStateRole:
        return testAttribute(m_attrs.value(index.row())) ? Qt::Checked : Qt::Unchecked;
    }

    return QVariant();
}

QVariant AbstractAttributeModel::headerData(int section, Qt::Orientation orientation,
                                            int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return tr("Attribute");
    return QAbstractTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags AbstractAttributeModel::flags(const QModelIndex &index) const
{
    const auto baseFlags = QAbstractTableModel::flags(index);
    if (!index.isValid())
        return baseFlags;
    return baseFlags | Qt::ItemIsUserCheckable;
}

bool AbstractAttributeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || !m_attrs.isValid() || role != Qt::CheckStateRole)
        return false;

    setAttribute(m_attrs.value(index.row()), value.toInt() == Qt::Checked);
    emit dataChanged(index, index);
    return false;
}
