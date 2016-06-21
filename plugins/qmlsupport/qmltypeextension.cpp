/*
  qmltypeextension.cpp

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

#include "qmltypeextension.h"

#include <core/aggregatedpropertymodel.h>
#include <core/objectinstance.h>
#include <core/propertycontroller.h>

#include <private/qqmlcompiler_p.h>
#include <private/qqmldata_p.h>
#include <private/qqmlmetatype_p.h>

using namespace GammaRay;

QmlTypeExtension::QmlTypeExtension(PropertyController *controller)
    : PropertyControllerExtension(controller->objectBaseName() + ".qmlType")
    , m_typePropertyModel(new AggregatedPropertyModel(controller))
{
    controller->registerModel(m_typePropertyModel, QStringLiteral("qmlTypeModel"));
}

QmlTypeExtension::~QmlTypeExtension()
{
}

bool QmlTypeExtension::setQObject(QObject *object)
{
    if (!object)
        return false;
    // C++ QML type
    if (setMetaObject(object->metaObject()))
        return true;

    // QML-defined type
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    auto data = QQmlData::get(object);
    if (!data || !data->compiledData)
        return false;

    const auto qmlType = QQmlMetaType::qmlType(data->compiledData->url());
    if (!qmlType)
        return false;

    m_typePropertyModel->setObject(ObjectInstance(qmlType, "QQmlType"));
    return true;
#else
    return false;
#endif
}

bool QmlTypeExtension::setMetaObject(const QMetaObject *metaObject)
{
    if (!metaObject)
        return false;

    const auto qmlType = QQmlMetaType::qmlType(metaObject);
    if (!qmlType)
        return false;

    m_typePropertyModel->setObject(ObjectInstance(qmlType, "QQmlType"));
    return true;
}
