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

#define private public
#define protected public
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

QQmlBinding *QmlBindingNode::bindingForProperty(QObject *obj, int propertyIndex)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    auto data = QQmlData::get(obj);
    if (!data)
        return 0;

    auto b = data->bindings;
    while (b) {
        auto binding = dynamic_cast<QQmlBinding*>(b);
        if (!binding)
            return 0;
        int index;
#if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)
        QQmlPropertyData::decodeValueTypePropertyIndex(b->targetPropertyIndex(), &index);
#else
        index = b->targetPropertyIndex().coreIndex();
#endif

        if (index == propertyIndex) {
            return binding;
        }
        b = b->nextBinding();
    }
#endif
    return 0;
}

QmlBindingNode::QmlBindingNode(QObject* object, int propertyIndex, QmlBindingNode *parent)
    : m_parent(parent)
    , m_object(object)
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
    m_value = object->metaObject()->property(propertyIndex).read(object);
}


QmlBindingNode::QmlBindingNode(QQmlBinding* binding, QmlBindingNode *parent)
    : m_parent(parent)
    , m_object(binding->targetObject())
    , m_binding(binding)

{
#if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)
    QQmlPropertyData::decodeValueTypePropertyIndex(binding->targetPropertyIndex(), &m_propertyIndex);
#else
    m_propertyIndex = m_binding->targetPropertyIndex().coreIndex();
#endif
    m_value = m_object->metaObject()->property(m_propertyIndex).read(m_object);

    fetchBindingCode();
    checkForLoops();
    findDependencies();
}

void GammaRay::QmlBindingNode::fetchPropertyCode()
{
    auto qmldata = QQmlData::get(m_object);
//     qmldata->;
}

void GammaRay::QmlBindingNode::fetchBindingCode()
{
#if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)
    QQmlEnginePrivate *ep = QQmlEnginePrivate::get(QQmlData::get(m_object)->context->engine);
    QV4::Scope scope(ep->v4engine());
    QV4::ScopedValue f(scope, m_binding->m_function.value());
    QV4::Function *function = f->as<QV4::FunctionObject>()->function();
#else
    QV4::Function *function = m_binding->function();
    QQmlSourceLocation loc = function->sourceLocation();
    m_sourceLocation = SourceLocation(QUrl(loc.sourceFile), loc.line, loc.column);
#endif

//     qDebug() << "Trying to get code from" << function->compilationUnit->fileName() << ", pos:"
//              << function->compiledFunction->codeOffset << function->compiledFunction->codeSize;
    QFile codeFile(QUrl(function->compilationUnit->fileName()).toLocalFile());
    if (!codeFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Can't open file :(";
        return;
    }

    codeFile.waitForReadyRead(1000);
    for (int i = 1; i < function->compiledFunction->location.line; i++)
        qDebug() << codeFile.readLine();
    if (!codeFile.canReadLine()) {
        qDebug() << "File ends before line" << function->compiledFunction->location.line;
    }

    m_expression = QString(codeFile.readLine()).trimmed();
    qDebug() << "Got expression" << m_expression;
}

void GammaRay::QmlBindingNode::checkForLoops()
{
    QmlBindingNode *ancestor = m_parent;
    while (ancestor) {
        if (ancestor->object() == m_object
            && ancestor->propertyIndex() == m_propertyIndex) {
            qDebug() << "Found a binding loop!";
            m_isBindingLoop = true;
        }
        ancestor = ancestor->parent();
    }
}

