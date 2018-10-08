/*
  bindingextension.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

std::vector<std::unique_ptr<AbstractBindingProvider>> BindingExtension::s_providers;

void BindingExtension::registerBindingProvider(std::unique_ptr<AbstractBindingProvider> provider)
{
    s_providers.push_back(std::move(provider));
}

BindingExtension::BindingExtension(PropertyController* controller)
    : QObject(controller)
    , PropertyControllerExtension(controller->objectBaseName() + ".bindings")
    , m_object(nullptr)
    , m_bindingModel(new BindingModel(this))
{
    ObjectBroker::registerObject(controller->objectBaseName() + ".bindingsExtension", this);
    controller->registerModel(m_bindingModel, QStringLiteral("bindingModel"));

    connect(ProblemCollector::instance(), SIGNAL(problemScanRequested()), this, SLOT(scanForBindingLoops()));
}

BindingExtension::~BindingExtension()
{
}

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
        if (std::find_if(s_providers.begin(), s_providers.end(),
                                        [object](const std::unique_ptr<AbstractBindingProvider>& provider) {
                                            return provider->canProvideBindingsFor(object);
                                        }) == s_providers.end()) {
            m_bindings.clear();
            m_bindingModel->setObject(nullptr, m_bindings);
            m_object = nullptr;
            return false;
        }

        m_bindings = bindingTreeForObject(object);
        for (size_t i = 0; i < m_bindings.size(); ++i) {
            const auto &node = m_bindings[i];
            int signalIndex = node->property().notifySignalIndex();
            if (signalIndex != -1) {
                QMetaObject::connect(object, signalIndex, this, metaObject()->indexOfMethod("propertyChanged()"), Qt::UniqueConnection);
            }
        }
        connect(object, SIGNAL(destroyed()), this, SLOT(clear()));
    }

    m_bindingModel->setObject(object, m_bindings);
    m_object = object;
    return true;
}

std::vector<std::unique_ptr<BindingNode>> BindingExtension::findDependenciesFor(BindingNode* node) const
{
    std::vector<std::unique_ptr<BindingNode>> allDependencies;
    if (node->isPartOfBindingLoop())
        return allDependencies;

    for (auto providerIt = s_providers.cbegin(); providerIt != s_providers.cend(); ++providerIt) {
        auto &&provider = *providerIt;
        auto providerDependencies = provider->findDependenciesFor(node);
        for (auto dependencyIt = providerDependencies.begin(); dependencyIt != providerDependencies.end(); ++dependencyIt) {
            dependencyIt->get()->dependencies() = findDependenciesFor(dependencyIt->get());
            allDependencies.push_back(std::move(*dependencyIt));
        }
    }
    std::sort(
        allDependencies.begin(),
        allDependencies.end(),
        [](const std::unique_ptr<BindingNode> &a, const std::unique_ptr<BindingNode> &b) {
            return a->object() < b->object() || (a->object() == b->object() && a->propertyIndex() < b->propertyIndex());
        }
    );
    return allDependencies;
}

void BindingExtension::propertyChanged()
{
    Q_ASSERT(sender() == m_object);

    for (size_t i = 0; i < m_bindings.size(); ++i) {
        const auto &bindingNode = m_bindings[i].get();
        if (bindingNode->property().notifySignalIndex() == senderSignalIndex()) {
            m_bindingModel->refresh(i, findDependenciesFor(bindingNode));
            // There can be more than one property with the same notify signal,
            // so no break here...
        }
    }
}

std::vector<std::unique_ptr<BindingNode>> BindingExtension::bindingTreeForObject(QObject* obj) const
{
    std::vector<std::unique_ptr<BindingNode>> bindings;
    if (obj) {
        for (auto providerIt = s_providers.begin(); providerIt != s_providers.cend(); ++providerIt) {
            auto newBindings = (*providerIt)->findBindingsFor(obj);
            for (auto nodeIt = newBindings.begin(); nodeIt != newBindings.end(); ++nodeIt) {
                BindingNode *node = nodeIt->get();
                if (std::find_if(bindings.begin(), bindings.end(),
                    [node](const std::unique_ptr<BindingNode> &other){ return *node == *other; }) != bindings.end()) {
                    continue; // apparantly this is a duplicate.
                }
                node->dependencies() = findDependenciesFor(node);

                bindings.push_back(std::move(*nodeIt));
            }

        }
    }
    return bindings;
}

void GammaRay::BindingExtension::scanForBindingLoops() const
{
    const QVector<QObject*> &allObjects = Probe::instance()->allQObjects();

    foreach (QObject *obj, allObjects) {
        auto bindings = bindingTreeForObject(obj);
        for (auto it = bindings.begin(); it != bindings.end(); ++it) {
            auto &&bindingNode = *it;
            if (bindingNode->isPartOfBindingLoop()) {
                Problem p;
                p.severity = Problem::Error;
                p.description = QStringLiteral("Object %1 / Property %2 has a binding loop.").arg(ObjectDataProvider::typeName(bindingNode->object())).arg(bindingNode->canonicalName());
                p.object = ObjectId(bindingNode->object());
                p.location = bindingNode->sourceLocation();
                p.problemId = QString("BindingLoop:%1.%2").arg(reinterpret_cast<qintptr>(bindingNode->object())).arg(bindingNode->propertyIndex());
                p.findingCategory = Problem::Scan;
                ProblemCollector::addProblem(p);
            }
        }
    }
}

BindingModel *BindingExtension::model() const
{
    return m_bindingModel;
}
