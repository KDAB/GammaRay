/*
  sggeometrymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

GammaRay::SGGeometryModel::SGGeometryModel(QObject *parent)
  : QAbstractTableModel(parent), m_geometry(0), m_node(0)
{
}

int SGGeometryModel::rowCount(const QModelIndex &parent) const
{
  if (!m_geometry || parent.isValid()) {
    return 0;
  }

  return m_geometry->vertexCount();
}

int GammaRay::SGGeometryModel::columnCount(const QModelIndex &parent) const
{
  if (!m_geometry || parent.isValid()) {
    return 0;
  }

  return m_geometry->attributeCount();
}

QVariant SGGeometryModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid() ||
      !m_geometry ||
      !index.internalPointer() ||
      index.row() >= m_geometry->vertexCount() ||
      index.column() >= m_geometry->attributeCount()) {
    return QVariant();
  }

  if (role == Qt::DisplayRole) {
    const QSGGeometry::Attribute *attrInfo = m_geometry->attributes();
    attrInfo += index.column();
    switch (attrInfo->type) {
    case GL_BYTE:
      return toStringList<char>(index.internalPointer(), attrInfo->tupleSize).join(", ");
    case GL_UNSIGNED_BYTE:
      return toStringList<unsigned char>(index.internalPointer(), attrInfo->tupleSize).join(", ");
    case GL_UNSIGNED_SHORT:
      return toStringList<quint16>(index.internalPointer(), attrInfo->tupleSize).join(", ");
    case GL_SHORT:
      return toStringList<qint16>(index.internalPointer(), attrInfo->tupleSize).join(", ");
    case GL_INT:
      return toStringList<int>(index.internalPointer(), attrInfo->tupleSize).join(", ");
    case GL_UNSIGNED_INT:
      return toStringList<uint>(index.internalPointer(), attrInfo->tupleSize).join(", ");
    case GL_FLOAT:
      return toStringList<float>(index.internalPointer(), attrInfo->tupleSize).join(", ");
#if defined(GL_DOUBLE) && GL_DOUBLE != GL_FLOAT
    case GL_DOUBLE:
      return toStringList<double>(index.internalPointer(), attrInfo->tupleSize).join(", ");
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
      return QString("Unknown %1 byte data: 0x").
        arg(attrInfo->tupleSize).
          append(QByteArray((char*)index.internalPointer(), attrInfo->tupleSize).
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

QMap< int, QVariant > SGGeometryModel::itemData(const QModelIndex &index) const
{
  QMap<int, QVariant> map = QAbstractItemModel::itemData(index);
  map.insert(IsCoordinateRole, data(index, IsCoordinateRole));
  map.insert(RenderRole, data(index, RenderRole));
  return map;
}

QVariant SGGeometryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal && m_geometry) {
    const QSGGeometry::Attribute *attrInfo = m_geometry->attributes();
    attrInfo += section;

    char const *const *attributeNames = m_node->material()->createShader()->attributeNames();

    for (int i = 0; i <= section; i++) {
      if (!attributeNames[i]) {
        break;
      }
      if (i == section) {
        return attributeNames[section];
      }
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

void SGGeometryModel::setNode(QSGGeometryNode *node)
{
  beginResetModel();
  m_node = node;
  m_geometry = node->geometry();
  endResetModel();
}

QModelIndex GammaRay::SGGeometryModel::index(int row, int column, const QModelIndex &parent) const
{
  if (!m_geometry ||
      row >= m_geometry->vertexCount() ||
      column >= m_geometry->attributeCount() ||
      parent.isValid()) {
    return QModelIndex();
  }

  char *attr = static_cast<char*>(m_geometry->vertexData());
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
