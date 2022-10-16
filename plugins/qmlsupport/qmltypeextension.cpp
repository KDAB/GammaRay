/*
  qmltypeextension.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "qmltypeextension.h"
#include "qmltypeutil.h"

#include <core/aggregatedpropertymodel.h>
#include <core/objectinstance.h>
#include <core/propertycontroller.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)
#include <private/qqmlcompiler_p.h>
#else
#include <private/qv4compileddata_p.h>
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
#include <private/qv4executablecompilationunit_p.h> // created in qtdeclarative commit fd6321c03e2d63997078bfa41332dbddefbb86b0
#endif
#endif
#include <private/qqmldata_p.h>
#include <private/qqmlmetatype_p.h>

using namespace GammaRay;

QmlTypeExtension::QmlTypeExtension(PropertyController *controller)
    : PropertyControllerExtension(controller->objectBaseName() + ".qmlType")
    , m_typePropertyModel(new AggregatedPropertyModel(controller))
{
    controller->registerModel(m_typePropertyModel, QStringLiteral("qmlTypeModel"));
}

QmlTypeExtension::~QmlTypeExtension() = default;

bool QmlTypeExtension::setQObject(QObject *object)
{
    if (!object)
        return false;
    // C++ QML type
    if (setMetaObject(object->metaObject()))
        return true;

    // QML-defined type
    auto data = QQmlData::get(object);
#if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)
    if (!data || !data->compiledData)
        return false;

    const auto qmlType = QQmlMetaType::qmlType(data->compiledData->url());
#else
    if (!data || !data->compilationUnit)
        return false;

    const auto qmlType = QQmlMetaType::qmlType(data->compilationUnit->url());
#endif
    if (!QmlType::isValid(qmlType))
        return false;

    m_typePropertyModel->setObject(QmlType::toObjectInstance(qmlType));
    return true;
}

bool QmlTypeExtension::setMetaObject(const QMetaObject *metaObject)
{
    if (!metaObject)
        return false;

    const auto qmlType = QQmlMetaType::qmlType(metaObject);
    if (!QmlType::isValid(qmlType))
        return false;

    m_typePropertyModel->setObject(QmlType::toObjectInstance(qmlType));
    return true;
}
