/*
  enumrepository.cpp

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

#include "enumrepository.h"

using namespace GammaRay;

EnumRepository::EnumRepository(QObject* parent)
    : QObject(parent)
{
}

EnumRepository::~EnumRepository() = default;

EnumDefinition EnumRepository::definition(EnumId id) const
{
    if (id >= m_definitions.size() || id == InvalidEnumId)
        return EnumDefinition();

    return m_definitions.at(id);
}

void EnumRepository::addDefinition(const EnumDefinition &def)
{
    if (def.id() >= m_definitions.size())
        m_definitions.resize(def.id() + 1);
    m_definitions[def.id()] = def;
}
