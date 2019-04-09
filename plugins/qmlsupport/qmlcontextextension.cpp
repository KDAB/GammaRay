/*
  qmlcontextextension.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "qmlcontextextension.h"
#include "qmlcontextmodel.h"

#include <core/aggregatedpropertymodel.h>
#include <core/objectinstance.h>
#include <core/propertycontroller.h>

#include <common/objectbroker.h>
#include <common/objectmodel.h>

#include <QQmlContext>
#include <private/qqmlcontext_p.h>
#include <private/qqmldata_p.h>

#include <QItemSelectionModel>

using namespace GammaRay;

QmlContextExtension::QmlContextExtension(PropertyController *controller)
    : PropertyControllerExtension(controller->objectBaseName() + ".qmlContext")
    , m_contextModel(new QmlContextModel(controller))
    , m_propertyModel(new AggregatedPropertyModel(controller))
{
    controller->registerModel(m_contextModel, QStringLiteral("qmlContextModel"));
    auto contextSelectionModel = ObjectBroker::selectionModel(m_contextModel);
    QObject::connect(contextSelectionModel, &QItemSelectionModel::selectionChanged,
                     [this](const QItemSelection &selection) {
        contextSelected(selection);
    });

    controller->registerModel(m_propertyModel, QStringLiteral("qmlContextPropertyModel"));
}

QmlContextExtension::~QmlContextExtension() = default;

bool QmlContextExtension::setQObject(QObject *object)
{
    if (!object)
        return false;

    auto context = qobject_cast<QQmlContext *>(object);
    if (!context) {
        auto data = QQmlData::get(object);
        if (data && data->context)
            context = data->context->asQQmlContext();
    }

    m_contextModel->setContext(context);
    return context;
}

void QmlContextExtension::contextSelected(const QItemSelection &selection)
{
    if (selection.isEmpty()) {
        m_propertyModel->setObject(nullptr);
        return;
    }

    const auto idx = selection.first().topLeft();
    const auto context = idx.data(ObjectModel::ObjectRole).value<QQmlContext *>();
    m_propertyModel->setObject(context);
}
