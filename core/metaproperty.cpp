/*
  metaproperty.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "metaproperty.h"
#include "metaobject.h"

#include <QVariant>

using namespace GammaRay;

MetaProperty::MetaProperty(const char *name)
    : m_class(nullptr)
    , m_name(name)
{
}

MetaProperty::~MetaProperty() = default;

const char *MetaProperty::name() const
{
    return m_name;
}

void MetaProperty::setValue(void *object, const QVariant &value)
{
    Q_UNUSED(object);
    Q_UNUSED(value);
    Q_ASSERT(isReadOnly()); // otherwise sub-class should have implement this...
}

MetaObject *MetaProperty::metaObject() const
{
    Q_ASSERT(m_class);
    return m_class;
}

void MetaProperty::setMetaObject(MetaObject *om)
{
    m_class = om;
}
