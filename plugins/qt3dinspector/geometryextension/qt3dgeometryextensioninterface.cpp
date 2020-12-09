/*
  qt3dgeometryextensioninterface.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <common/streamoperators.h>

#include <QDebug>

using namespace GammaRay;

QT_BEGIN_NAMESPACE
GAMMARAY_ENUM_STREAM_OPERATORS(Qt3DRender::QAttribute::AttributeType)
GAMMARAY_ENUM_STREAM_OPERATORS(Qt3DRender::QAttribute::VertexBaseType)
GAMMARAY_ENUM_STREAM_OPERATORS(Qt3DRender::QBuffer::BufferType)

static QDataStream &operator<<(QDataStream &out, const Qt3DGeometryAttributeData &data)
{
    out << data.name << data.attributeType << data.byteOffset << data.byteStride << data.count
        << data.divisor << data.vertexBaseType << data.vertexSize << data.bufferIndex;
    return out;
}

static QDataStream &operator>>(QDataStream &in, Qt3DGeometryAttributeData &data)
{
    in >> data.name >> data.attributeType >> data.byteOffset >> data.byteStride >> data.count
       >> data.divisor >> data.vertexBaseType >> data.vertexSize >> data.bufferIndex;
    return in;
}
QT_END_NAMESPACE

bool Qt3DGeometryAttributeData::operator==(const Qt3DGeometryAttributeData &rhs) const
{
    return
        name == rhs.name
        && attributeType == rhs.attributeType
        && byteOffset == rhs.byteOffset
        && byteStride == rhs.byteStride
        && count == rhs.count
        && divisor == rhs.divisor
        && vertexBaseType == rhs.vertexBaseType
        && vertexSize == rhs.vertexSize
        && bufferIndex == rhs.bufferIndex;
}

QT_BEGIN_NAMESPACE
static QDataStream &operator<<(QDataStream &out, const Qt3DGeometryBufferData &data)
{
    out << data.name << data.data << data.type;
    return out;
}

static QDataStream &operator>>(QDataStream &in, Qt3DGeometryBufferData &data)
{
    in >> data.name >> data.data >> data.type;
    return in;
}
QT_END_NAMESPACE

bool Qt3DGeometryBufferData::operator==(const Qt3DGeometryBufferData &rhs) const
{
    return name == rhs.name && data == rhs.data;
}

QT_BEGIN_NAMESPACE
static QDataStream &operator<<(QDataStream &out, const Qt3DGeometryData &data)
{
    out << data.attributes << data.buffers;
    return out;
}

static QDataStream &operator>>(QDataStream &in, Qt3DGeometryData &data)
{
    in >> data.attributes >> data.buffers;
    return in;
}
QT_END_NAMESPACE

bool Qt3DGeometryData::operator==(const Qt3DGeometryData &rhs) const
{
    return attributes == rhs.attributes && buffers == rhs.buffers;
}

Qt3DGeometryExtensionInterface::Qt3DGeometryExtensionInterface(const QString &name, QObject *parent)
    : QObject(parent)
{
    StreamOperators::registerOperators<Qt3DGeometryData>();
    ObjectBroker::registerObject(name, this);
}

Qt3DGeometryExtensionInterface::~Qt3DGeometryExtensionInterface()
{
}

Qt3DGeometryData Qt3DGeometryExtensionInterface::geometryData() const
{
    return m_data;
}

void Qt3DGeometryExtensionInterface::setGeometryData(const Qt3DGeometryData &data)
{
    if (m_data == data)
        return;
    m_data = data;
    emit geometryDataChanged();
}
