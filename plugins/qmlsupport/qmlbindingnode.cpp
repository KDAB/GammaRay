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
#include "qmlbindingextension.h"
#include <core/util.h>

#include <QDebug>

#include <QtQuick/QQuickItem>
#include <private/qquickitem_p.h>
#include <private/qquickitemchangelistener_p.h>

#include <private/qqmlabstractbinding_p.h>
#include <private/qqmlbinding_p.h>
#include <private/qqmlvaluetypeproxybinding_p.h>
#include <private/qqmldata_p.h>
#include <private/qqmlproperty_p.h>
#include <QFile>

using namespace GammaRay;

BindingNode::BindingNode(QObject *obj, int propIndex, BindingNode *parent)
    : parent(parent)
    , object(obj)
    , propertyIndex(propIndex)
{
    Q_ASSERT(obj);
    canonicalName = object->metaObject() ? object->metaObject()->property(propertyIndex).name() : ":(";
    refreshValue();
    checkForLoops();
}

BindingNode::BindingNode(QObject *object, int propertyIndex, qintptr id_)
    : BindingNode(object, propertyIndex, nullptr)
{
    id = id_;
}

BindingNode::BindingNode(const BindingNode &other)
    : parent(other.parent)
    , object(other.object)
    , propertyIndex(other.propertyIndex)
    , id(other.id)
    , canonicalName(other.canonicalName)
    , value(other.value)
    , isActive(other.isActive)
    , isBindingLoop(other.isBindingLoop)
    , expression(other.expression)
    , sourceLocation(other.sourceLocation)
{
    dependencies.reserve(other.dependencies.size());
    for (auto &&dependency : other.dependencies) {
        dependencies.push_back(std::unique_ptr<BindingNode>(new BindingNode(*dependency)));
    }
}

void BindingNode::checkForLoops()
{
    BindingNode *ancestor = parent;
    while (ancestor) {
        if (ancestor->object == object
            && ancestor->propertyIndex == propertyIndex) {
            qDebug() << "Found a binding loop!";
            isBindingLoop = true;
            return;
        }
        ancestor = ancestor->parent;
    }
    isBindingLoop = false;
}

QMetaProperty BindingNode::property() const
{
    return object->metaObject()->property(propertyIndex);
}
void BindingNode::refreshValue()
{
    value = object->metaObject()->property(propertyIndex).read(object);
}

uint BindingNode::depth() const
{
    uint depth = 0;
    if (isBindingLoop) {
        return std::numeric_limits<uint>::max(); // to be considered as infinity.
    }
    for (const auto &dependency : dependencies) {
        if (!dependency->isActive)
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

AbstractBindingProvider::~AbstractBindingProvider()
{
}

QQmlAbstractBinding *QmlBindingProvider::bindingForProperty(QObject *obj, int propertyIndex)
{
    auto data = QQmlData::get(obj);
    if (!data || !data->hasBindingBit(propertyIndex))
        return Q_NULLPTR;

    auto b = data->bindings;
    while (b) {
        int index;
        index = b->targetPropertyIndex().coreIndex();

        if (index == propertyIndex) {
            return b;
        }
        b = b->nextBinding();
    }
    return Q_NULLPTR;
}

// std::unique_ptr<BindingNode> QmlBindingProvider::bindingNodeFromQmlProperty(QQmlProperty property, BindingNode * parent)
// {
//     auto node = std::unique_ptr<BindingNode>(new BindingNode(property.object(), property.index(), parent));
//     auto binding = QQmlPropertyPrivate::binding(property.object(), QQmlPropertyIndex::fromEncoded(property.index()));
//
//     if (binding) {
//         fetchSourceLocationFor(node.get(), binding);
//     }
//     return node;
// }

bool QmlBindingProvider::canProvideBindingsFor(QObject *object)
{
    return QQmlData::get(object);
}

void QmlBindingProvider::fetchSourceLocationFor(BindingNode *node, QQmlBinding *binding)
{
    QV4::Function *function = binding->function();
    QQmlSourceLocation loc = function->sourceLocation();
    node->sourceLocation = SourceLocation(QUrl(loc.sourceFile), loc.line, loc.column);

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

std::vector<std::unique_ptr<BindingNode>> QmlBindingProvider::findDependenciesFor(BindingNode *node)
{
    std::vector<std::unique_ptr<BindingNode>> dependencies;
    if (node->isBindingLoop) // Don't look for further dependencies, if this is already known to be part of a loop.
        return dependencies;

    QQmlAbstractBinding *abstractBinding = QQmlPropertyPrivate::binding(node->object, QQmlPropertyIndex::fromEncoded(node->propertyIndex));
    QQmlBinding *binding = dynamic_cast<QQmlBinding*>(abstractBinding);
    if (!binding)
        return dependencies;

    fetchSourceLocationFor(node, binding); // While we have the QQmlBinding at hand, let's grab the source location
    for (const auto &dependency : binding->dependencies()) {
        dependencies.push_back(std::unique_ptr<BindingNode>(new BindingNode(dependency.object(), dependency.index(), node)));
    }
    return dependencies;
}

std::vector<std::unique_ptr<BindingNode>> QmlBindingProvider::findBindingsFor(QObject *obj)
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
                node->canonicalName = QStringLiteral("%1.%2").arg(id, node->canonicalName);
        }

        bindings.push_back(std::unique_ptr<BindingNode>(node));
        b = b->nextBinding();
    }
    return bindings;
}

