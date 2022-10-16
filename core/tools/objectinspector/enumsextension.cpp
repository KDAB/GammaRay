/*
  enumsextension.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
