/*
  streamoperators.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "streamoperators.h"
#include "metatypedeclarations.h"
#include "variantwrapper.h"
#include "sourcelocation.h"
#include "qmetaobjectvalidatorresult.h"
#include "objectid.h"
#include "enumdefinition.h"
#include "enumvalue.h"
#include "propertymodel.h"

#include <QMetaMethod>

using namespace GammaRay;

QT_BEGIN_NAMESPACE
GAMMARAY_ENUM_STREAM_OPERATORS(QMetaMethod::Access)
GAMMARAY_ENUM_STREAM_OPERATORS(QMetaMethod::MethodType)
GAMMARAY_ENUM_STREAM_OPERATORS(Qt::ConnectionType)
GAMMARAY_ENUM_STREAM_OPERATORS(Qt::CaseSensitivity)
GAMMARAY_ENUM_STREAM_OPERATORS(GammaRay::QMetaObjectValidatorResult::Results)
GAMMARAY_ENUM_STREAM_OPERATORS(GammaRay::PropertyModel::PropertyFlags)
QT_END_NAMESPACE

void StreamOperators::registerOperators()
{
    StreamOperators::registerOperators<QMetaMethod::Access>();
    StreamOperators::registerOperators<QMetaMethod::MethodType>();
    StreamOperators::registerOperators<Qt::ConnectionType>();
    StreamOperators::registerOperators<Qt::CaseSensitivity>();
    StreamOperators::registerOperators<QHash<int, QByteArray>>();

    StreamOperators::registerOperators<ObjectId>();

    StreamOperators::registerOperators<ObjectIds>();

    StreamOperators::registerOperators<GammaRay::VariantWrapper>();
    StreamOperators::registerOperators<GammaRay::SourceLocation>();
    StreamOperators::registerOperators<QVector<SourceLocation>>();
    StreamOperators::registerOperators<GammaRay::QMetaObjectValidatorResult::Results>();
    StreamOperators::registerOperators<GammaRay::PropertyModel::PropertyFlags>();

    StreamOperators::registerOperators<EnumDefinition>();
    StreamOperators::registerOperators<EnumValue>();
}
