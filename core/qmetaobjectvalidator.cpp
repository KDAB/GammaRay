/*
  qmetaobjectvalidator.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <config-gammaray.h>

#include "qmetaobjectvalidator.h"
#include "execution.h"
#include "metaobjectregistry.h"
#include "probe.h"
#include "problemcollector.h"

#include <QDebug>
#include <QMetaMethod>
#include <QMetaObject>
#include <QMetaProperty>

#include <private/qobject_p.h>
#include <private/qmetaobject_p.h>

using namespace GammaRay;

QMetaObjectValidatorResult::Results QMetaObjectValidator::checkProperty(const QMetaObject *mo, const QMetaProperty &property)
{
    QMetaObjectValidatorResult::Results r = QMetaObjectValidatorResult::NoIssue;

    // check if property overrides base-class declaration
    if (mo->superClass()) {
        const auto baseIdx = mo->superClass()->indexOfProperty(property.name());
        if (baseIdx >= 0)
            r |= QMetaObjectValidatorResult::PropertyOverride;
    }

    // check if property uses a known type
    if (property.userType() == QMetaType::UnknownType)
        r |= QMetaObjectValidatorResult::UnknownPropertyType;

    return r;
}

QMetaObjectValidatorResult::Results QMetaObjectValidator::checkMethod(const QMetaObject *mo, const QMetaMethod &method)
{
    QMetaObjectValidatorResult::Results r = QMetaObjectValidatorResult::NoIssue;

    // check for parameters with unknown type
    // don't check internal methods such as _q_createJSWrapper() from QQuickItem
    if (!method.name().startsWith("_q")) { // krazy:exclude=strings
        for (int j = 0; j < method.parameterCount(); ++j) {
            if (method.parameterType(j) == QMetaType::UnknownType)
                r |= QMetaObjectValidatorResult::UnknownMethodParameterType;
        }
    }

    // check for signal overrides
    if (method.methodType() == QMetaMethod::Signal && mo->superClass()) {
        const auto baseIdx = mo->superClass()->indexOfMethod(method.methodSignature());
        if (baseIdx >= 0)
            r |= QMetaObjectValidatorResult::SignalOverride;
    }

    return r;
}

static bool isDynamicMetaObject(const QMetaObject *mo)
{
    Q_ASSERT(reinterpret_cast<const QMetaObjectPrivate *>(mo->d.data)->revision >= 3);
    return reinterpret_cast<const QMetaObjectPrivate *>(mo->d.data)->flags & DynamicMetaObject;
}

QMetaObjectValidatorResult::Results QMetaObjectValidator::check(const QMetaObject *mo)
{
    QMetaObjectValidatorResult::Results r = QMetaObjectValidatorResult::NoIssue;
    if (isDynamicMetaObject(mo)) // those may dynamically add properties we query...
        return r;

    for (int i = mo->propertyOffset(); i < mo->propertyCount(); ++i) {
        const auto prop = mo->property(i);
        r |= checkProperty(mo, prop);
    }

    for (int i = mo->methodOffset(); i < mo->methodCount(); ++i) {
        const auto method = mo->method(i);
        r |= checkMethod(mo, method);
    }

    return r;
}
