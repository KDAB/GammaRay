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
#include <private/qqmldata_p.h>
#include <private/qqmlproperty_p.h>
#include <QFile>

using namespace GammaRay;

BindingNode::BindingNode(QObject *object, int propertyIndex, BindingNode *parent)
    : m_object(object)
    , m_propertyIndex(propertyIndex)
    , m_parent(parent)
{
    QQmlContext *ctx = QQmlEngine::contextForObject(object);
    QString propertyName = object->metaObject() ? object->metaObject()->property(propertyIndex).name() : ":(";
    if (ctx) {
        QString id = ctx->nameForObject(object);
        m_name = id.isEmpty() ? propertyName : QStringLiteral("%1.%2").arg(id, propertyName);
    } else {
        m_name = propertyName;
    }
    refreshValue();
    checkForLoops();
}

std::unique_ptr<BindingNode> BindingNode::refresh()
{
    auto clone = std::unique_ptr<BindingNode>(new BindingNode(m_object, m_propertyIndex, m_parent)); // Will refresh value, dependencies, etc.
    clone->m_id = m_id;
    clone->m_isActive = m_isActive;
    clone->m_expression = m_expression;
    clone->m_sourceLocation = m_sourceLocation;
    return clone;
}

void BindingNode::checkForLoops()
{
    BindingNode *ancestor = m_parent;
    while (ancestor) {
        if (ancestor->object() == m_object
            && ancestor->propertyIndex() == propertyIndex()) {
            qDebug() << "Found a binding loop!";
            m_isBindingLoop = true;
            return;
        }
        ancestor = ancestor->parent();
    }
    m_isBindingLoop = false;
}

bool BindingNode::operator<(const BindingNode & other) const
{
    if (m_object == other.m_object) {
        return propertyIndex() < other.propertyIndex();
    }
    return m_object < other.m_object;
}

bool BindingNode::operator>(const BindingNode & other) const
{
    if (m_object == other.m_object) {
        return propertyIndex() > other.propertyIndex();
    }
    return m_object > other.m_object;
}

BindingNode *BindingNode::parent() const
{
    return m_parent;
}

void BindingNode::setParent(BindingNode * parent)
{
    m_parent = parent;
}

const QString &BindingNode::expression() const
{
    return m_expression;
}

QObject *BindingNode::object() const
{
    return m_object;
}

const QString &BindingNode::name() const
{
    return m_name;
}

qintptr BindingNode::id() const
{
    return m_id;
}

bool BindingNode::isActive() const
{
    return m_isActive;
}

bool BindingNode::isBindingLoop() const
{
    return m_isBindingLoop;
}

int BindingNode::propertyIndex() const
{
    return m_propertyIndex;
}

QMetaProperty BindingNode::property() const
{
    return m_object->metaObject()->property(propertyIndex());
}

const QVariant &BindingNode::value() const
{
    return m_value;
}

void BindingNode::refreshValue()
{
    m_value = m_object->metaObject()->property(propertyIndex()).read(m_object);
}

const SourceLocation &BindingNode::sourceLocation() const
{
    return m_sourceLocation;
}

const std::vector<std::unique_ptr<BindingNode>> &BindingNode::dependencies() const
{
    return m_dependencies;
}

std::vector<std::unique_ptr<BindingNode>> &BindingNode::dependencies()
{
    return m_dependencies;
}

void GammaRay::BindingNode::addDependency(std::unique_ptr<BindingNode> dependency)
{
    m_dependencies.push_back(std::move(dependency));
}

void BindingNode::clearDependencies()
{
    m_dependencies.clear();
}

uint BindingNode::depth() const
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

void GammaRay::BindingNode::setActive(bool active)
{
    m_isActive = active;
}

void GammaRay::BindingNode::setName(const QString &name)
{
    m_name = name;
}

void GammaRay::BindingNode::setId(qintptr id)
{
    m_id = id;
}

void GammaRay::BindingNode::setIsBindingLoop(bool isLoop)
{
    m_isBindingLoop = isLoop;
}


void GammaRay::BindingNode::setExpression(const QString &expression)
{
    m_expression = expression;
}

