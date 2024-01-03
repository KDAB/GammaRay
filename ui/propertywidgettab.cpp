/*
  propertywidgettab.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "propertywidgettab.h"

using namespace GammaRay;

PropertyWidgetTabFactoryBase::PropertyWidgetTabFactoryBase(const QString &name,
                                                           const QString &label, int priority)
    : m_name(name)
    , m_label(label)
    , m_priority(priority)
{
}

PropertyWidgetTabFactoryBase::~PropertyWidgetTabFactoryBase() = default;

QString PropertyWidgetTabFactoryBase::name() const
{
    return m_name;
}

QString PropertyWidgetTabFactoryBase::label() const
{
    return m_label;
}

int PropertyWidgetTabFactoryBase::priority() const
{
    return m_priority;
}
