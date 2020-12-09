/*
  streamoperators.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
    StreamOperators::registerOperators<QHash<int, QByteArray> >();

    StreamOperators::registerOperators<ObjectId>();
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    // This is needed so QVariant based comparison works (ie: QAIM::match)
    QMetaType::registerComparators<ObjectId>();
#endif

    StreamOperators::registerOperators<ObjectIds>();
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    // This is needed so QVariant based comparison works (ie: QAIM::match)
    QMetaType::registerComparators<ObjectIds>();
#endif

    StreamOperators::registerOperators<GammaRay::VariantWrapper>();
    StreamOperators::registerOperators<GammaRay::SourceLocation>();
    StreamOperators::registerOperators<QVector<SourceLocation>>();
    StreamOperators::registerOperators<GammaRay::QMetaObjectValidatorResult::Results>();
    StreamOperators::registerOperators<GammaRay::PropertyModel::PropertyFlags>();

    StreamOperators::registerOperators<EnumDefinition>();
    StreamOperators::registerOperators<EnumValue>();
}
