/*
  widgetattributeextension.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "widgetattributeextension.h"

#include <core/attributemodel.h>
#include <core/propertycontroller.h>

#include <QWidget>

using namespace GammaRay;

WidgetAttributeExtension::WidgetAttributeExtension(PropertyController *controller)
    : PropertyControllerExtension(controller->objectBaseName() + ".widgetAttributes")
    , m_attributeModel(new AttributeModel<QWidget, Qt::WidgetAttribute>(controller))
{
    m_attributeModel->setAttributeType("WidgetAttribute");
    controller->registerModel(m_attributeModel, QStringLiteral("widgetAttributeModel"));
}

WidgetAttributeExtension::~WidgetAttributeExtension() = default;

bool WidgetAttributeExtension::setQObject(QObject *object)
{
    if (auto widget = qobject_cast<QWidget *>(object)) {
        m_attributeModel->setObject(widget);
        return true;
    }

    m_attributeModel->setObject(nullptr);
    return false;
}
