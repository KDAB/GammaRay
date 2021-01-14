/*
  materialextension.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <QSGFlatColorMaterial>
#include <QSGTextureMaterial>
#include <QSGVertexColorMaterial>

#include <private/qsgmaterialshader_p.h>
#include <private/qsgdistancefieldglyphnode_p_p.h>

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
#include <private/qquickopenglshadereffectnode_p.h>
#endif

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

    m_materialShader.reset(material->createShader());
    m_shaderModel->setMaterialShader(m_materialShader.get());
    return true;
}

void MaterialExtension::getShader(int row)
{
    emit gotShader(m_shaderModel->shaderForRow(row));
}
