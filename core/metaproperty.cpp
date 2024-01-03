/*
  metaproperty.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
