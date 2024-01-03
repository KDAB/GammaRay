/*
  propertycontrollerinterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "propertycontrollerinterface.h"
#include "objectbroker.h"

using namespace GammaRay;

PropertyControllerInterface::PropertyControllerInterface(const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
{
    ObjectBroker::registerObject(name, this);
}

PropertyControllerInterface::~PropertyControllerInterface() = default;

QString PropertyControllerInterface::name() const
{
    return m_name;
}

QStringList PropertyControllerInterface::availableExtensions() const
{
    return m_availableExtensions;
}

void PropertyControllerInterface::setAvailableExtensions(const QStringList &availableExtensions)
{
    if (m_availableExtensions == availableExtensions)
        return;
    m_availableExtensions = availableExtensions;
    emit availableExtensionsChanged();
}
