/*
  qt3dgeometryextensioninterface.cpp

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

#include "qt3dgeometryextensioninterface.h"

#include <common/objectbroker.h>

#include <QDataStream>
#include <QDebug>

using namespace GammaRay;

static QDataStream &operator<<(QDataStream &out, Qt3DRender::QAttribute::VertexBaseType type)
{
    out << (quint32)type;
    return out;
}

QDataStream &operator>>(QDataStream &in, Qt3DRender::QAttribute::VertexBaseType &type)
{
    quint32 v;
    in >> v;
    type = static_cast<Qt3DRender::QAttribute::VertexBaseType>(v);
    return in;
}

static QDataStream &operator<<(QDataStream &out, const Qt3DGeometryAttributeData &data)
{
    out << data.byteOffset << data.byteStride << data.count << data.divisor << data.vertexBaseType << data.vertexSize << data.data;
    return out;
}

QDataStream &operator>>(QDataStream &in, Qt3DGeometryAttributeData &data)
{
    in >> data.byteOffset >> data.byteStride >> data.count >> data.divisor >> data.vertexBaseType >> data.vertexSize >> data.data;
    return in;
}

Qt3DGeometryAttributeData::Qt3DGeometryAttributeData() :
    byteOffset(0),
    byteStride(0),
    count(0),
    divisor(0),
    vertexBaseType(Qt3DRender::QAttribute::UnsignedShort),
    vertexSize(0)
{
}

bool Qt3DGeometryAttributeData::operator==(const Qt3DGeometryAttributeData& rhs) const
{
    return
        byteOffset == rhs.byteOffset &&
        byteStride == rhs.byteStride &&
        count == rhs.count &&
        divisor == rhs.divisor &&
        vertexBaseType == rhs.vertexBaseType &&
        vertexSize == rhs.vertexSize &&
        data == rhs.data;
}


static QDataStream &operator<<(QDataStream &out, const Qt3DGeometryData &data)
{
    out << data.vertexPositions << data.vertexNormals << data.index;
    return out;
}

QDataStream &operator>>(QDataStream &in, Qt3DGeometryData &data)
{
    in >> data.vertexPositions >> data.vertexNormals >> data.index;
    return in;
}

bool Qt3DGeometryData::operator==(const Qt3DGeometryData &rhs) const
{
    return vertexPositions == rhs.vertexPositions && vertexNormals == rhs.vertexNormals;
}


Qt3DGeometryExtensionInterface::Qt3DGeometryExtensionInterface(const QString& name, QObject* parent) :
    QObject(parent)
{
    qRegisterMetaType<Qt3DGeometryData>();
    qRegisterMetaTypeStreamOperators<Qt3DGeometryData>();
    ObjectBroker::registerObject(name, this);
}

Qt3DGeometryExtensionInterface::~Qt3DGeometryExtensionInterface()
{
}

Qt3DGeometryData Qt3DGeometryExtensionInterface::geometryData() const
{
    return m_data;
}

void Qt3DGeometryExtensionInterface::setGeometryData(const Qt3DGeometryData& data)
{
    if (m_data == data)
        return;
    m_data = data;
    qDebug();
    emit geometryDataChanged();
}
