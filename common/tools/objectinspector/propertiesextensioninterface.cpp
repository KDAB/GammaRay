/*
  propertiesextensioninterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "propertiesextensioninterface.h"
#include "objectbroker.h"

using namespace GammaRay;

PropertiesExtensionInterface::PropertiesExtensionInterface(const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
    , m_canAddProperty(false)
    , m_hasPropertyValues(true)
{
    ObjectBroker::registerObject(name, this);
}

PropertiesExtensionInterface::~PropertiesExtensionInterface() = default;

const QString &PropertiesExtensionInterface::name() const
{
    return m_name;
}

bool PropertiesExtensionInterface::canAddProperty() const
{
    return m_canAddProperty;
}

void PropertiesExtensionInterface::setCanAddProperty(bool canAdd)
{
    if (m_canAddProperty == canAdd)
        return;
    m_canAddProperty = canAdd;
    emit canAddPropertyChanged();
}

bool PropertiesExtensionInterface::hasPropertyValues() const
{
    return m_hasPropertyValues;
}

void PropertiesExtensionInterface::setHasPropertyValues(bool hasValues)
{
    if (m_hasPropertyValues == hasValues)
        return;
    m_hasPropertyValues = hasValues;
    emit hasPropertyValuesChanged();
}
