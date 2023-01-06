/*
  enumvalue.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "enumvalue.h"

using namespace GammaRay;

EnumValue::EnumValue(EnumId id, int value)
    : m_id(id)
    , m_value(value)
{
}

EnumId EnumValue::id() const
{
    return m_id;
}

int EnumValue::value() const
{
    return m_value;
}

void EnumValue::setValue(int value)
{
    m_value = value;
}

bool EnumValue::isValid() const
{
    return m_id != InvalidEnumId;
}

namespace GammaRay {
QDataStream &operator<<(QDataStream &out, const EnumValue &v)
{
    out << v.m_id << v.m_value;
    return out;
}

QDataStream &operator>>(QDataStream &in, EnumValue &v)
{
    in >> v.m_id >> v.m_value;
    return in;
}
}
