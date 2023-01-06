/*
  qmlbindingprovider.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

// Own
#include "qmlbindingprovider.h"

#include <core/util.h>
#include <core/bindingnode.h>

// Qt
#include <private/qqmlabstractbinding_p.h>
#include <private/qqmlbinding_p.h>
#include <private/qqmlglobal_p.h>
#include <private/qqmlvaluetypeproxybinding_p.h>
#include <private/qqmldata_p.h>
#include <private/qqmlproperty_p.h>

using namespace GammaRay;

QQmlAbstractBinding *QmlBindingProvider::bindingForProperty(QObject *obj, int propertyIndex)
{
    auto data = QQmlData::get(obj);
    if (!data || !data->hasBindingBit(propertyIndex))
        return nullptr;

    auto b = data->bindings;
    while (b) {
        int index;
        index = b->targetPropertyIndex().coreIndex();

        if (index == propertyIndex) {
            return b;
        }
        b = b->nextBinding();
    }
    return nullptr;
}

bool QmlBindingProvider::canProvideBindingsFor(QObject *object) const
{
    return QQmlData::get(object);
}

void QmlBindingProvider::fetchSourceLocationFor(BindingNode *node, QQmlBinding *binding)
{
    QV4::Function *function = binding->function();
    if (function) {
        QQmlSourceLocation loc = function->sourceLocation();
        node->setSourceLocation(SourceLocation::fromOneBased(QUrl(loc.sourceFile), loc.line, loc.column));
    }
}

std::vector<std::unique_ptr<BindingNode>> QmlBindingProvider::findDependenciesFor(BindingNode *node) const
{
    std::vector<std::unique_ptr<BindingNode>> dependencies;
    if (node->hasFoundBindingLoop()) // Don't look for further dependencies, if this is already known to be part of a loop.
        return dependencies;

    QQmlAbstractBinding *abstractBinding = QQmlPropertyPrivate::binding(node->object(), QQmlPropertyIndex::fromEncoded(node->propertyIndex()));
    QQmlBinding *binding = dynamic_cast<QQmlBinding *>(abstractBinding);
    if (!binding)
        return dependencies;

    fetchSourceLocationFor(node, binding); // While we have the QQmlBinding at hand, let's grab the source location
    const auto bindingDependencies = binding->dependencies();
    for (const auto &dependency : bindingDependencies) {
        BindingNode *childNode = new BindingNode(dependency.object(), dependency.index(), node);
        QQmlContext *ctx = QQmlEngine::contextForObject(dependency.object());
        if (ctx) {
            QString id = ctx->nameForObject(dependency.object());
            if (!id.isEmpty())
                childNode->setCanonicalName(QStringLiteral("%1.%2").arg(id, childNode->canonicalName()));
        }

        dependencies.push_back(std::unique_ptr<BindingNode>(childNode));
    }
    return dependencies;
}

std::vector<std::unique_ptr<BindingNode>> QmlBindingProvider::findBindingsFor(QObject *obj) const
{
    std::vector<std::unique_ptr<BindingNode>> bindings;
    auto data = QQmlData::get(obj);
    if (!data)
        return bindings;

    auto b = data->bindings;
    while (b) {
        BindingNode *node = new BindingNode(obj, b->targetPropertyIndex().coreIndex());
        QQmlContext *ctx = QQmlEngine::contextForObject(obj);
        if (ctx) {
            QString id = ctx->nameForObject(obj);
            if (!id.isEmpty())
                node->setCanonicalName(QStringLiteral("%1.%2").arg(id, node->canonicalName()));
        }

        bindings.push_back(std::unique_ptr<BindingNode>(node));
        b = b->nextBinding();
    }
    return bindings;
}
