/*
  qmetaobjectvalidator.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
