/*
  qmlbindingextension.cpp

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

#include "qmlbindingextension.h"
#include "qmlbindingmodel.h"

#include <core/propertycontroller.h>

#include <private/qqmldata_p.h>

using namespace GammaRay;

QmlBindingExtension::QmlBindingExtension(PropertyController* controller)
    : PropertyControllerExtension(controller->objectBaseName() + ".qmlBindings")
    , m_bindingModel(new QmlBindingModel(controller))
{
    controller->registerModel(m_bindingModel, QStringLiteral("qmlBindingModel"));
}

QmlBindingExtension::~QmlBindingExtension()
{
}

bool QmlBindingExtension::setQObject(QObject* object)
{
    if (!object)
        return false;

#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    auto data = QQmlData::get(object);
    if (!data)
        return false;

    m_bindingModel->setObject(object);
    return true;
#else
    return false;
#endif
}
