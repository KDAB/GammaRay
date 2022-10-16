/*
  bindingextension.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

// Own
#include "bindingextension.h"
#include "bindingmodel.h"

#include <core/abstractbindingprovider.h>
#include <core/bindingaggregator.h>
#include <core/bindingnode.h>
#include <core/objectdataprovider.h>
#include <core/probe.h>
#include <core/problemcollector.h>
#include <core/propertycontroller.h>
#include <common/objectbroker.h>

// Qt
#include <QMetaProperty>
#include <QMetaObject>

using namespace GammaRay;

BindingExtension::BindingExtension(PropertyController *controller)
    : QObject(controller)
    , PropertyControllerExtension(controller->objectBaseName() + ".bindings")
    , m_object(nullptr)
    , m_bindingModel(new BindingModel(this))
{
    ObjectBroker::registerObject(controller->objectBaseName() + ".bindingsExtension", this);
    controller->registerModel(m_bindingModel, QStringLiteral("bindingModel"));
}

BindingExtension::~BindingExtension() = default;

void BindingExtension::clear()
{
    if (m_object)
        disconnect(m_object, nullptr, this, nullptr);

    m_bindingModel->aboutToClear();
    m_bindings.clear();
    m_object = nullptr;
    m_bindingModel->cleared();
}

bool BindingExtension::setQObject(QObject *object)
{
    if (m_object)
        disconnect(m_object, nullptr, this, nullptr);

    if (object) {
        if (!BindingAggregator::providerAvailableFor(object)) {
            m_bindings.clear();
            m_bindingModel->setObject(nullptr, m_bindings);
            m_object = nullptr;
            return false;
        }

        m_bindings = BindingAggregator::bindingTreeForObject(object);
        for (const auto &node : m_bindings) {
            int signalIndex = node->property().notifySignalIndex();
            if (signalIndex != -1) {
                QMetaObject::connect(object, signalIndex, this, metaObject()->indexOfMethod("propertyChanged()"), Qt::UniqueConnection);
            }
        }
        connect(object, &QObject::destroyed, this, &BindingExtension::clear);
    }

    m_bindingModel->setObject(object, m_bindings);
    m_object = object;
    return true;
}


void BindingExtension::propertyChanged()
{
    Q_ASSERT(sender() == m_object);

    for (size_t i = 0; i < m_bindings.size(); ++i) {
        const auto &bindingNode = m_bindings[i].get();
        if (bindingNode->property().notifySignalIndex() == senderSignalIndex()) {
            m_bindingModel->refresh(i, BindingAggregator::findDependenciesFor(bindingNode));
            // There can be more than one property with the same notify signal,
            // so no break here...
        }
    }
}

BindingModel *BindingExtension::model() const
{
    return m_bindingModel;
}
