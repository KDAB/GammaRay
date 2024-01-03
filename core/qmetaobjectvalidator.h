/*
  qmetaobjectvalidator.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QMETAOBJECTVALIDATOR_H
#define GAMMARAY_QMETAOBJECTVALIDATOR_H

#include <common/qmetaobjectvalidatorresult.h>

QT_BEGIN_NAMESPACE
class QMetaMethod;
struct QMetaObject;
class QMetaProperty;
QT_END_NAMESPACE

namespace GammaRay {

/*! Checks QMetaObject instances for common problems. */
namespace QMetaObjectValidator {

QMetaObjectValidatorResult::Results checkProperty(const QMetaObject *mo, const QMetaProperty &property);
QMetaObjectValidatorResult::Results checkMethod(const QMetaObject *mo, const QMetaMethod &method);

QMetaObjectValidatorResult::Results check(const QMetaObject *mo);

}

}

#endif // GAMMARAY_QMETAOBJECTVALIDATOR_H
