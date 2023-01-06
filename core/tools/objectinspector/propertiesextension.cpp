/*
  propertiesextension.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "propertiesextension.h"

#include "aggregatedpropertymodel.h"
#include "propertycontroller.h"
#include "objectinstance.h"
#include <probe.h>
#include <common/propertymodel.h>
#include <QMetaProperty>

using namespace GammaRay;

PropertiesExtension::PropertiesExtension(PropertyController *controller)
    : PropertiesExtensionInterface(controller->objectBaseName() + ".propertiesExtension",
                                   controller)
    , PropertyControllerExtension(controller->objectBaseName() + ".properties")
    , m_aggregatedPropertyModel(new AggregatedPropertyModel(this))
{
    controller->registerModel(m_aggregatedPropertyModel, QStringLiteral("properties"));
}

PropertiesExtension::~PropertiesExtension() = default;

bool PropertiesExtension::setQObject(QObject *object)
{
    if (m_object == object)
        return true;
    m_object = object;
    m_aggregatedPropertyModel->setObject(object);
    setCanAddProperty(true);
    setHasPropertyValues(true);
    return true;
}

bool PropertiesExtension::setObject(void *object, const QString &typeName)
{
    m_object = nullptr;
    m_aggregatedPropertyModel->setObject(ObjectInstance(object, typeName.toUtf8()));
    setCanAddProperty(false);
    setHasPropertyValues(true);
    return true;
}

bool PropertiesExtension::setMetaObject(const QMetaObject *metaObject)
{
    m_object = nullptr;
    m_aggregatedPropertyModel->setObject(ObjectInstance(nullptr, metaObject));
    setCanAddProperty(false);
    setHasPropertyValues(false);
    return true;
}

void PropertiesExtension::setProperty(const QString &name, const QVariant &value)
{
    if (!m_object)
        return;
    m_object->setProperty(name.toUtf8(), value);
}