std::vector<std::unique_ptr<BindingNode>> QuickImplicitBindingDependencyProvider::findBindingsFor(QObject *obj)
{
    return {};
}

bool QuickImplicitBindingDependencyProvider::canProvideBindingsFor(QObject *object)
{
    return false;
}

std::unique_ptr<BindingNode> GammaRay::QuickImplicitBindingDependencyProvider::createBindingNode(QObject* obj, const char *propertyName) const
{
    if (!obj || !obj->metaObject())
        return {};

    int propertyIndex = obj->metaObject()->indexOfProperty(propertyName);
    return std::unique_ptr<BindingNode>(new BindingNode(obj, propertyIndex));
}

std::vector<std::unique_ptr<BindingNode>> QuickImplicitBindingDependencyProvider::findDependenciesFor(BindingNode *binding)
{
    std::vector<std::unique_ptr<BindingNode>> dependencies;
    // So far, we can only hard code implicit dependencies.
    qDebug() << "looking for implicitDependencies.";
    QObject *object = binding->object;
    if (!object)
        return dependencies;
    if (QQuickItem *item = qobject_cast<QQuickItem*>(object)) {
        QQuickItemPrivate *itemPriv = QQuickItemPrivate::get(item);
        qDebug() << "Have an Item (" << binding->propertyIndex << item->metaObject()->property(binding->propertyIndex).name() << ")";
        if (binding->propertyIndex == item->metaObject()->indexOfProperty("width")) {
            if (!itemPriv->widthValid) {
                dependencies.push_back(createBindingNode(object, "implicitWidth"));
            }
        } else if (binding->propertyIndex == item->metaObject()->indexOfProperty("height")) {
            if (!itemPriv->heightValid) {
                dependencies.push_back(createBindingNode(object, "implicitHeight"));
            }
        } else if (binding->propertyIndex == item->metaObject()->indexOfProperty("implicitWidth")) {
            if (item->inherits("QQuickBasePositioner")) {
                for (QQuickItem *child : item->childItems()) {
                    dependencies.push_back(createBindingNode(child, "width"));
                }
            }
        } else if (binding->propertyIndex == item->metaObject()->indexOfProperty("implicitHeight")) {
            if (item->inherits("QQuickBasePositioner")) {
                for (QQuickItem *child : item->childItems()) {
                    dependencies.push_back(createBindingNode(child, "height"));
                }
            }
        } else if (binding->propertyIndex == item->metaObject()->indexOfProperty("x")) {
        } else if (binding->propertyIndex == item->metaObject()->indexOfProperty("y")) {
        } else if (binding->propertyIndex == item->metaObject()->indexOfProperty("childrenRect")) {
            for (auto &&child : item->childItems()) {
                dependencies.push_back(createBindingNode(child, "width"));
                dependencies.push_back(createBindingNode(child, "height"));
            }
        } else if (binding->propertyIndex == item->metaObject()->indexOfProperty("top") && itemPriv->anchors()) {
            dependencies.push_back(createBindingNode(itemPriv->anchors(), "top"));
        } else if (binding->propertyIndex == item->metaObject()->indexOfProperty("bottom") && itemPriv->anchors()) {
            dependencies.push_back(createBindingNode(itemPriv->anchors(), "bottom"));
        } else if (binding->propertyIndex == item->metaObject()->indexOfProperty("left") && itemPriv->anchors()) {
            dependencies.push_back(createBindingNode(itemPriv->anchors(), "left"));
        } else if (binding->propertyIndex == item->metaObject()->indexOfProperty("right") && itemPriv->anchors()) {
            dependencies.push_back(createBindingNode(itemPriv->anchors(), "right"));
        } else if (binding->propertyIndex == item->metaObject()->indexOfProperty("horizontalCenter") && itemPriv->anchors()) {
            dependencies.push_back(createBindingNode(itemPriv->anchors(), "horizontalCenter"));
        } else if (binding->propertyIndex == item->metaObject()->indexOfProperty("verticalCenter") && itemPriv->anchors()) {
            dependencies.push_back(createBindingNode(itemPriv->anchors(), "verticalCenter"));
        } else if (binding->propertyIndex == item->metaObject()->indexOfProperty("baseline") && itemPriv->anchors()) {
            dependencies.push_back(createBindingNode(itemPriv->anchors(), "baseline"));
        }
    }
    if (QQuickAnchors *anchors = qobject_cast<QQuickAnchors*>(object)) {
        QQuickAnchorLine anchorLine = object->metaObject()->property(binding->propertyIndex).read(object).value<QQuickAnchorLine>();
        auto dependencyPropertyName = anchorLine.anchorLine == QQuickAnchors::TopAnchor ? "top"
                                    : anchorLine.anchorLine == QQuickAnchors::BottomAnchor ? "bottom"
                                    : anchorLine.anchorLine == QQuickAnchors::LeftAnchor ? "left"
                                    : anchorLine.anchorLine == QQuickAnchors::RightAnchor ? "right"
                                    : anchorLine.anchorLine == QQuickAnchors::HCenterAnchor ? "horizontalCenter"
                                    : anchorLine.anchorLine == QQuickAnchors::VCenterAnchor ? "verticalCenter"
                                    : anchorLine.anchorLine == QQuickAnchors::BaselineAnchor ? "baseline"
                                    : "";
        if (anchorLine.item) {
            dependencies.push_back(createBindingNode(anchorLine.item, dependencyPropertyName));
        }
    }
    return dependencies;
}
