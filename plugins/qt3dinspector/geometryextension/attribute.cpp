/*
  attribute.cpp

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

// krazy:excludeall=typedefs since we need int8_t and friends in here

#include "attribute.h"

#include <cstdint>

using namespace GammaRay;

// ### careful: order must match qattribute.h
static const int size_table[] = {
    sizeof(int8_t), // Byte
    sizeof(uint8_t), // UnsignedByte
    sizeof(int16_t), // Short
    sizeof(uint16_t), // UnsignedShort
    sizeof(int32_t), // Int
    sizeof(uint32_t), // UnsignedInt
    16, // HalfFloat
    sizeof(float), // Float
    sizeof(double), // Double
};

int Attribute::size(Qt3DRender::QAttribute::VertexBaseType type)
{
    return size_table[type];
}

template<typename T> static QVariant toVariant(const char *data)
{
    // cppcheck-suppress invalidPointerCast
    return QVariant::fromValue<T>(*reinterpret_cast<const T *>(data));
}

QVariant Attribute::variant(Qt3DRender::QAttribute::VertexBaseType type, const char *data)
{
    switch (type) {
    case Qt3DRender::QAttribute::Byte:
        return toVariant<int8_t>(data);
    case Qt3DRender::QAttribute::UnsignedByte:
        return toVariant<uint8_t>(data);
    case Qt3DRender::QAttribute::Short:
        return toVariant<int16_t>(data);
    case Qt3DRender::QAttribute::UnsignedShort:
        return toVariant<uint16_t>(data);
    case Qt3DRender::QAttribute::Int:
        return toVariant<int32_t>(data);
    case Qt3DRender::QAttribute::UnsignedInt:
        return toVariant<uint32_t>(data);
    case Qt3DRender::QAttribute::HalfFloat:
        return QVariant("TODO");
    case Qt3DRender::QAttribute::Float:
        return toVariant<float>(data);
    case Qt3DRender::QAttribute::Double:
        return toVariant<double>(data);
    }
    return QVariant();
}
