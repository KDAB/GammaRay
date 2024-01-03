/*
  qmetaobjectvalidatorresult.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