void BindingNode::setSourceLocation(const SourceLocation &location)
{
    m_sourceLocation = location;
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

std::unique_ptr<BindingNode> QmlBindingProvider::bindingNodeFromQmlProperty(QQmlProperty property, BindingNode * parent)
{
    auto node = std::unique_ptr<BindingNode>(new BindingNode(property.object(), property.index(), parent));
    auto binding = bindingForProperty(property.object(), property.index());

    if (binding) {
        fetchSourceLocationFor(node.get(), binding);
    }
    return node;
}

bool QmlBindingProvider::canProvideBindingsFor(QObject *object)
{
    return QQmlData::get(object);
}

void QmlBindingProvider::fetchSourceLocationFor(BindingNode *node, QQmlAbstractBinding *abstractBinding)
{
    QQmlBinding *binding = dynamic_cast<QQmlBinding*>(abstractBinding);
    if (!binding)
        return;

    QV4::Function *function = binding->function();
    QQmlSourceLocation loc = function->sourceLocation();
    node->setSourceLocation(SourceLocation(QUrl(loc.sourceFile), loc.line, loc.column));

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

std::vector<AbstractBindingProvider::Dependency> QmlBindingProvider::findDependenciesFor(BindingNode *node)
{
    std::vector<AbstractBindingProvider::Dependency> dependencies;
    if (node->isBindingLoop()) // Don't look for further dependencies, if this is already known to be part of a loop.
        return dependencies;

    QQmlAbstractBinding *binding = bindingForProperty(node->object(), node->propertyIndex());
    if (!binding)
        return dependencies;

    fetchSourceLocationFor(node, binding); // While we have the QQmlBinding at hand, let's grab the source location
    for (const auto &dependency : binding->dependencies()) {
//         node->addDependency(std::unique_ptr<BindingNode>(new BindingNode(dependency.object(), dependency.index(), node)));
        dependencies.push_back({ dependency.object(), dependency.index() });
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

std::vector<AbstractBindingProvider::Dependency> QuickImplicitBindingDependencyProvider::findDependenciesFor(BindingNode *binding)
{
    std::vector<AbstractBindingProvider::Dependency> dependencies;
    // So far, we can only hard code implicit dependencies.
    qDebug() << "looking for implicitDependencies.";
    QObject *object = binding->object();
    if (!object)
        return dependencies;
    if (QQuickItem *item = qobject_cast<QQuickItem*>(object)) {
        QQuickItemPrivate *itemPriv = QQuickItemPrivate::get(item);
        qDebug() << "Have an Item (" << binding->propertyIndex() << item->metaObject()->property(binding->propertyIndex()).name() << ")";
        if (binding->propertyIndex() == item->metaObject()->indexOfProperty("width")) {
            if (!itemPriv->widthValid) {
                dependencies.push_back({ object, object->metaObject()->indexOfProperty("implicitWidth") });
            }
        } else if (binding->propertyIndex() == item->metaObject()->indexOfProperty("height")) {
            if (!itemPriv->heightValid) {
                dependencies.push_back({ object, object->metaObject()->indexOfProperty("implicitHeight") });
            }
        } else if (binding->propertyIndex() == item->metaObject()->indexOfProperty("implicitWidth")) {
            if (item->inherits("QQuickBasePositioner")) {
                for (QQuickItem *child : item->childItems()) {
                    dependencies.push_back({ child, child->metaObject()->indexOfProperty("width") });
                }
            }
        } else if (binding->propertyIndex() == item->metaObject()->indexOfProperty("implicitHeight")) {
            if (item->inherits("QQuickBasePositioner")) {
                for (QQuickItem *child : item->childItems()) {
                    dependencies.push_back({ child, child->metaObject()->indexOfProperty("height") });
                }
            }
        } else if (binding->propertyIndex() == item->metaObject()->indexOfProperty("x")) {
        } else if (binding->propertyIndex() == item->metaObject()->indexOfProperty("y")) {
        } else if (binding->propertyIndex() == item->metaObject()->indexOfProperty("childrenRect")) {
            for (auto &&child : item->childItems()) {
                dependencies.push_back({ child, child->metaObject()->indexOfProperty("width") });
                dependencies.push_back({ child, child->metaObject()->indexOfProperty("height") });
            }
        } else if (binding->propertyIndex() == item->metaObject()->indexOfProperty("top") && itemPriv->anchors()) {
            dependencies.push_back({ itemPriv->anchors(), itemPriv->anchors()->metaObject()->indexOfProperty("top") });
        } else if (binding->propertyIndex() == item->metaObject()->indexOfProperty("bottom") && itemPriv->anchors()) {
            dependencies.push_back({ itemPriv->anchors(), itemPriv->anchors()->metaObject()->indexOfProperty("bottom") });
        } else if (binding->propertyIndex() == item->metaObject()->indexOfProperty("left") && itemPriv->anchors()) {
            dependencies.push_back({ itemPriv->anchors(), itemPriv->anchors()->metaObject()->indexOfProperty("left") });
        } else if (binding->propertyIndex() == item->metaObject()->indexOfProperty("right") && itemPriv->anchors()) {
            dependencies.push_back({ itemPriv->anchors(), itemPriv->anchors()->metaObject()->indexOfProperty("right") });
        } else if (binding->propertyIndex() == item->metaObject()->indexOfProperty("horizontalCenter") && itemPriv->anchors()) {
            dependencies.push_back({ itemPriv->anchors(), itemPriv->anchors()->metaObject()->indexOfProperty("horizontalCenter") });
        } else if (binding->propertyIndex() == item->metaObject()->indexOfProperty("verticalCenter") && itemPriv->anchors()) {
            dependencies.push_back({ itemPriv->anchors(), itemPriv->anchors()->metaObject()->indexOfProperty("verticalCenter") });
        } else if (binding->propertyIndex() == item->metaObject()->indexOfProperty("baseline") && itemPriv->anchors()) {
            dependencies.push_back({ itemPriv->anchors(), itemPriv->anchors()->metaObject()->indexOfProperty("baseline") });
        }
    }
    if (QQuickAnchors *anchors = qobject_cast<QQuickAnchors*>(object)) {
        QQuickAnchorLine anchorLine = object->metaObject()->property(binding->propertyIndex()).read(object).value<QQuickAnchorLine>();
        auto dependencyPropertyName = anchorLine.anchorLine == QQuickAnchors::TopAnchor ? "top"
                                    : anchorLine.anchorLine == QQuickAnchors::BottomAnchor ? "bottom"
                                    : anchorLine.anchorLine == QQuickAnchors::LeftAnchor ? "left"
                                    : anchorLine.anchorLine == QQuickAnchors::RightAnchor ? "right"
                                    : anchorLine.anchorLine == QQuickAnchors::HCenterAnchor ? "horizontalCenter"
                                    : anchorLine.anchorLine == QQuickAnchors::VCenterAnchor ? "verticalCenter"
                                    : anchorLine.anchorLine == QQuickAnchors::BaselineAnchor ? "baseline"
                                    : "";
        if (anchorLine.item) {
            dependencies.push_back({ anchorLine.item, anchorLine.item->metaObject()->indexOfProperty(dependencyPropertyName) });
        }
    }
    return dependencies;
}
