/*
  enumdefinition.cpp

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

#include "enumdefinition.h"

using namespace GammaRay;

EnumDefinitionElement::EnumDefinitionElement(int value, const char *name)
    : m_value(value)
    , m_name(name)
{
}

QByteArray EnumDefinitionElement::name() const
{
    return m_name;
}

int EnumDefinitionElement::value() const
{
    return m_value;
}

namespace GammaRay {
QDataStream& operator<<(QDataStream &out, const EnumDefinitionElement &elem)
{
    out << elem.m_value << elem.m_name;
    return out;
}

QDataStream& operator>>(QDataStream &in, EnumDefinitionElement &elem)
{
    in >> elem.m_value >> elem.m_name;
    return in;
}
}

EnumDefinition::EnumDefinition(EnumId id, const QByteArray &name)
    : m_id(id)
    , m_name(name)
{
}

bool EnumDefinition::isValid() const
{
    return m_id != InvalidEnumId && !m_name.isEmpty() && !m_elements.isEmpty();
}

EnumId EnumDefinition::id() const
{
    return m_id;
}

QByteArray EnumDefinition::name() const
{
    return m_name;
}

bool EnumDefinition::isFlag() const
{
    return m_isFlag;
}

void EnumDefinition::setIsFlag(bool isFlag)
{
    m_isFlag = isFlag;
}

QVector<EnumDefinitionElement> EnumDefinition::elements() const
{
    return m_elements;
}

void EnumDefinition::setElements(const QVector<EnumDefinitionElement>& elements)
{
    m_elements = elements;
}

QByteArray EnumDefinition::valueToString(const EnumValue& value) const
{
    Q_ASSERT(value.id() == id());
    if (isFlag()) {
        QByteArray r;
        int handledFlags = 0;
        for (const auto &elem : m_elements) {
            if ((elem.value() & value.value()) == elem.value() && elem.value() != 0) {
                r += elem.name() + '|';
                handledFlags |= elem.value();
            }
        }
        if (value.value() & ~handledFlags)
            r += "flag 0x" + QByteArray::number(value.value() & ~handledFlags, 16) + '|';

        if (!r.isEmpty()) {
            r.chop(1);
        } else {
            // check for dedicated 0-values
            Q_ASSERT(value.value() == 0);
            for (const auto &elem : m_elements) {
                if (elem.value() == 0)
                    return elem.name();
            }
            return "<none>";
        }
        return r;
    } else {
        for (const auto &elem : m_elements) {
            if (elem.value() == value.value())
                return elem.name();
        }
        return "unknown (" + QByteArray::number(value.value()) + ')';
    }
}

namespace GammaRay {
QDataStream& operator<<(QDataStream &out, const EnumDefinition &def)
{
    out << def.m_id << def.m_isFlag << def.m_name << def.m_elements;
    return out;
}

QDataStream& operator>>(QDataStream &in, EnumDefinition &def)
{
    in >>  def.m_id >> def.m_isFlag >> def.m_name >> def.m_elements;
    return in;
}

}
