/*
  materialextension.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <core/aggregatedpropertymodel.h>
#include <core/objectinstance.h>
#include <core/propertycontroller.h>
#include <core/varianthandler.h>
#include <common/metatypedeclarations.h>

#include <QFile>
#include <QStandardItemModel>
#include <QSGNode>
#include <QSGMaterial>
#include <QSGFlatColorMaterial>
#include <QSGTextureMaterial>
#include <QSGVertexColorMaterial>

#include <private/qsgmaterialshader_p.h>

using namespace GammaRay;

class SGMaterialShaderThief : public QSGMaterialShader
{
  public:
    QHash<QOpenGLShader::ShaderType, QStringList> getShaderSources()
    {
      return d_func()->m_sourceFiles;
    }
};

MaterialExtension::MaterialExtension(PropertyController *controller)
  : MaterialExtensionInterface(controller->objectBaseName() + ".material", controller),
    PropertyControllerExtension(controller->objectBaseName() + ".material"),
    m_node(0),
    m_materialPropertyModel(new AggregatedPropertyModel(this)),
    m_shaderModel(new QStandardItemModel(this))
{
  controller->registerModel(m_materialPropertyModel, "materialPropertyModel");
  controller->registerModel(m_shaderModel, "shaderModel");
}

MaterialExtension::~MaterialExtension()
{
}

#include <iostream>

static const char* typeForMaterial(QSGMaterial *material)
{
#define MT(type) if (dynamic_cast<type*>(material)) return #type;
  MT(QSGFlatColorMaterial)
  MT(QSGTextureMaterial)
  MT(QSGOpaqueTextureMaterial)
  MT(QSGVertexColorMaterial)
#undef MT
  return "QSGMaterial";
}

bool MaterialExtension::setObject(void *object, const QString &typeName)
{
  if (typeName == "QSGGeometryNode") {
    m_node = static_cast<QSGGeometryNode*>(object);

    m_materialPropertyModel->setObject(ObjectInstance(m_node->material(), typeForMaterial(m_node->material())));

    QSGMaterialShader *materialShader = m_node->material()->createShader();
    SGMaterialShaderThief *thief = reinterpret_cast<SGMaterialShaderThief*>(materialShader);
    const QHash<QOpenGLShader::ShaderType, QStringList> shaderSources = thief->getShaderSources();

    m_shaderModel->clear();
    m_shaderModel->setHorizontalHeaderLabels(QStringList() << "Shader");
    for (auto it = shaderSources.constBegin(); it != shaderSources.constEnd(); ++it) {
      foreach (const QString &source, it.value()) {
        auto *item = new QStandardItem(source);
        item->setEditable(false);
        item->setToolTip(tr("Shader type: %1").arg(VariantHandler::displayString(it.key())));
        m_shaderModel->appendRow(item);
      }
    }

    return true;
  }

  m_materialPropertyModel->setObject(0);
  return false;
}

void MaterialExtension::getShader(const QString &fileName)
{
  QFile shaderFile(fileName);
  if (!shaderFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return;
  }

  QString source(shaderFile.readAll());

  if (!source.isEmpty()) {
    emit gotShader(source);
  }
}
