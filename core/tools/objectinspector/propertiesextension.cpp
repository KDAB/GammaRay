/*
  propertiesextension.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "propertiesextension.h"

#include "aggregatedpropertymodel.h"
#include "metapropertymodel.h"
#include "objectdynamicpropertymodel.h"
#include "objectstaticpropertymodel.h"
#include "propertycontroller.h"
#include <probe.h>
#include <common/propertymodel.h>
#include <QMetaProperty>

using namespace GammaRay;

PropertiesExtension::PropertiesExtension(PropertyController *controller) :
  PropertiesExtensionInterface(controller->objectBaseName() + ".propertiesExtension", controller),
  PropertyControllerExtension(controller->objectBaseName() + ".properties"),
  m_staticPropertyModel(new ObjectStaticPropertyModel(this)),
  m_dynamicPropertyModel(new ObjectDynamicPropertyModel(this)),
  m_metaPropertyModel(new MetaPropertyModel(this)),
  m_aggregatedPropertyModel(new AggregatedPropertyModel(this))
{
  controller->registerModel(m_aggregatedPropertyModel, "properties");

  m_aggregatedPropertyModel->addModel(m_staticPropertyModel);
  m_aggregatedPropertyModel->addModel(m_metaPropertyModel);
  m_aggregatedPropertyModel->addModel(m_dynamicPropertyModel);
}

PropertiesExtension::~PropertiesExtension()
{
}

bool PropertiesExtension::setQObject(QObject *object)
{
  if (m_object == object)
    return true;
  m_object = object;
  m_staticPropertyModel->setObject(object);
  m_dynamicPropertyModel->setObject(object);
  m_metaPropertyModel->setObject(object);
  setCanAddProperty(true);
  return true;
}

bool PropertiesExtension::setObject(void *object, const QString &typeName)
{
  m_object = 0;
  m_staticPropertyModel->setObject(0);
  m_dynamicPropertyModel->setObject(0);
  m_metaPropertyModel->setObject(object, typeName);
  setCanAddProperty(false);
  return true;
}

bool PropertiesExtension::setMetaObject(const QMetaObject* metaObject)
{
  m_object = 0;
  m_staticPropertyModel->setMetaObject(metaObject);
  m_dynamicPropertyModel->setObject(0);
  m_metaPropertyModel->setObject(0);
  setCanAddProperty(false);
  return true;
}

void PropertiesExtension::navigateToValue(int modelRow)
{
  QModelIndex index = m_aggregatedPropertyModel->index(modelRow, 2);
  QVariant propertyValue = index.data(PropertyModel::ValueRole);
  if (propertyValue.canConvert<QObject*>()) {
    Probe::instance()->selectObject(propertyValue.value<QObject*>());
  } else {
    Probe::instance()->selectObject(*reinterpret_cast<void**>(propertyValue.data()),
                                    index.data(Qt::DisplayRole).toString());
  }
}

void PropertiesExtension::setProperty(const QString &name, const QVariant &value)
{
  if (!m_object) {
    return;
  }
  m_object->setProperty(name.toUtf8(), value);
}

void PropertiesExtension::resetProperty(const QString &name)
{
  if (!m_object || name.isEmpty()) {
    return;
  }

  const int index = m_object->metaObject()->indexOfProperty(name.toUtf8());
  const QMetaProperty prop = m_object->metaObject()->property(index);
  prop.reset(m_object);
}
