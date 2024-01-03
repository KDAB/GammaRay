/*
  enumsextension.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "enumsextension.h"
#include "propertycontroller.h"
#include "probe.h"
#include <objectenummodel.h>
#include <QMetaProperty>

using namespace GammaRay;

EnumsExtension::EnumsExtension(PropertyController *controller)
    : PropertyControllerExtension(controller->objectBaseName() + ".enums")
    , m_model(new ObjectEnumModel(controller))
{
    controller->registerModel(m_model, QStringLiteral("enums"));
}

EnumsExtension::~EnumsExtension() = default;

bool EnumsExtension::setQObject(QObject *object)
{
    m_model->setMetaObject(object ? object->metaObject() : nullptr);
    return m_model->rowCount() > 0;
}

bool EnumsExtension::setMetaObject(const QMetaObject *metaObject)
{
    m_model->setMetaObject(metaObject);
    return m_model->rowCount() > 0;
}
