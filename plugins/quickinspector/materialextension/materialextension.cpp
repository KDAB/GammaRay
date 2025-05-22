/*
  materialextension.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <config-gammaray.h>

#include "materialextension.h"
#include "materialshadermodel.h"

#include <core/aggregatedpropertymodel.h>
#include <core/objectinstance.h>
#include <core/propertycontroller.h>
#include <core/varianthandler.h>
#include <core/util.h>
#include <common/metatypedeclarations.h>

#include <QSGNode>
#include <QSGMaterial>

using namespace GammaRay;

MaterialExtension::MaterialExtension(PropertyController *controller)
    : MaterialExtensionInterface(controller->objectBaseName() + ".material", controller)
    , PropertyControllerExtension(controller->objectBaseName() + ".material")
    , m_node(nullptr)
    , m_materialPropertyModel(new AggregatedPropertyModel(this))
    , m_shaderModel(new MaterialShaderModel(this))
{
    controller->registerModel(m_materialPropertyModel, QStringLiteral("materialPropertyModel"));
    controller->registerModel(m_shaderModel, QStringLiteral("shaderModel"));
}

MaterialExtension::~MaterialExtension() = default;

bool MaterialExtension::setObject(void *object, const QString &typeName)
{
    QSGMaterial *material = nullptr;
    m_shaderModel->setMaterialShader(nullptr);
    m_materialShader.reset();

    if (typeName == QStringLiteral("QSGGeometryNode")) {
        m_node = static_cast<QSGGeometryNode *>(object);

        material = m_node->material();
    }

    // the QSG software renderer puts 0x1 into material, so consider that as no material too
    if (Util::isNullish(material)) {
        m_materialPropertyModel->setObject(nullptr);
        return false;
    }

    m_materialPropertyModel->setObject(ObjectInstance(material, "QSGMaterial"));

    m_materialShader.reset(material->createShader(QSGRendererInterface::RenderMode2D));
    m_shaderModel->setMaterialShader(m_materialShader.get());
    return true;
}

void MaterialExtension::getShader(int row)
{
    emit gotShader(m_shaderModel->shaderForRow(row));
}
