/*
  sggeometryextension.cpp

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

#include "sggeometryextension.h"
#include "sggeometrymodel.h"
#include <core/propertycontroller.h>
#include <core/probe.h>
#include <QMetaProperty>
#include <QSGNode>
#include <QtCore/QDebug>

using namespace GammaRay;

SGGeometryExtension::SGGeometryExtension(PropertyController* controller) :
  PropertyControllerExtension(controller->objectBaseName() + ".sgGeometry"),
  SGGeometryExtensionInterface(controller->objectBaseName() + ".sgGeometry", controller),
  m_node(0),
  m_model(new SGGeometryModel)
{
  controller->registerModel(m_model, "sgGeometryModel");
}

SGGeometryExtension::~SGGeometryExtension()
{
}

bool SGGeometryExtension::setObject(QObject* object)
{
  return false;
}

bool SGGeometryExtension::setObject(void* object, const QString& typeName)
{
  if (typeName == "QSGGeometryNode") {
    m_node = static_cast<QSGGeometryNode*>(object);
    m_model->setGeometry(m_node->geometry());
    return true;
  }
  return false;
}

bool SGGeometryExtension::setMetaObject(const QMetaObject* metaObject)
{
  return false;
}
