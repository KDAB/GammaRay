/*
  sggeometryextension.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
