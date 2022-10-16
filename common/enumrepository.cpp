/*
  enumrepository.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "enumrepository.h"

using namespace GammaRay;

EnumRepository::EnumRepository(QObject *parent)
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
