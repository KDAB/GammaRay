/*
  bindingaggregator.cpp

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
#include "bindingaggregator.h"

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
#include <QMutexLocker>

using namespace GammaRay;

Q_GLOBAL_STATIC(std::vector<std::unique_ptr<AbstractBindingProvider>>, s_providers)

void BindingAggregator::registerBindingProvider(std::unique_ptr<AbstractBindingProvider> provider)
{
    s_providers()->push_back(std::move(provider));
}

bool GammaRay::BindingAggregator::providerAvailableFor(QObject* object)
{
    return std::find_if(s_providers()->begin(), s_providers()->end(),
                                        [object](const std::unique_ptr<AbstractBindingProvider>& provider) {
                                            return provider->canProvideBindingsFor(object);
                                        }) != s_providers()->end();
}

std::vector<std::unique_ptr<BindingNode>> BindingAggregator::findDependenciesFor(BindingNode* node)
{
    std::vector<std::unique_ptr<BindingNode>> allDependencies;
    if (node->isPartOfBindingLoop())
        return allDependencies;

    for (const auto &provider : *s_providers()) {
        auto providerDependencies = provider->findDependenciesFor(node);
        for (auto &&providerDependency : providerDependencies) {
            providerDependency->dependencies() = findDependenciesFor(providerDependency.get());
            allDependencies.push_back(std::move(providerDependency));
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

std::vector<std::unique_ptr<BindingNode>> BindingAggregator::bindingTreeForObject(QObject* obj)
{
    std::vector<std::unique_ptr<BindingNode>> bindings;
    if (obj) {
        for (auto providerIt = s_providers()->begin(); providerIt != s_providers()->cend(); ++providerIt) {
            auto newBindings = (*providerIt)->findBindingsFor(obj);
            for (auto &&newBinding : newBindings) {
                BindingNode *node = newBinding.get();
                if (std::find_if(bindings.begin(), bindings.end(),
                    [node](const std::unique_ptr<BindingNode> &other){ return *node == *other; }) != bindings.end()) {
                    continue; // apparantly this is a duplicate.
                }
                node->dependencies() = findDependenciesFor(node);

                bindings.push_back(std::move(newBinding));
            }

        }
    }
    return bindings;
}

void BindingAggregator::scanForBindingLoops()
{
    const QVector<QObject*> &allObjects = Probe::instance()->allQObjects();

    QMutexLocker lock(Probe::objectLock());
    for (QObject *obj : allObjects) {
        if (!Probe::instance()->isValidObject(obj))
            continue;

        auto bindings = bindingTreeForObject(obj);
        for (auto &&bindingNode : bindings) {
            if (bindingNode->isPartOfBindingLoop()) {
                Problem p;
                p.severity = Problem::Error;
                p.description = QStringLiteral("Object %1 / Property %2 has a binding loop.").arg(ObjectDataProvider::typeName(bindingNode->object())).arg(bindingNode->canonicalName());
                p.object = ObjectId(bindingNode->object());
                p.locations.push_back(bindingNode->sourceLocation());
                p.problemId = QString("com.kdab.GammaRay.ObjectInspector.BindingLoopScan:%1.%2").arg(reinterpret_cast<quintptr>(bindingNode->object())).arg(bindingNode->propertyIndex());
                p.findingCategory = Problem::Scan;
                ProblemCollector::addProblem(p);
            }
        }
    }
}
