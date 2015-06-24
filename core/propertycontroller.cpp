/*
  propertycontroller.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "propertycontroller.h"

#include "probe.h"

#include <QStringList>

using namespace GammaRay;

QVector<PropertyControllerExtensionFactoryBase*> PropertyController::s_extensionFactories = QVector<PropertyControllerExtensionFactoryBase*>();
QVector<PropertyController*> PropertyController::s_instances = QVector<PropertyController*>();

PropertyController::PropertyController(const QString &baseName, QObject *parent) :
  PropertyControllerInterface(baseName + ".controller", parent),
  m_objectBaseName(baseName)
{
  s_instances << this;
  foreach (PropertyControllerExtensionFactoryBase *factory, s_extensionFactories) {
    m_extensions << factory->create(this);
  }
}

PropertyController::~PropertyController()
{
  const auto i = s_instances.indexOf(this);
  if (i >= 0)
    s_instances.remove(i);
}

void PropertyController::loadExtension(PropertyControllerExtensionFactoryBase* factory)
{
  m_extensions << factory->create(this);
}

const QString &PropertyController::objectBaseName()
{
  return m_objectBaseName;
}

void PropertyController::registerModel(QAbstractItemModel *model, const QString &nameSuffix)
{
  Probe::instance()->registerModel(m_objectBaseName + '.' + nameSuffix, model);
}

void PropertyController::setObject(QObject *object)
{
  if (m_object)
    disconnect(m_object, SIGNAL(destroyed(QObject*)), this, SLOT(objectDestroyed()));
  if (object)
    connect(object, SIGNAL(destroyed(QObject*)), this, SLOT(objectDestroyed()));

  m_object = object;

  QStringList availableExtensions;

  foreach (PropertyControllerExtension *extension, m_extensions) {
    if (extension->setQObject(object))
      availableExtensions << extension->name();
  }

  setAvailableExtensions(availableExtensions);
}

void PropertyController::setObject(void *object, const QString &className)
{
  setObject(0);

  QStringList availableExtensions;

  foreach (PropertyControllerExtension *extension, m_extensions) {
    if (extension->setObject(object, className))
      availableExtensions << extension->name();
  }

  setAvailableExtensions(availableExtensions);
}

void PropertyController::setMetaObject(const QMetaObject *metaObject)
{
  setObject(0);

  QStringList availableExtensions;

  foreach (PropertyControllerExtension *extension, m_extensions) {
    if (extension->setMetaObject(metaObject))
      availableExtensions << extension->name();
  }

  setAvailableExtensions(availableExtensions);
}

void PropertyController::objectDestroyed()
{
  setObject(0);
}
