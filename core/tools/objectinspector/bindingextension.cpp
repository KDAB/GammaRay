/*
  bindingextension.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Volker Krause <volker.krause@kdab.com>
           Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

BindingExtension::BindingExtension(PropertyController* controller)
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

bool BindingExtension::setQObject(QObject* object)
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
