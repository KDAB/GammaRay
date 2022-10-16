/*
  qmetaobjectvalidatorresult.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QMETAOBJECTVALIDATOR_RESULT_H
#define GAMMARAY_QMETAOBJECTVALIDATOR_RESULT_H

#include <qglobal.h>
#include <QMetaType>

namespace GammaRay {

/*! Results of QMetaObjectValidator. */
namespace QMetaObjectValidatorResult {

enum Result
{
    NoIssue = 0,
    SignalOverride = 1,
    UnknownMethodParameterType = 2,
    PropertyOverride = 4,
    UnknownPropertyType = 8
};

Q_DECLARE_FLAGS(Results, Result)

}

}

Q_DECLARE_METATYPE(GammaRay::QMetaObjectValidatorResult::Results)
Q_DECLARE_OPERATORS_FOR_FLAGS(GammaRay::QMetaObjectValidatorResult::Results)

#endif
