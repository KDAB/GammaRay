/*
  qmlbindingnode.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#include "qmlbindingnode.h"
#include <core/util.h>

#define USE_QT_BINDINGDEPENDENCY_API

#ifndef USE_QT_BINDINGDEPENDENCY_API
#define private public
#define protected public
#endif

#include <QDebug>

#include <private/qqmlabstractbinding_p.h>
#include <private/qqmlbinding_p.h>
#include <private/qqmldata_p.h>
#include <private/qqmlproperty_p.h>
#include <QFile>

using namespace GammaRay;

QDebug &GammaRay::operator<< (QDebug dbg, const QmlBindingNode &node)
{
    const auto prop = node.object()->metaObject()->property(node.propertyIndex());
    dbg.nospace() << (node.isBinding() ? "Binding: " : "Property: ")
                  << prop.name()
                  << "("
                  << (node.object()->objectName().isEmpty()
                        ? QString::number(reinterpret_cast<long long>(node.object()), 16)
                        : node.object()->objectName())
                  << ")"
                  << (node.isBindingLoop() ? "(This binding produces a loop!)" : "");
    for (const auto &childNode : node.dependencies()) {
        dbg << '\n';
        dbg << ">" << *childNode;
    }
    return dbg;
}

QQmlAbstractBinding *QmlBindingNode::bindingForProperty(QObject *obj, int propertyIndex)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    auto data = QQmlData::get(obj);
    if (!data)
        return Q_NULLPTR;

    auto b = data->bindings;
    while (b) {
        int index;
#if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)
        QQmlPropertyData::decodeValueTypePropertyIndex(b->targetPropertyIndex(), &index);
#else
        index = b->targetPropertyIndex().coreIndex();
#endif

        if (index == propertyIndex) {
            return b;
        }
        b = b->nextBinding();
    }
#endif
    return Q_NULLPTR;
}

QmlBindingNode::QmlBindingNode(QObject* object, int propertyIndex, QmlBindingNode *parent)
    : m_parent(parent)
    , m_object(object)
    , m_id(QQmlEngine::contextForObject(object)->nameForObject(object))
    , m_propertyIndex(propertyIndex)
{
    m_binding = bindingForProperty(object, propertyIndex);
    if (m_binding) {
        fetchBindingCode();
        checkForLoops();
        findDependencies();
    } else {
        fetchPropertyCode();
    }
    refreshValue();
}


QmlBindingNode::QmlBindingNode(QQmlAbstractBinding* binding, QmlBindingNode *parent)
    : m_parent(parent)
    , m_object(binding->targetObject())
    , m_binding(binding)

{
#if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)
    QQmlPropertyData::decodeValueTypePropertyIndex(binding->targetPropertyIndex(), &m_propertyIndex);
#else
    m_propertyIndex = m_binding->targetPropertyIndex().coreIndex();
#endif

    QQmlContext *ctx = QQmlEngine::contextForObject(m_object);
    if (ctx) {
        m_id = ctx->nameForObject(m_object);
    }

    refreshValue();
    fetchBindingCode();
    checkForLoops();
    findDependencies();
}

QmlBindingNode::QmlBindingNode(QQmlProperty property, QmlBindingNode* parent)
    : m_parent(parent)
    , m_object(property.object())
    , m_propertyIndex(property.index())
{
    m_binding = bindingForProperty(m_object, m_propertyIndex);

    QQmlContext *ctx = QQmlEngine::contextForObject(m_object);
    if (ctx) {
        m_id = ctx->nameForObject(m_object);
    }

    if (m_binding) {
        fetchBindingCode();
        checkForLoops();
        findDependencies();
    } else {
        fetchPropertyCode();
    }
    refreshValue();
}

void GammaRay::QmlBindingNode::fetchPropertyCode()
{
//     auto qmldata = QQmlData::get(m_object);
}

void GammaRay::QmlBindingNode::fetchBindingCode()
{
    return;
// #if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)
//     QQmlEnginePrivate *ep = QQmlEnginePrivate::get(QQmlData::get(m_object)->context->engine);
//     QV4::Scope scope(ep->v4engine());
//     QV4::ScopedValue f(scope, m_binding->m_function.value());
//     QV4::Function *function = f->as<QV4::FunctionObject>()->function();
// #else
//     QV4::Function *function = m_binding->function();
//     QQmlSourceLocation loc = function->sourceLocation();
//     m_sourceLocation = SourceLocation(QUrl(loc.sourceFile), loc.line, loc.column);
// #endif
//
//     QString fileName = function->compilationUnit->fileName();
//     if (fileName.isEmpty()) {
//         return;
//     }
//     QFile codeFile(QUrl(function->compilationUnit->fileName()).toLocalFile());
//     if (!codeFile.open(QIODevice::ReadOnly)) {
//         qDebug() << "Can't open file :(";
//         return;
//     }
//
//     codeFile.waitForReadyRead(1000);
//     for (uint i = 1; i < function->compiledFunction->location.line; i++)
//         codeFile.readLine();
//     if (!codeFile.canReadLine()) {
//         qDebug() << "File ends before line" << function->compiledFunction->location.line;
//     }
//
//     m_expression = QString(codeFile.readLine()).trimmed();
}

void GammaRay::QmlBindingNode::checkForLoops()
{
    QmlBindingNode *ancestor = m_parent;
    while (ancestor) {
        if (ancestor->object() == m_object
            && ancestor->propertyIndex() == m_propertyIndex) {
            qDebug() << "Found a binding loop!";
            m_isBindingLoop = true;
            return;
        }
        ancestor = ancestor->parent();
    }
    m_isBindingLoop = false;
}

void GammaRay::QmlBindingNode::findDependencies()
{
    if (m_isBindingLoop) // Don't look for further dependencies, if this is already known to be part of a loop.
        return;

#ifdef USE_QT_BINDINGDEPENDENCY_API
    for (const auto &dependency : m_binding->dependencies()) {
        m_dependencies.push_back(std::unique_ptr<QmlBindingNode>(new QmlBindingNode(dependency, this)));
    }
#else
    auto context = QQmlEngine::contextForObject(m_object);
    QQmlEnginePrivate *ep = QQmlEnginePrivate::get(QQmlData::get(m_object)->context->engine);
    QV4::Scope scope(ep->v4engine());
#if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)
    QV4::ScopedValue f(scope, m_binding->m_function.value());
    QV4::Function *function = f->as<QV4::FunctionObject>()->function();
#else
    QV4::Function *function = m_binding->function();
#endif

    // Static dependencies
    const QV4::CompiledData::Function *compiledData = function->compiledFunction;
    const auto idObjectDependencyTable = compiledData->qmlIdObjectDependencyTable();
    const auto contextPropertiesDependencyTable = compiledData->qmlContextPropertiesDependencyTable();
    const auto scopePropertiesDependencyTable = compiledData->qmlScopePropertiesDependencyTable();
    for (quint32 i = 0; i < compiledData->nDependingIdObjects; ++i) {
        auto idObjectIndex = *(idObjectDependencyTable + i);
    }
    for (quint32 i = 0; i < compiledData->nDependingContextProperties; ++i) {
        auto propertyIndex = *(contextPropertiesDependencyTable + 2*i);
        auto notifyIndex = *(contextPropertiesDependencyTable + 2*i + 1);
        m_dependencies.push_back(std::unique_ptr<QmlBindingNode>(new QmlBindingNode(context->contextObject(), propertyIndex, this)));
    }
    for (quint32 i = 0; i < compiledData->nDependingScopeProperties; ++i) {
        auto propertyIndex = *(scopePropertiesDependencyTable + 2*i);
        auto notifyIndex = *(scopePropertiesDependencyTable + 2*i + 1);
        auto dependencyNode = new QmlBindingNode(m_object, propertyIndex, this);
        m_dependencies.push_back(std::unique_ptr<QmlBindingNode>(dependencyNode));
    }



    // Dynamic dependencies
    auto guards = m_binding->activeGuards;
    QQmlJavaScriptExpressionGuard *guard = guards.first();
    while (guard) {
        if (guard->signalIndex() > 0) { // FIXME: is sender is a notifier, signalIndex isn't null, but uninitialized! This is going to crash!!!!!
            QObject *senderObject = guard->senderAsObject();
            if (!senderObject)
                continue;
            const QMetaObject *senderMeta = senderObject->metaObject();
            if (!senderMeta)
                continue;

            for (int i = 0; i < senderMeta->propertyCount(); i++) {
                QMetaProperty property = senderMeta->property(i);
                if (property.notifySignalIndex() == Util::signalIndexToMethodIndex(senderMeta, guard->signalIndex())) {
                    m_dependencies.push_back(std::unique_ptr<QmlBindingNode>(new QmlBindingNode(senderObject, i, this)));
                }
            }
        } else {

        }
        guard = guards.next(guard);
    }

    std::sort(m_dependencies.begin(), m_dependencies.end(), [](std::unique_ptr<QmlBindingNode> &a, std::unique_ptr<QmlBindingNode> &b) {
        return *a < *b;
    });
#endif
}

bool QmlBindingNode::operator<(const QmlBindingNode& other) const
{
    if (m_object == other.m_object) {
        return m_propertyIndex < other.m_propertyIndex;
    }
    return m_object < other.m_object;
}

bool QmlBindingNode::operator>(const QmlBindingNode& other) const
{
    if (m_object == other.m_object) {
        return m_propertyIndex > other.m_propertyIndex;
    }
    return m_object > other.m_object;
}

GammaRay::QmlBindingNode * GammaRay::QmlBindingNode::parent() const
{
    return m_parent;
}

void GammaRay::QmlBindingNode::setParent(GammaRay::QmlBindingNode* parent)
{
    m_parent = parent;
}

const QString &QmlBindingNode::expression() const
{
    return m_expression;
}

QQmlAbstractBinding *QmlBindingNode::binding() const
{
    return m_binding;
}

QObject *QmlBindingNode::object() const
{
    return m_object;
}

const QString & GammaRay::QmlBindingNode::id() const
{
    return m_id;
}

bool QmlBindingNode::isBinding() const
{
    return static_cast<bool>(m_binding);
}

bool GammaRay::QmlBindingNode::isActive() const
{
    return m_isActive;
}

bool QmlBindingNode::isBindingLoop() const
{
    return m_isBindingLoop;
}

int QmlBindingNode::propertyIndex() const
{
    return m_propertyIndex;
}

QMetaProperty GammaRay::QmlBindingNode::property() const
{
    return m_object->metaObject()->property(m_propertyIndex);
}

const QVariant &QmlBindingNode::value() const
{
    return m_value;
}

void QmlBindingNode::refreshValue()
{
    m_value = m_object->metaObject()->property(m_propertyIndex).read(m_object);
}

const SourceLocation &QmlBindingNode::sourceLocation() const
{
    return m_sourceLocation;
}

const std::vector<std::unique_ptr<QmlBindingNode>> &QmlBindingNode::dependencies() const
{
    return m_dependencies;
}

std::vector<std::unique_ptr<QmlBindingNode>> &QmlBindingNode::dependencies()
{
    return m_dependencies;
}

uint GammaRay::QmlBindingNode::depth() const
{
    uint depth = 0;
    if (m_isBindingLoop) {
        return std::numeric_limits<uint>::max(); // to be considered as infinity.
    }
    for (const auto &dependency : m_dependencies) {
        if (!dependency->isActive())
            continue;
        uint depDepth = dependency->depth();
        if (depDepth == std::numeric_limits<uint>::max()) {
            depth = depDepth;
            break;
        } else if (depDepth + 1 > depth) {
            depth = depDepth + 1;
        }
    }
    return depth;
}

