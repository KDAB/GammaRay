/*
  attribute.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

// krazy:excludeall=typedefs since we need int8_t and friends in here

#include "attribute.h"

#include <cstdint>

#include <QVariant>

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

int Attribute::size(Qt3DGeometry::QAttribute::VertexBaseType type)
{
    return size_table[type];
}

template<typename T>
static QVariant toVariant(const char *data)
{
    // cppcheck-suppress invalidPointerCast
    return QVariant::fromValue<T>(*reinterpret_cast<const T *>(data));
}

QVariant Attribute::variant(Qt3DGeometry::QAttribute::VertexBaseType type, const char *data)
{
    switch (type) {
    case Qt3DGeometry::QAttribute::Byte:
        return toVariant<int8_t>(data);
    case Qt3DGeometry::QAttribute::UnsignedByte:
        return toVariant<uint8_t>(data);
    case Qt3DGeometry::QAttribute::Short:
        return toVariant<int16_t>(data);
    case Qt3DGeometry::QAttribute::UnsignedShort:
        return toVariant<uint16_t>(data);
    case Qt3DGeometry::QAttribute::Int:
        return toVariant<int32_t>(data);
    case Qt3DGeometry::QAttribute::UnsignedInt:
        return toVariant<uint32_t>(data);
    case Qt3DGeometry::QAttribute::HalfFloat:
        return QVariant("TODO");
    case Qt3DGeometry::QAttribute::Float:
        return toVariant<float>(data);
    case Qt3DGeometry::QAttribute::Double:
        return toVariant<double>(data);
    }
    return QVariant();
}
