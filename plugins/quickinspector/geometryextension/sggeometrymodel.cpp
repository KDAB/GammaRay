/*
  sggeometrymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "sggeometrymodel.h"

#include <QMetaEnum>
#include <QMetaObject>
#include <QSGGeometry>
#include <QSGMaterial>
#include <QSGNode>
#include <QtGui/qopengl.h>

using namespace GammaRay;

Q_DECLARE_METATYPE(QVector<quint16>)
Q_DECLARE_METATYPE(QVector<qint16>)
Q_DECLARE_METATYPE(QVector<int>)
Q_DECLARE_METATYPE(QVector<uint>)
Q_DECLARE_METATYPE(QVector<float>)
Q_DECLARE_METATYPE(QVector<double>)

template<typename T>
static QStringList toStringList(void *data, int size)
{
    QStringList list;
    T *typedData = static_cast<T *>(data);
    for (int i = 0; i < size; i++) {
        list << QString::number(*typedData);
        ++typedData;
    }
    return list;
}

template<typename T>
static QVariantList toVariantList(void *data, int size)
{
    QVariantList list;
    T *typedData = static_cast<T *>(data);
    for (int i = 0; i < size; i++) {
        list << QVariant::fromValue<T>(*typedData);
        ++typedData;
    }
    return list;
}

GammaRay::SGVertexModel::SGVertexModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_geometry(nullptr)
    , m_node(nullptr)
{
}

int SGVertexModel::rowCount(const QModelIndex &parent) const
{
    if (!m_geometry || parent.isValid())
        return 0;

    return m_geometry->vertexCount();
}

int GammaRay::SGVertexModel::columnCount(const QModelIndex &parent) const
{
    if (!m_geometry || parent.isValid())
        return 0;

    return m_geometry->attributeCount();
}

QVariant SGVertexModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()
        || !m_geometry
        || !index.internalPointer()
        || index.row() >= m_geometry->vertexCount()
        || index.column() >= m_geometry->attributeCount())
        return QVariant();

    if (role == Qt::DisplayRole) {
        const QSGGeometry::Attribute *attrInfo = m_geometry->attributes();
        attrInfo += index.column();
        switch (attrInfo->type) {
        case GL_BYTE:
            return toStringList<char>(index.internalPointer(), attrInfo->tupleSize).join(QStringLiteral(
                                                                                             ", "));
        case GL_UNSIGNED_BYTE:
            return toStringList<unsigned char>(index.internalPointer(), attrInfo->tupleSize).join(QStringLiteral(
                                                                                                      ", "));
        case GL_UNSIGNED_SHORT:
            return toStringList<quint16>(index.internalPointer(), attrInfo->tupleSize).join(QStringLiteral(
                                                                                                ", "));
        case GL_SHORT:
            return toStringList<qint16>(index.internalPointer(), attrInfo->tupleSize).join(QStringLiteral(
                                                                                               ", "));
        case GL_INT:
            return toStringList<int>(index.internalPointer(), attrInfo->tupleSize).join(QStringLiteral(
                                                                                            ", "));
        case GL_UNSIGNED_INT:
            return toStringList<uint>(index.internalPointer(), attrInfo->tupleSize).join(QStringLiteral(
                                                                                             ", "));
        case GL_FLOAT:
            return toStringList<float>(index.internalPointer(), attrInfo->tupleSize).join(QStringLiteral(
                                                                                              ", "));
#if defined(GL_DOUBLE) && GL_DOUBLE != GL_FLOAT
        case GL_DOUBLE:
            return toStringList<double>(index.internalPointer(), attrInfo->tupleSize).join(QStringLiteral(
                                                                                               ", "));
#endif
#ifndef QT_OPENGL_ES_2
        case GL_2_BYTES:
            return "2Bytes";
        case GL_3_BYTES:
            return "3Bytes";
        case GL_4_BYTES:
            return "4Bytes";
#endif
        default:
            return QStringLiteral("Unknown %1 byte data: 0x").
                   arg(attrInfo->tupleSize).
                   append(QByteArray((char *)index.internalPointer(), attrInfo->tupleSize).
                          toHex());
        }
    } else if (role == IsCoordinateRole) {
        const QSGGeometry::Attribute *attrInfo = m_geometry->attributes();
        attrInfo += index.column();
        return (bool)attrInfo->isVertexCoordinate;
    } else if (role == RenderRole) {
        const QSGGeometry::Attribute *attrInfo = m_geometry->attributes();
        attrInfo += index.column();
        switch (attrInfo->type) {
        case GL_BYTE:
            return toVariantList<char>(index.internalPointer(), attrInfo->tupleSize);
        case GL_UNSIGNED_BYTE:
            return toVariantList<unsigned char>(index.internalPointer(), attrInfo->tupleSize);
        case GL_UNSIGNED_SHORT:
            return toVariantList<quint16>(index.internalPointer(), attrInfo->tupleSize);
        case GL_SHORT:
            return toVariantList<qint16>(index.internalPointer(), attrInfo->tupleSize);
        case GL_INT:
            return toVariantList<int>(index.internalPointer(), attrInfo->tupleSize);
        case GL_UNSIGNED_INT:
            return toVariantList<uint>(index.internalPointer(), attrInfo->tupleSize);
        case GL_FLOAT:
            return toVariantList<float>(index.internalPointer(), attrInfo->tupleSize);
#if defined(GL_DOUBLE) && GL_DOUBLE != GL_FLOAT
        case GL_DOUBLE:
            return toVariantList<double>(index.internalPointer(), attrInfo->tupleSize);
#endif
        default:
            return QVariant();
        }
    }

    return QVariant();
}

QMap< int, QVariant > SGVertexModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> map = QAbstractItemModel::itemData(index);
    map.insert(IsCoordinateRole, data(index, IsCoordinateRole));
    map.insert(RenderRole, data(index, RenderRole));
    return map;
}

QVariant SGVertexModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal && m_geometry) {
        char const * const *attributeNames = m_node->material()->createShader()->attributeNames();

        for (int i = 0; i <= section; i++) {
            if (!attributeNames[i])
                break;
            if (i == section)
                return attributeNames[section];
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

void SGVertexModel::setNode(QSGGeometryNode *node)
{
    beginResetModel();
    m_node = node;
    m_geometry = node->geometry();
    endResetModel();
}

QModelIndex GammaRay::SGVertexModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!m_geometry
        || row >= m_geometry->vertexCount()
        || column >= m_geometry->attributeCount()
        || parent.isValid())
        return {};

    char *attr = static_cast<char *>(m_geometry->vertexData());
    attr += m_geometry->sizeOfVertex() * row;
    const QSGGeometry::Attribute *attrInfo = m_geometry->attributes();
    int tupleItemSize = 0;

    switch (attrInfo->type) {
    case GL_BYTE:
        tupleItemSize = sizeof(char);
        break;
    case GL_UNSIGNED_BYTE:
        tupleItemSize = sizeof(unsigned char);
        break;
    case GL_UNSIGNED_SHORT:
        tupleItemSize = sizeof(unsigned short);
        break;
    case GL_SHORT:
        tupleItemSize = sizeof(short);
        break;
    case GL_INT:
        tupleItemSize = sizeof(int);
        break;
    case GL_UNSIGNED_INT:
        tupleItemSize = sizeof(unsigned int);
        break;
    case GL_FLOAT:
        tupleItemSize = sizeof(float);
        break;
#if defined(GL_DOUBLE) && GL_DOUBLE != GL_FLOAT
    case GL_DOUBLE:
        tupleItemSize = sizeof(double);
        break;
#endif
    default:
        return createIndex(row, column);
    }

    for (int i = 0; i < column; i++) {
        attr += tupleItemSize * attrInfo->tupleSize;
        attrInfo++;
    }

    return createIndex(row, column, attr);
}


GammaRay::SGAdjacencyModel::SGAdjacencyModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_geometry(nullptr)
    , m_node(nullptr)
{
}

int SGAdjacencyModel::rowCount(const QModelIndex &parent) const
{
    if (!m_geometry || parent.isValid())
        return 0;

    return m_geometry->indexCount();
}

QVariant SGAdjacencyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()
        || !m_geometry
        || index.row() >= m_geometry->indexCount()
        || index.column() != 0)
        return QVariant();

    if (role == DrawingModeRole) {
        return m_geometry->drawingMode();
    }
    if (role == RenderRole) {
        int indexType = m_geometry->indexType();

        if (indexType == GL_UNSIGNED_INT)
            return *(static_cast<const quint32 *>(m_geometry->indexData()) + index.row());
        else if (indexType == GL_UNSIGNED_SHORT)
            return *(static_cast<const quint16 *>(m_geometry->indexData()) + index.row());
        else if (indexType == GL_UNSIGNED_BYTE)
            return *(static_cast<const quint8 *>(m_geometry->indexData()) + index.row());
    }

    return QVariant();
}

void SGAdjacencyModel::setNode(QSGGeometryNode *node)
{
    beginResetModel();
    m_node = node;
    m_geometry = node->geometry();
    endResetModel();
}

QMap< int, QVariant > SGAdjacencyModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> map = QAbstractItemModel::itemData(index);
    map.insert(DrawingModeRole, data(index, DrawingModeRole));
    map.insert(RenderRole, data(index, RenderRole));
    return map;
}
