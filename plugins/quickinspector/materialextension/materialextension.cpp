/*
  materialextension.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#include "materialextension.h"
#include <core/propertycontroller.h>
#include <QFile>
#include <QSGNode>
#include <QSGMaterial>

#include "config-gammaray.h"

#ifdef HAVE_SG_INSPECTOR
#include <private/qsgmaterialshader_p.h> //krazy:exclude=camelcase
#endif

using namespace GammaRay;

#ifdef HAVE_SG_INSPECTOR
class SGMaterialShaderThief : public QSGMaterialShader
{
public:
  QHash<QOpenGLShader::ShaderType, QStringList> getShaderSources()
  {
    return d_func()->m_sourceFiles;
  }
};
#endif

MaterialExtension::MaterialExtension(PropertyController* controller) :
  PropertyControllerExtension(controller->objectBaseName() + ".material"),
  MaterialExtensionInterface(controller->objectBaseName() + ".material", controller),
  m_node(0)
{
}

MaterialExtension::~MaterialExtension()
{
}

bool MaterialExtension::setObject(QObject* object)
{
  return false;
}

bool MaterialExtension::setObject(void* object, const QString& typeName)
{
#ifdef HAVE_SG_INSPECTOR
  if (typeName == "QSGGeometryNode") {
    m_node = static_cast<QSGGeometryNode*>(object);

    QSGMaterialShader *materialShader = m_node->material()->createShader();
    SGMaterialShaderThief *thief = reinterpret_cast<SGMaterialShaderThief*>(materialShader);
    QHash<QOpenGLShader::ShaderType, QStringList> shaderSources = thief->getShaderSources();

    QStringList sourceFiles;
    foreach (QStringList fileList, shaderSources) {
      sourceFiles << fileList;
    }

    emit shaderListChanged(sourceFiles);
    return true;
  }
#endif
  return false;
}

bool MaterialExtension::setMetaObject(const QMetaObject* metaObject)
{
  return false;
}

void MaterialExtension::getShader(const QString& fileName)
{
  QFile shaderFile(fileName);
  if (!shaderFile.open(QIODevice::ReadOnly | QIODevice::Text))
    return;

  QString source(shaderFile.readAll());

  if (!source.isEmpty())
    emit gotShader(source);
}
