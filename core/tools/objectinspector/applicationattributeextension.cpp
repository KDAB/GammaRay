/*
  applicationattributeextension.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "applicationattributeextension.h"

#include <core/attributemodel.h>
#include <core/propertycontroller.h>

#include <QCoreApplication>

using namespace GammaRay;

ApplicationAttributeExtension::ApplicationAttributeExtension(
    GammaRay::PropertyController *controller)
    : PropertyControllerExtension(controller->objectBaseName() + ".applicationAttributes")
    , m_attributeModel(new AttributeModel<QCoreApplication, Qt::ApplicationAttribute>(controller))
{
    m_attributeModel->setAttributeType("ApplicationAttribute");
    controller->registerModel(m_attributeModel, QStringLiteral("applicationAttributeModel"));
}

ApplicationAttributeExtension::~ApplicationAttributeExtension() = default;

bool ApplicationAttributeExtension::setQObject(QObject *object)
{
    if (auto app = qobject_cast<QCoreApplication *>(object)) {
        m_attributeModel->setObject(app);
        return true;
    }

    m_attributeModel->setObject(nullptr);
    return false;
}
