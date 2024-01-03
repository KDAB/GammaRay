/*
  widgetattributeextension.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
