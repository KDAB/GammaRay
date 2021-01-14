/*
  widgetattributeextension.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
