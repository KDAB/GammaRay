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

#include <QSet>

#include <functional>
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

namespace {
struct ReachableObjects
{
    QList<QObject *> objects;
    QSet<QObject *> seen;
    QSet<QObject *> expanded;

    void add(QObject *obj)
    {
        if (obj && !seen.contains(obj)) {
            seen.insert(obj);
            objects.push_back(obj);
        }
    }
    void addWithChildren(QObject *obj)
    {
        if (!obj || expanded.contains(obj))
            return;
        expanded.insert(obj);
        add(obj);
        for (QObject *child : obj->children())
            add(child);
    }
};
}

// the visual parent of QtQuick items, without linking QtQuick
static QObject *parentProperty(QObject *obj)
{
    const int index = obj->metaObject()->indexOfProperty("parent");
    if (index < 0 || !(obj->metaObject()->property(index).metaType().flags() & QMetaType::PointerToQObject))
        return nullptr;
    return obj->metaObject()->property(index).read(obj).value<QObject *>();
}

// The objects a binding can reach without going through a property: its scope object, the
// ancestors, children and siblings of it, and the objects with an id in its contexts.
static QList<QObject *> reachableObjects(QQmlJavaScriptExpression *expression)
{
    ReachableObjects reachable;
    QObject *scope = expression->scopeObject();
    for (QObject *obj = scope; obj; obj = obj->parent())
        reachable.addWithChildren(obj);
    for (QObject *obj = scope; obj; obj = parentProperty(obj))
        reachable.addWithChildren(obj);
    for (auto context = expression->context(); context; context = context->parent()) {
        for (int i = 0; i < context->numIdValues(); ++i)
            reachable.add(context->idValue(i));
    }
    return reachable.objects;
}

// What kind of observer a QPropertyObserver is, is private. Access checks don't apply to the
// arguments of explicit template instantiations, a friend function defined there hands it out.
template<typename Tag, typename Tag::type Member>
struct PrivateMember
{
    friend typename Tag::type get(Tag)
    {
        return Member;
    }
};
struct ObserverNext
{
    using type = QTaggedPointer<QPropertyObserver, QPropertyObserverBase::ObserverTag> QPropertyObserverBase::*;
    friend type get(ObserverNext);
};
template struct PrivateMember<ObserverNext, &QPropertyObserverBase::next>;

// A binding reads bindable properties through QPropertyObservers it puts into the observer list of
// each such property, without telling which property. Find these lists among the properties of the
// objects the binding can reach.
static void findBindableDependencies(QQmlPropertyBinding *binding, const std::function<void(QObject *, int)> &found)
{
    const auto objects = reachableObjects(binding->jsExpression());
    for (QObject *obj : objects) {
        const QMetaObject *meta = obj->metaObject();
        for (int i = 0; i < meta->propertyCount(); ++i) {
            const auto property = meta->property(i);
            if (!property.isBindable())
                continue;
            const auto data = QUntypedBindablePrivate::getPropertyData(property.bindable(obj));
            // the properties of QObjects keep their binding data in the binding storage, only
            // when something observes them or binds them
            const auto bindingData = obj->bindingStorage()->bindingData(data);
            if (!bindingData)
                continue;
            for (auto observer = QPropertyBindingDataPointer { bindingData }.firstObserver(); observer; observer = observer.nextObserver()) {
                if ((observer.ptr->*get(ObserverNext())).tag() == QPropertyObserver::ObserverNotifiesBinding && observer.binding() == binding) {
                    found(obj, i);
                    break;
                }
            }
        }
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
        auto expression = propertyBinding->jsExpression();
        if (QV4::Function *function = expression->function()) {
            const QQmlSourceLocation loc = function->sourceLocation();
            node->setSourceLocation(SourceLocation::fromOneBased(QUrl(loc.sourceFile), loc.line, loc.column));
        }
        QSet<std::pair<QObject *, int>> seen;
        const auto addDependency = [&](QObject *obj, int propertyIndex) {
            if (seen.contains({ obj, propertyIndex }))
                return;
            seen.insert({ obj, propertyIndex });
            auto childNode = new BindingNode(obj, propertyIndex, node);
            setCanonicalNameFromId(childNode, obj);
            dependencies.push_back(std::unique_ptr<BindingNode>(childNode));
        };
        // properties without bindable storage are read through their notify signals
        for (auto guard = ExpressionGuards::first(expression); guard; guard = ExpressionGuards::next(expression, guard)) {
            QObject *sender = guard->signalIndex() == -1 ? nullptr : guard->senderAsObject();
            if (!sender)
                continue;
            const QMetaObject *meta = sender->metaObject();
            const int notifyIndex = QMetaObjectPrivate::signal(meta, guard->signalIndex()).methodIndex();
            for (int i = 0; i < meta->propertyCount(); ++i) {
                if (meta->property(i).notifySignalIndex() == notifyIndex)
                    addDependency(sender, i);
            }
        }
        findBindableDependencies(propertyBinding, addDependency);
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
