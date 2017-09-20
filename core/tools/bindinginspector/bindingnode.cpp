/*
  bindingnode.cpp

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

#include "bindingnode.h"
// #include "bindingextension.h"
#include <core/util.h>

#include <QDebug>
#include <QFile>
#include <QMetaProperty>

using namespace GammaRay;

BindingNode::BindingNode(QObject *obj, int propIndex, BindingNode *parent)
    : m_parent(parent)
    , m_object(obj)
    , m_propertyIndex(propIndex)
{
    Q_ASSERT(obj);
    m_canonicalName
        = m_object->metaObject() ? m_object->metaObject()->property(m_propertyIndex).name() : ":(";
    refreshValue();
    checkForLoops();
}

BindingNode::BindingNode(const BindingNode &other)
    : m_parent(other.m_parent)
    , m_object(other.m_object)
    , m_propertyIndex(other.m_propertyIndex)
    , m_canonicalName(other.m_canonicalName)
    , m_value(other.m_value)
    , m_isActive(other.m_isActive)
    , m_isBindingLoop(other.m_isBindingLoop)
    , m_expression(other.m_expression)
    , m_sourceLocation(other.m_sourceLocation)
{
    m_dependencies.reserve(other.m_dependencies.size());
    for (auto &&dependency : other.m_dependencies) {
        m_dependencies.push_back(std::unique_ptr<BindingNode>(new BindingNode(*dependency)));
    }
}

BindingNode::BindingNode(BindingNode &&other)
    : m_parent(other.m_parent)
    , m_object(other.m_object)
    , m_propertyIndex(other.m_propertyIndex)
    , m_canonicalName(other.m_canonicalName)
    , m_value(other.m_value)
    , m_isActive(other.m_isActive)
    , m_isBindingLoop(other.m_isBindingLoop)
    , m_expression(other.m_expression)
    , m_sourceLocation(other.m_sourceLocation)
    , m_dependencies(std::move(other.m_dependencies))
{
//     dependencies.reserve(other.dependencies.size());
//     for (auto &&dependency : other.dependencies) {
//         dependencies.push_back(std::unique_ptr<BindingNode>(new BindingNode(*dependency)));
//     }
}

void BindingNode::checkForLoops()
{
    BindingNode *ancestor = m_parent;
    while (ancestor) {
        if (ancestor->m_object == m_object
            && ancestor->m_propertyIndex == m_propertyIndex) {
            m_isBindingLoop = true;
            return;
        }
        ancestor = ancestor->m_parent;
    }
    m_isBindingLoop = false;
}

QMetaProperty BindingNode::property() const
{
    return m_object->metaObject()->property(m_propertyIndex);
}

QVariant BindingNode::cachedValue() const
{
    return m_value;
}
QVariant BindingNode::readValue() const
{
    return m_object->metaObject()->property(m_propertyIndex).read(m_object);
}
void BindingNode::refreshValue()
{
    m_value = m_object->metaObject()->property(m_propertyIndex).read(m_object);
}

void BindingNode::setParent(BindingNode *newParent)
{
    m_parent = newParent;
    checkForLoops();
}

GammaRay::BindingNode * GammaRay::BindingNode::parent() const
{
    return m_parent;
}
QObject * GammaRay::BindingNode::object() const
{
    return m_object;
}
int GammaRay::BindingNode::propertyIndex() const
{
    return m_propertyIndex;
}
const QString & GammaRay::BindingNode::canonicalName() const
{
    return m_canonicalName;
}
const QString & GammaRay::BindingNode::expression() const
{
    return m_expression;
}
bool GammaRay::BindingNode::isActive() const
{
    return m_isActive;
}
bool GammaRay::BindingNode::isBindingLoop() const
{
    return m_isBindingLoop;
}
GammaRay::SourceLocation GammaRay::BindingNode::sourceLocation() const
{
    return m_sourceLocation;
}
std::vector<std::unique_ptr<BindingNode> > & GammaRay::BindingNode::dependencies()
{
    return m_dependencies;
}
const std::vector<std::unique_ptr<BindingNode> > & GammaRay::BindingNode::dependencies() const
{
    return m_dependencies;
}

void GammaRay::BindingNode::setCanonicalName(const QString& name)
{
    m_canonicalName = name;
}
void GammaRay::BindingNode::setSourceLocation(GammaRay::SourceLocation location)
{
    m_sourceLocation = location;
}


uint BindingNode::depth() const
{
    uint depth = 0;
    if (m_isBindingLoop) {
        return std::numeric_limits<uint>::max(); // to be considered as infinity.
    }
    for (const auto &dependency : m_dependencies) {
        if (!dependency->m_isActive)
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