void GammaRay::QmlBindingNode::findDependencies()
{
    if (m_isBindingLoop) // Don't look for further dependencies, if this is already known to be part of a loop.
        return;

    auto context = QQmlEngine::contextForObject(m_object);
    QQmlEnginePrivate *ep = QQmlEnginePrivate::get(QQmlData::get(m_object)->context->engine);
    QV4::Scope scope(ep->v4engine());
#if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)
    QV4::ScopedValue f(scope, m_binding->m_function.value());
    QV4::Function *function = f->as<QV4::FunctionObject>()->function();
#else
    QV4::Function *function = m_binding->function();
#endif
    const QV4::CompiledData::Function *compiledData = function->compiledFunction;
    qDebug() << "Binding:" << function->compilationUnit->data->sourceFileIndex; //->stringAt(compiledData->codeOffset);
    const auto idObjectDependencyTable = compiledData->qmlIdObjectDependencyTable();
    const auto contextPropertiesDependencyTable = compiledData->qmlContextPropertiesDependencyTable();
    const auto scopePropertiesDependencyTable = compiledData->qmlScopePropertiesDependencyTable();
    for (int i = 0; i < compiledData->nDependingIdObjects; ++i) {
        auto idObjectIndex = *(idObjectDependencyTable + i);
//         auto idObject = function->compilationUnit->objectAt(idObjectIndex);
//         qDebug() << "idObjectDependency found:" << idObjectIndex << idObject->nProperties << idObject->nBindings << function->compilationUnit->stringAt(idObject->idNameIndex);
    }
    for (int i = 0; i < compiledData->nDependingContextProperties; ++i) {
        auto propertyIndex = *(contextPropertiesDependencyTable + 2*i);
        auto notifyIndex = *(contextPropertiesDependencyTable + 2*i + 1);
        m_dependencies.push_back(std::unique_ptr<QmlBindingNode>(new QmlBindingNode(context->contextObject(), propertyIndex, this)));
        qDebug() << "contextPropertyDependency found:" << propertyIndex << notifyIndex << context->contextObject()->metaObject()->property(propertyIndex).name();
    }
    for (int i = 0; i < compiledData->nDependingScopeProperties; ++i) {
        auto propertyIndex = *(scopePropertiesDependencyTable + 2*i);
        auto notifyIndex = *(scopePropertiesDependencyTable + 2*i + 1);
        auto dependencyNode = new QmlBindingNode(m_object, propertyIndex, this);
        m_dependencies.push_back(std::unique_ptr<QmlBindingNode>(dependencyNode));
        qDebug() << "scopePropertyDependency found:" << propertyIndex << notifyIndex << m_object->metaObject()->property(propertyIndex).name();

//         dependencyNode->m_isActive = false;
//         auto guards = m_binding->permanentGuards;
//         QQmlJavaScriptExpressionGuard *guard = guards.first();
//         while (guard) {
//             if (guard->senderAsObject() == m_object && guard->signalIndex() == notifyIndex) {
//                 dependencyNode->m_isActive = true;
//             }
//             guard = guards.next(guard);
//         }
    }



//     // Find active dependencies
// // #if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)
//     auto guards = m_binding->activeGuards;
// // #else
// //     auto guards = m_binding->permanentGuards;
// // #endif
//     QQmlJavaScriptExpressionGuard *guard = guards.first();
//     while (guard) {
//         if (guard->signalIndex() > 0) { // FIXME: is sender is a notifier, signalIndex isn't null, but uninitialized! This is going to crash!!!!!
//             QObject *senderObject = guard->senderAsObject();
//             if (!senderObject)
//                 continue;
//             const QMetaObject *senderMeta = senderObject->metaObject();
//             if (!senderMeta)
//                 continue;
//
//             for (int i = 0; i < senderMeta->propertyCount(); i++) {
//                 QMetaProperty property = senderMeta->property(i);
//                 if (property.notifySignalIndex() == Util::signalIndexToMethodIndex(senderMeta, guard->signalIndex())) {
//                     m_dependencies.push_back(std::unique_ptr<QmlBindingNode>(new QmlBindingNode(senderObject, i)));
//                 }
//             }
//         } else {
//
//         }
//         guard = guards.next(guard);
//     }
}

GammaRay::QmlBindingNode * GammaRay::QmlBindingNode::parent() const
{
    return m_parent;
}

const QString &QmlBindingNode::expression() const
{
    return m_expression;
}

QQmlBinding *QmlBindingNode::binding() const
{
    return m_binding;
}

QObject *QmlBindingNode::object() const
{
    return m_object;
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

const QVariant & GammaRay::QmlBindingNode::value() const
{
    return m_value;
}

const SourceLocation &QmlBindingNode::sourceLocation() const
{
    return m_sourceLocation;
}

const std::vector<std::unique_ptr<QmlBindingNode>> &QmlBindingNode::dependencies() const
{
    return m_dependencies;
}

uint GammaRay::QmlBindingNode::depth() const
{
    int depth = 0;
    if (m_isBindingLoop) {
        return std::numeric_limits<uint>::max(); // to be considered as infinity.
    }
    for (const auto &dependency : m_dependencies) {
        int depDepth = dependency->depth();
        if (depDepth == std::numeric_limits<int>::max()) {
            depth = depDepth;
            break;
        } else if (depDepth + 1 > depth) {
            depth = depDepth + 1;
        }
    }
    return depth;
}

