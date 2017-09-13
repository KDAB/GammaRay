/*
  quickimplicitbindingdependencyprovider.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.
//
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

#include "quickimplicitbindingdependencyprovider.h"
#include <core/util.h>
#include <core/tools/bindinginspector/bindingnode.h>

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
    auto node = std::unique_ptr<BindingNode>(new BindingNode(obj, propertyIndex));
    QQmlContext *ctx = QQmlEngine::contextForObject(obj);
    if (ctx) {
        QString id = ctx->nameForObject(obj);
        if (!id.isEmpty())
            node->setCanonicalName(QStringLiteral("%1.%2").arg(id, node->canonicalName()));
    }
    return node;
}

std::vector<std::unique_ptr<BindingNode>> QuickImplicitBindingDependencyProvider::findDependenciesFor(BindingNode *binding)
{
    std::vector<std::unique_ptr<BindingNode>> dependencies;
    // So far, we can only hard code implicit dependencies.
    QObject *object = binding->object();
    if (!object)
        return dependencies;

    auto dependency = [this, binding, object, &dependencies](const char *propName, QObject *depObj, const char *depName)
    {
        if (depObj && binding->propertyIndex() == object->metaObject()->indexOfProperty(propName)) {
            dependencies.push_back(createBindingNode(depObj, depName));
        }
    };

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
            dependencies.push_back(createBindingNode(anchorLine.item, dependencyPropertyName));
        }
    }

    if (QQuickItem *item = qobject_cast<QQuickItem*>(object)) {
        QQuickItemPrivate *itemPriv = QQuickItemPrivate::get(item);
        if (!itemPriv)
            return dependencies;
        QQuickAnchors *anchors = itemPriv->anchors();
        if (!anchors)
            return dependencies;

        if (!itemPriv->widthValid) {
            dependency("width", object, "implicitWidth");
        }
        if (!itemPriv->heightValid) {
            dependency("height", object, "implicitHeight");
        }
        if (binding->propertyIndex()
                   == item->metaObject()->indexOfProperty("childrenRect")) {
            for (auto &&child : item->childItems()) {
                dependencies.push_back(createBindingNode(child, "width"));
                dependencies.push_back(createBindingNode(child, "height"));
            }
        }
        if (item->inherits("QQuickBasePositioner")) {
            if (binding->propertyIndex()
                   == item->metaObject()->indexOfProperty("implicitWidth")) {
                for (QQuickItem *child : item->childItems()) {
                    dependencies.push_back(createBindingNode(child, "width"));
                }
            } else if (binding->propertyIndex()
                   == item->metaObject()->indexOfProperty("implicitHeight")) {
                for (QQuickItem *child : item->childItems()) {
                    dependencies.push_back(createBindingNode(child, "height"));
                }
            }
        }


        // Horizontal
        if (anchors->fill()) {
            QQuickItem *fill = anchors->fill();
            dependency("width", fill, "width");
            dependency("width", anchors, "leftMargin");
            dependency("width", anchors, "rightMargin");
            dependency("x", fill, "left");
            dependency("x", anchors, "leftMargin");
            dependency("left", fill, "left");
            dependency("left", anchors, "leftMargin");
            dependency("right", fill, "right");
            dependency("right", anchors, "rightMargin");
            dependency("horizontalCenter", item, "left");
            dependency("horizontalCenter", item, "right");
        } else if (anchors->centerIn()) {
            QQuickItem *centerIn = anchors->centerIn();
            dependency("horizontalCenter", centerIn, "horizontalCenter");
            dependency("x", centerIn, "horizontalCenter");
            dependency("x", item, "width");
            dependency("left", centerIn, "horizontalCenter");
            dependency("left", item, "width");
            dependency("right", centerIn, "horizontalCenter");
            dependency("right", item, "width");
        } else if (anchors->left().anchorLine != QQuickAnchors::InvalidAnchor) {
            dependency("left", anchors, "left");
            dependency("x", anchors, "left");
            if (anchors->right().anchorLine != QQuickAnchors::InvalidAnchor) {
                dependency("right", anchors, "right");
                dependency("right", anchors, "rightMargin");
                dependency("width", anchors, "left");
                dependency("width", anchors, "right");
                dependency("horizontalCenter", anchors, "left");
                dependency("horizontalCenter", anchors, "right");
            } else if (anchors->horizontalCenter().anchorLine != QQuickAnchors::InvalidAnchor) {
                dependency("horizontalCenter", anchors, "horizontalCenter");
                dependency("width", anchors, "left");
                dependency("width", anchors, "horizontalCenter");
                dependency("right", anchors, "left");
                dependency("right", anchors, "horizontalCenter");
            } else {
                dependency("right", anchors, "left");
                dependency("right", item, "width");
                dependency("horizontalCenter", anchors, "left");
                dependency("horizontalCenter", item, "width");
            }
        } else if (anchors->right().anchorLine != QQuickAnchors::InvalidAnchor) {
            dependency("right", anchors, "right");
            if (anchors->horizontalCenter().anchorLine != QQuickAnchors::InvalidAnchor) {
                dependency("horizontalCenter", anchors, "horizontalCenter");
                dependency("width", anchors, "right");
                dependency("width", anchors, "horizontalCenter");
                dependency("x", anchors, "horizontalCenter");
                dependency("x", anchors, "right");
                dependency("left", anchors, "horizontalCenter");
                dependency("left", anchors, "right");
            } else {
                dependency("x", anchors, "right");
                dependency("x", item, "width");
                dependency("left", anchors, "right");
                dependency("left", item, "width");
                dependency("horizontalCenter", anchors, "right");
                dependency("horizontalCenter", item, "width");
            }
        } else if (anchors->horizontalCenter().anchorLine != QQuickAnchors::InvalidAnchor) {
            dependency("horizontalCenter", anchors, "horizontalCenter");
            dependency("x", anchors, "horizontalCenter");
            dependency("x", item, "width");
            dependency("left", anchors, "horizontalCenter");
            dependency("left", item, "width");
            dependency("right", anchors, "horizontalCenter");
            dependency("right", item, "width");
        } else {
            dependency("left", item, "x");
            dependency("right", item, "x");
            dependency("right", item, "width");
            dependency("horizontalCenter", item, "x");
            dependency("horizontalCenter", item, "width");
        }


        // Vertical TODO: Bottomline
        if (anchors->fill()) {
            QQuickItem *fill = anchors->fill();
            dependency("height", fill, "height");
            dependency("height", anchors, "topMargin");
            dependency("height", anchors, "bottomMargin");
            dependency("y", fill, "top");
            dependency("y", anchors, "topMargin");
            dependency("top", fill, "top");
            dependency("top", anchors, "topMargin");
            dependency("bottom", fill, "bottom");
            dependency("bottom", anchors, "bottomMargin");
            dependency("verticalCenter", item, "top");
            dependency("verticalCenter", item, "bottom");
        } else if (anchors->centerIn()) {
            QQuickItem *centerIn = anchors->centerIn();
            dependency("verticalCenter", centerIn, "verticalCenter");
            dependency("y", centerIn, "verticalCenter");
            dependency("y", item, "height");
            dependency("top", centerIn, "verticalCenter");
            dependency("top", item, "height");
            dependency("bottom", centerIn, "verticalCenter");
            dependency("bottom", item, "height");
        } else if (anchors->top().anchorLine != QQuickAnchors::InvalidAnchor) {
            dependency("top", anchors, "top");
            dependency("y", anchors, "top");
            if (anchors->bottom().anchorLine != QQuickAnchors::InvalidAnchor) {
                dependency("bottom", anchors, "bottom");
                dependency("bottom", anchors, "bottomMargin");
                dependency("height", anchors, "top");
                dependency("height", anchors, "bottom");
                dependency("verticalCenter", anchors, "top");
                dependency("verticalCenter", anchors, "bottom");
            } else if (anchors->verticalCenter().anchorLine != QQuickAnchors::InvalidAnchor) {
                dependency("verticalCenter", anchors, "verticalCenter");
                dependency("height", anchors, "top");
                dependency("height", anchors, "verticalCenter");
                dependency("y", anchors, "top");
                dependency("bottom", anchors, "top");
                dependency("bottom", anchors, "verticalCenter");
            } else {
                dependency("bottom", anchors, "top");
                dependency("bottom", item, "height");
                dependency("verticalCenter", anchors, "top");
                dependency("verticalCenter", item, "height");
            }
        } else if (anchors->bottom().anchorLine != QQuickAnchors::InvalidAnchor) {
            dependency("bottom", anchors, "bottom");
            if (anchors->verticalCenter().anchorLine != QQuickAnchors::InvalidAnchor) {
                dependency("verticalCenter", anchors, "verticalCenter");
                dependency("height", anchors, "bottom");
                dependency("height", anchors, "verticalCenter");
                dependency("y", anchors, "verticalCenter");
                dependency("y", anchors, "bottom");
                dependency("top", anchors, "verticalCenter");
                dependency("top", anchors, "bottom");
            } else {
                dependency("y", anchors, "bottom");
                dependency("y", item, "height");
                dependency("top", anchors, "bottom");
                dependency("top", item, "height");
                dependency("verticalCenter", anchors, "bottom");
                dependency("verticalCenter", item, "height");
            }
        } else if (anchors->verticalCenter().anchorLine != QQuickAnchors::InvalidAnchor) {
            dependency("verticalCenter", anchors, "verticalCenter");
            dependency("y", anchors, "verticalCenter");
            dependency("y", item, "height");
            dependency("top", anchors, "verticalCenter");
            dependency("top", item, "height");
            dependency("bottom", anchors, "verticalCenter");
            dependency("bottom", item, "height");
        } else {
            dependency("top", item, "y");
            dependency("bottom", item, "y");
            dependency("bottom", item, "height");
            dependency("verticalCenter", item, "y");
            dependency("verticalCenter", item, "height");
        }
    }

    return dependencies;
}

QString QuickImplicitBindingDependencyProvider::canonicalNameFor(BindingNode* binding)
{
    return QString();
}
