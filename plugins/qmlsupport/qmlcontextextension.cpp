/*
  qmlcontextextension.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
#include <private/qqmlcontextdata_p.h>

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
                     m_propertyModel,
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
