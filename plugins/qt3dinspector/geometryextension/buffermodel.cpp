/*
  buffermodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "buffermodel.h"
#include "attribute.h"

#include <QDebug>

using namespace GammaRay;

BufferModel::BufferModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_bufferIndex(-1)
    , m_rowSize(0)
{
}

BufferModel::~BufferModel()
{
}

void BufferModel::setGeometryData(const GammaRay::Qt3DGeometryData &data)
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
    for (const auto &attr : qAsConst(m_data.attributes)) {
        if (attr.bufferIndex == ( uint )m_bufferIndex)
            updateAttribute(attr);
    }

    std::sort(m_attrs.begin(), m_attrs.end(), [](const ColumnData &lhs, const ColumnData &rhs) {
        return lhs.offset < rhs.offset;
    });
}

void BufferModel::updateAttribute(const GammaRay::Qt3DGeometryAttributeData &attrData)
{
    m_rowSize = m_buffer.size() / attrData.count;
    for (uint i = 0; i < std::max(attrData.vertexSize, 1u); ++i) {
        ColumnData col;
        col.name = attrData.name;
        if (attrData.vertexSize > 1)
            col.name += QLatin1Char('[') + QString::number(i) + QLatin1Char(']');
        col.offset = attrData.byteOffset + i * Attribute::size(attrData.vertexBaseType);
        col.type = attrData.vertexBaseType;
        col.stride = std::max(attrData.byteStride, ( uint )Attribute::size(attrData.vertexBaseType) * attrData.vertexSize);
        m_attrs.push_back(col);
    }
}

int BufferModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_attrs.size();
}

int BufferModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || m_attrs.isEmpty())
        return 0;
    return m_buffer.size() / m_rowSize;
}

QVariant BufferModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || m_attrs.isEmpty())
        return QVariant();

    if (role == Qt::DisplayRole) {
        const auto &attr = m_attrs.at(index.column());
        const char *c = m_buffer.constData() + (attr.stride * index.row()) + attr.offset;
        return Attribute::variant(m_attrs.at(index.column()).type, c);
    }

    return QVariant();
}

QVariant BufferModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return m_attrs.at(section).name;
    else if (orientation == Qt::Vertical && role == Qt::DisplayRole)
        return QString::number(section); // 0-based rather than 1-based
    return QAbstractTableModel::headerData(section, orientation, role);
}
