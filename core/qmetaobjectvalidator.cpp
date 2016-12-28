/*
  qmetaobjectvalidator.cpp

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

#include "qmetaobjectvalidator.h"

#include <QDebug>
#include <QMetaMethod>
#include <QMetaObject>
#include <QMetaProperty>

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

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    // check if property uses a known type
    if (property.userType() == QMetaType::UnknownType)
        r |= QMetaObjectValidatorResult::UnknownPropertyType;
#endif

    return r;
}

QMetaObjectValidatorResult::Results QMetaObjectValidator::checkMethod(const QMetaObject *mo, const QMetaMethod &method)
{
    QMetaObjectValidatorResult::Results r = QMetaObjectValidatorResult::NoIssue;

    // check for parameters with unknown type
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    // don't check internal methods such as _q_createJSWrapper() from QQuickItem
    if (!method.name().startsWith("_q")) {
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
#else
    Q_UNUSED(mo);
    Q_UNUSED(method);
#endif

    return r;
}

QMetaObjectValidatorResult::Results QMetaObjectValidator::check(const QMetaObject *mo)
{
    QMetaObjectValidatorResult::Results r = QMetaObjectValidatorResult::NoIssue;
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
