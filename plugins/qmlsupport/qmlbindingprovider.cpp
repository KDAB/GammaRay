/*
  qmlbindingprovider.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
#include <private/qqmlpropertybinding_p.h>
#include <private/qmetaobject_p.h>
#include <private/qproperty_p.h>

using namespace GammaRay;

// Bindings of bindable properties, which most QtQuick properties are in Qt 6, aren't in QQmlData's
// list, the property holds them.
static QQmlPropertyBinding *qmlPropertyBinding(QObject *obj, int propertyIndex)
{
    const auto property = obj->metaObject()->property(propertyIndex);
    if (!property.isBindable())
        return nullptr;
    const auto binding = property.bindable(obj).binding();
    auto priv = QPropertyBindingPrivate::get(binding);
    // only QQmlPropertyBinding has a custom vtable, see QPropertyBindingPrivate::destroyAndFreeMemory()
    if (!priv || !priv->hasCustomVTable())
        return nullptr;
    return static_cast<QQmlPropertyBinding *>(priv);
}

// activeGuards is protected, its member pointer can be taken through a subclass
struct ExpressionGuards : QQmlJavaScriptExpression
{
    static QQmlJavaScriptExpressionGuard *first(QQmlJavaScriptExpression *expression)
    {
        return (expression->*(&ExpressionGuards::activeGuards)).first();
    }
    static QQmlJavaScriptExpressionGuard *next(QQmlJavaScriptExpression *expression, QQmlJavaScriptExpressionGuard *guard)
    {
        return (expression->*(&ExpressionGuards::activeGuards)).next(guard);
    }
};

static void setCanonicalNameFromId(BindingNode *node, QObject *obj)
{
    if (QQmlContext *ctx = QQmlEngine::contextForObject(obj)) {
        const QString id = ctx->nameForObject(obj);
        if (!id.isEmpty())
            node->setCanonicalName(QStringLiteral("%1.%2").arg(id, node->canonicalName()));
    }
}

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
    if (!binding) {
        auto propertyBinding = qmlPropertyBinding(node->object(), node->propertyIndex());
        if (!propertyBinding)
            return dependencies;
        // only what is read through notify signals, reads of other bindable properties are
        // tracked by QPropertyBindingPrivate without telling which property
        auto expression = propertyBinding->jsExpression();
        if (QV4::Function *function = expression->function()) {
            const QQmlSourceLocation loc = function->sourceLocation();
            node->setSourceLocation(SourceLocation::fromOneBased(QUrl(loc.sourceFile), loc.line, loc.column));
        }
        for (auto guard = ExpressionGuards::first(expression); guard; guard = ExpressionGuards::next(expression, guard)) {
            QObject *sender = guard->signalIndex() == -1 ? nullptr : guard->senderAsObject();
            if (!sender)
                continue;
            const QMetaObject *meta = sender->metaObject();
            const int notifyIndex = QMetaObjectPrivate::signal(meta, guard->signalIndex()).methodIndex();
            for (int i = 0; i < meta->propertyCount(); ++i) {
                if (meta->property(i).notifySignalIndex() != notifyIndex)
                    continue;
                auto childNode = new BindingNode(sender, i, node);
                setCanonicalNameFromId(childNode, sender);
                dependencies.push_back(std::unique_ptr<BindingNode>(childNode));
            }
        }
        return dependencies;
    }

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

    const QMetaObject *meta = obj->metaObject();
    for (int i = 0; i < meta->propertyCount(); ++i) {
        if (!qmlPropertyBinding(obj, i))
            continue;
        auto node = new BindingNode(obj, i);
        setCanonicalNameFromId(node, obj);
        bindings.push_back(std::unique_ptr<BindingNode>(node));
    }
    return bindings;
}
