/*
  bindingextension.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "bindingextension.h"
#include "qmlbindingmodel.h"

#include <core/propertycontroller.h>

using namespace GammaRay;

BindingExtension::BindingExtension(PropertyController* controller)
    : PropertyControllerExtension(controller->objectBaseName() + ".bindings")
    , m_bindingModel(new QmlBindingModel(controller))
{
    controller->registerModel(m_bindingModel, QStringLiteral("bindingModel"));
}

BindingExtension::~BindingExtension()
{
}

bool BindingExtension::setQObject(QObject* object)
{
    if (!object)
        return false;

    return m_bindingModel->setObject(object);
}
