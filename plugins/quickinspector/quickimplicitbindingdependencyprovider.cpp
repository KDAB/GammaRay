/*
  quickimplicitbindingdependencyprovider.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

// Own
#include "quickimplicitbindingdependencyprovider.h"

#include <core/util.h>
#include <core/bindingnode.h>

// Qt
#include <QDebug>

#include <QtQuick/QQuickItem>
#include <private/qquickitem_p.h>
#include <private/qquickitemchangelistener_p.h>

using namespace GammaRay;

std::unique_ptr<BindingNode> GammaRay::QuickImplicitBindingDependencyProvider::createBindingNode(QObject* obj, const char *propertyName, BindingNode *parent) const
{
    if (!obj || !obj->metaObject())
        return {};

    QQmlProperty qmlProperty(obj, propertyName);
    auto node = std::unique_ptr<BindingNode>(new BindingNode(qmlProperty.object(), qmlProperty.index(), parent));
    QQmlContext *ctx = QQmlEngine::contextForObject(obj);
    QString canonicalName = propertyName;
    if (ctx) {
        QString id = ctx->nameForObject(obj);
        if (!id.isEmpty())
            canonicalName = QStringLiteral("%1.%2").arg(id, canonicalName);
    }
    node->setCanonicalName(canonicalName);
    return node;
}

bool QuickImplicitBindingDependencyProvider::canProvideBindingsFor(QObject *object) const
{
    return object->inherits("QQuickAnchors") || object->inherits("QQuickItem");
}

std::vector<std::unique_ptr<BindingNode>> QuickImplicitBindingDependencyProvider::findBindingsFor(QObject *obj) const
{
    std::vector<std::unique_ptr<BindingNode>> bindings;

    if (QQuickItem *item = qobject_cast<QQuickItem*>(obj)) { //FIXME: Check for QQuickAnchors directly here, as soon as we show properties of object-properties.
        QQuickItemPrivate *itemPriv = QQuickItemPrivate::get(item);
        if (!itemPriv)
            return bindings;
        QQuickAnchors *anchors = itemPriv->_anchors;
        if (!anchors)
            return bindings;

        auto usedAnchors = anchors->usedAnchors();
        if (usedAnchors & QQuickAnchors::TopAnchor)      bindings.push_back(createBindingNode(item, "anchors.top"));
        if (usedAnchors & QQuickAnchors::BottomAnchor)   bindings.push_back(createBindingNode(item, "anchors.bottom"));
        if (usedAnchors & QQuickAnchors::LeftAnchor)     bindings.push_back(createBindingNode(item, "anchors.left"));
        if (usedAnchors & QQuickAnchors::RightAnchor)    bindings.push_back(createBindingNode(item, "anchors.right"));
        if (usedAnchors & QQuickAnchors::HCenterAnchor)  bindings.push_back(createBindingNode(item, "anchors.horizontalCenter"));
        if (usedAnchors & QQuickAnchors::VCenterAnchor)  bindings.push_back(createBindingNode(item, "anchors.verticalCenter"));
        if (usedAnchors & QQuickAnchors::BaselineAnchor) bindings.push_back(createBindingNode(item, "anchors.baseline"));
    }

    return bindings;
}

std::vector<std::unique_ptr<BindingNode>> QuickImplicitBindingDependencyProvider::findDependenciesFor(BindingNode *binding) const
{
    std::vector<std::unique_ptr<BindingNode>> dependencies;
    // So far, we can only hard code implicit dependencies.
    QObject *object = binding->object();
    if (!object)
        return dependencies;

    auto addDependency = [this, binding, object, &dependencies](const char *propName, QObject *depObj, const char *depName)
    {
        if (depObj && binding->propertyIndex() == object->metaObject()->indexOfProperty(propName)) {
            dependencies.push_back(createBindingNode(depObj, depName, binding));
        }
    };

    if (QQuickAnchors *anchors = qobject_cast<QQuickAnchors*>(object)) {
        anchorBindings(dependencies, anchors, binding->propertyIndex(), binding);
    }
    if (QQuickItem *item = qobject_cast<QQuickItem*>(object)) {
        implicitSizeDependencies(item, addDependency);
        anchoringDependencies(item, addDependency);
        if (binding->propertyIndex() == item->metaObject()->indexOfProperty("childrenRect")) {
            childrenRectDependencies(item, addDependency);
        }
        if (item->inherits("QQuickBasePositioner")) {
            positionerDependencies(item, addDependency);
        }
    }

    return dependencies;
}

void QuickImplicitBindingDependencyProvider::anchorBindings(std::vector<std::unique_ptr<BindingNode>> &dependencies,
                                                            QQuickAnchors *anchors, int propertyIndex,
                                                            BindingNode *parent) const
{
    QQuickAnchorLine anchorLine = anchors->metaObject()->property(propertyIndex).read(anchors).value<QQuickAnchorLine>();
    auto dependencyPropertyName = anchorLine.anchorLine == QQuickAnchors::TopAnchor ? "top"
                                : anchorLine.anchorLine == QQuickAnchors::BottomAnchor ? "bottom"
                                : anchorLine.anchorLine == QQuickAnchors::LeftAnchor ? "left"
                                : anchorLine.anchorLine == QQuickAnchors::RightAnchor ? "right"
                                : anchorLine.anchorLine == QQuickAnchors::HCenterAnchor ? "horizontalCenter"
                                : anchorLine.anchorLine == QQuickAnchors::VCenterAnchor ? "verticalCenter"
                                : anchorLine.anchorLine == QQuickAnchors::BaselineAnchor ? "baseline"
                                : "";
    if (anchorLine.item) {
        dependencies.push_back(createBindingNode(anchorLine.item, dependencyPropertyName, parent));
    }
}

template<class Func>
void QuickImplicitBindingDependencyProvider::implicitSizeDependencies(QQuickItem *item, Func addDependency) const
{
    QQuickItemPrivate *itemPriv = QQuickItemPrivate::get(item);
    if (!itemPriv)
        return;

    if (!itemPriv->widthValid) {
        addDependency("width", item, "implicitWidth");
    }
    if (!itemPriv->heightValid) {
        addDependency("height", item, "implicitHeight");
    }
}

template<class Func>
void QuickImplicitBindingDependencyProvider::childrenRectDependencies(QQuickItem *item, Func addDependency) const
{
    foreach (auto &&child, item->childItems()) {
        addDependency("childrenRect", child, "width");
        addDependency("childrenRect", child, "height");
    }
}
template<class Func>
void QuickImplicitBindingDependencyProvider::positionerDependencies(QQuickItem *item, Func addDependency) const
{
    foreach (QQuickItem *child, item->childItems()) {
        addDependency("implicitWidth", child, "width");
        addDependency("implicitHeight", child, "height");
    }
}

template<class Func>
void QuickImplicitBindingDependencyProvider::anchoringDependencies(QQuickItem *item, Func addDependency) const
{
    QQuickItemPrivate *itemPriv = QQuickItemPrivate::get(item);
    if (!itemPriv)
        return;
    QQuickAnchors *anchors = itemPriv->_anchors;

    // Horizontal
    if (anchors && anchors->fill()) {
        QQuickItem *fill = anchors->fill();
        addDependency("width", fill, "width");
        addDependency("width", item, "anchors.leftMargin");
        addDependency("width", item, "anchors.rightMargin");
        addDependency("x", fill, "left");
        addDependency("x", item, "anchors.leftMargin");
        addDependency("left", fill, "left");
        addDependency("left", item, "anchors.leftMargin");
        addDependency("right", fill, "right");
        addDependency("right", item, "anchors.rightMargin");
        addDependency("horizontalCenter", item, "left");
        addDependency("horizontalCenter", item, "right");
    } else if (anchors && anchors->centerIn()) {
        QQuickItem *centerIn = anchors->centerIn();
        addDependency("horizontalCenter", centerIn, "horizontalCenter");
        addDependency("x", centerIn, "horizontalCenter");
        addDependency("x", item, "width");
        addDependency("left", centerIn, "horizontalCenter");
        addDependency("left", item, "width");
        addDependency("right", centerIn, "horizontalCenter");
        addDependency("right", item, "width");
    } else if (anchors && anchors->left().anchorLine != QQuickAnchors::InvalidAnchor) {
        addDependency("left", item, "anchors.left");
        addDependency("x", item, "anchors.left");
        if (anchors->right().anchorLine != QQuickAnchors::InvalidAnchor) {
            addDependency("right", item, "anchors.right");
            addDependency("right", item, "anchors.rightMargin");
            addDependency("width", item, "anchors.left");
            addDependency("width", item, "anchors.right");
            addDependency("horizontalCenter", item, "anchors.left");
            addDependency("horizontalCenter", item, "anchors.right");
        } else if (anchors->horizontalCenter().anchorLine != QQuickAnchors::InvalidAnchor) {
            addDependency("horizontalCenter", item, "anchors.horizontalCenter");
            addDependency("width", item, "anchors.left");
            addDependency("width", item, "anchors.horizontalCenter");
            addDependency("right", item, "anchors.left");
            addDependency("right", item, "anchors.horizontalCenter");
        } else {
            addDependency("right", item, "anchors.left");
            addDependency("right", item, "width");
            addDependency("horizontalCenter", item, "anchors.left");
            addDependency("horizontalCenter", item, "width");
        }
    } else if (anchors && anchors->right().anchorLine != QQuickAnchors::InvalidAnchor) {
        addDependency("right", item, "anchors.right");
        if (anchors->horizontalCenter().anchorLine != QQuickAnchors::InvalidAnchor) {
            addDependency("horizontalCenter", item, "anchors.horizontalCenter");
            addDependency("width", item, "anchors.right");
            addDependency("width", item, "anchors.horizontalCenter");
            addDependency("x", item, "anchors.horizontalCenter");
            addDependency("x", item, "anchors.right");
            addDependency("left", item, "anchors.horizontalCenter");
            addDependency("left", item, "anchors.right");
        } else {
            addDependency("x", item, "anchors.right");
            addDependency("x", item, "width");
            addDependency("left", item, "anchors.right");
            addDependency("left", item, "width");
            addDependency("horizontalCenter", item, "anchors.right");
            addDependency("horizontalCenter", item, "width");
        }
    } else if (anchors && anchors->horizontalCenter().anchorLine != QQuickAnchors::InvalidAnchor) {
        addDependency("horizontalCenter", item, "anchors.horizontalCenter");
        addDependency("x", item, "anchors.horizontalCenter");
        addDependency("x", item, "width");
        addDependency("left", item, "anchors.horizontalCenter");
        addDependency("left", item, "width");
        addDependency("right", item, "anchors.horizontalCenter");
        addDependency("right", item, "width");
    } else {
        addDependency("left", item, "x");
        addDependency("right", item, "x");
        addDependency("right", item, "width");
        addDependency("horizontalCenter", item, "x");
        addDependency("horizontalCenter", item, "width");
    }


    // Vertical TODO: Bottomline
    if (anchors && anchors->fill()) {
        QQuickItem *fill = anchors->fill();
        addDependency("height", fill, "height");
        addDependency("height", item, "anchors.topMargin");
        addDependency("height", item, "anchors.bottomMargin");
        addDependency("y", fill, "top");
        addDependency("y", item, "anchors.topMargin");
        addDependency("top", fill, "top");
        addDependency("top", item, "anchors.topMargin");
        addDependency("bottom", fill, "bottom");
        addDependency("bottom", item, "anchors.bottomMargin");
        addDependency("verticalCenter", item, "top");
        addDependency("verticalCenter", item, "bottom");
    } else if (anchors && anchors->centerIn()) {
        QQuickItem *centerIn = anchors->centerIn();
        addDependency("verticalCenter", centerIn, "verticalCenter");
        addDependency("y", centerIn, "verticalCenter");
        addDependency("y", item, "height");
        addDependency("top", centerIn, "verticalCenter");
        addDependency("top", item, "height");
        addDependency("bottom", centerIn, "verticalCenter");
        addDependency("bottom", item, "height");
    } else if (anchors && anchors->top().anchorLine != QQuickAnchors::InvalidAnchor) {
        addDependency("top", item, "anchors.top");
        addDependency("y", item, "anchors.top");
        if (anchors->bottom().anchorLine != QQuickAnchors::InvalidAnchor) {
            addDependency("bottom", item, "anchors.bottom");
            addDependency("bottom", item, "anchors.bottomMargin");
            addDependency("height", item, "anchors.top");
            addDependency("height", item, "anchors.bottom");
            addDependency("verticalCenter", item, "anchors.top");
            addDependency("verticalCenter", item, "anchors.bottom");
        } else if (anchors->verticalCenter().anchorLine != QQuickAnchors::InvalidAnchor) {
            addDependency("verticalCenter", item, "anchors.verticalCenter");
            addDependency("height", item, "anchors.top");
            addDependency("height", item, "anchors.verticalCenter");
            addDependency("y", item, "anchors.top");
            addDependency("bottom", item, "anchors.top");
            addDependency("bottom", item, "anchors.verticalCenter");
        } else {
            addDependency("bottom", item, "anchors.top");
            addDependency("bottom", item, "height");
            addDependency("verticalCenter", item, "anchors.top");
            addDependency("verticalCenter", item, "height");
        }
    } else if (anchors && anchors->bottom().anchorLine != QQuickAnchors::InvalidAnchor) {
        addDependency("bottom", item, "anchors.bottom");
        if (anchors->verticalCenter().anchorLine != QQuickAnchors::InvalidAnchor) {
            addDependency("verticalCenter", item, "anchors.verticalCenter");
            addDependency("height", item, "anchors.bottom");
            addDependency("height", item, "anchors.verticalCenter");
            addDependency("y", item, "anchors.verticalCenter");
            addDependency("y", item, "anchors.bottom");
            addDependency("top", item, "anchors.verticalCenter");
            addDependency("top", item, "anchors.bottom");
        } else {
            addDependency("y", item, "anchors.bottom");
            addDependency("y", item, "height");
            addDependency("top", item, "anchors.bottom");
            addDependency("top", item, "height");
            addDependency("verticalCenter", item, "anchors.bottom");
            addDependency("verticalCenter", item, "height");
        }
    } else if (anchors && anchors->verticalCenter().anchorLine != QQuickAnchors::InvalidAnchor) {
        addDependency("verticalCenter", item, "anchors.verticalCenter");
        addDependency("y", item, "anchors.verticalCenter");
        addDependency("y", item, "height");
        addDependency("top", item, "anchors.verticalCenter");
        addDependency("top", item, "height");
        addDependency("bottom", item, "anchors.verticalCenter");
        addDependency("bottom", item, "height");
    } else {
        addDependency("top", item, "y");
        addDependency("bottom", item, "y");
        addDependency("bottom", item, "height");
        addDependency("verticalCenter", item, "y");
        addDependency("verticalCenter", item, "height");
    }

}
