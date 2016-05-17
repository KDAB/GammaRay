/*
  buffermodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "buffermodel.h"
#include "attribute.h"

#include <QDebug>

using namespace GammaRay;

BufferModel::BufferModel(QObject* parent) :
    QAbstractTableModel(parent),
    m_bufferIndex(-1),
    m_rowSize(0)
{
}

BufferModel::~BufferModel()
{
}

void BufferModel::setGeometryData(const GammaRay::Qt3DGeometryData& data)
{
    beginResetModel();
    m_data = data;
    m_bufferIndex = 0;
    updateAttributes();
    endResetModel();
}

void BufferModel::setBufferIndex(int index)
{
    beginResetModel();
    m_bufferIndex = index;
    updateAttributes();
    endResetModel();
}

void BufferModel::updateAttributes()
{
    m_attrs.clear();
    m_buffer.clear();

    if (m_data.buffers.isEmpty() || m_bufferIndex < 0)
        return;

    Q_ASSERT(m_data.buffers.size() >= m_bufferIndex);
    m_buffer = m_data.buffers.at(m_bufferIndex).data;

    if (m_data.vertexPositions.bufferIndex == (uint)m_bufferIndex)
        updateAttribute(m_data.vertexPositions);
    if (m_data.vertexNormals.bufferIndex == (uint)m_bufferIndex)
        updateAttribute(m_data.vertexNormals);
    if (m_data.index.bufferIndex == (uint)m_bufferIndex)
        updateAttribute(m_data.index);

    std::sort(m_attrs.begin(), m_attrs.end(), [](const ColumnData &lhs, const ColumnData &rhs) {
        return lhs.offset < rhs.offset;
    });
}

void BufferModel::updateAttribute(const GammaRay::Qt3DGeometryAttributeData& attrData)
{
    m_rowSize = m_buffer.size() / attrData.count;
    for (uint i = 0; i < std::max(attrData.vertexSize, 1u); ++i) {
        ColumnData col;
        col.name = attrData.name;
        if (attrData.vertexSize > 1)
            col.name += QLatin1Char('[') + QString::number(i) + QLatin1Char(']');
        col.offset = attrData.byteOffset + i * Attribute::size(attrData.vertexBaseType);
        col.type = attrData.vertexBaseType;
        m_attrs.push_back(col);
    }
}

int BufferModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_attrs.size();
}

int BufferModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid() || m_attrs.isEmpty())
        return 0;
    return m_buffer.size() / m_rowSize;
}

QVariant BufferModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || m_attrs.isEmpty())
        return QVariant();

    if (role == Qt::DisplayRole) {
        const char *c = m_buffer.constData() + (m_rowSize * index.row()) + m_attrs.at(index.column()).offset;
        return Attribute::variant(m_attrs.at(index.column()).type, c);
    }

    return QVariant();
}

QVariant BufferModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return m_attrs.at(section).name;
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}
