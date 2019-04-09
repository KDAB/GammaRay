/*
  sggeometryextension.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "sggeometryextension.h"
#include "sggeometrymodel.h"
#include <core/propertycontroller.h>
#include <core/probe.h>
#include <core/util.h>
#include <QMetaProperty>
#include <QSGNode>

using namespace GammaRay;

SGGeometryExtension::SGGeometryExtension(PropertyController *controller)
    : PropertyControllerExtension(controller->objectBaseName() + ".sgGeometry")
    , m_node(nullptr)
    , m_vertexModel(new SGVertexModel(controller))
    , m_adjacencyModel(new SGAdjacencyModel(controller))
{
    controller->registerModel(m_vertexModel, QStringLiteral("sgGeometryVertexModel"));
    controller->registerModel(m_adjacencyModel, QStringLiteral("sgGeometryAdjacencyModel"));
}

SGGeometryExtension::~SGGeometryExtension() = default;

bool SGGeometryExtension::setObject(void *object, const QString &typeName)
{
    if (typeName == QStringLiteral("QSGGeometryNode")) {
        m_node = static_cast<QSGGeometryNode *>(object);
        auto geometry = m_node->geometry();
        // the QSG software renderer puts 0x1 into geometry, so consider that as no geometry too
        if (Util::isNullish(geometry))
            return false;

        m_vertexModel->setNode(m_node);
        m_adjacencyModel->setNode(m_node);
        return true;
    }
    return false;
}
