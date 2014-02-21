/*
  propertycontrollerextension.h

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

#ifndef PROPERTYCONTROLLEREXTENSION_H
#define PROPERTYCONTROLLEREXTENSION_H

class QObject;
class QMetaObject;

#include "gammaray_core_export.h"
#include <QString>

namespace GammaRay {

class PropertyController;

class GAMMARAY_CORE_EXPORT PropertyControllerExtension
{
public:
  explicit PropertyControllerExtension(const QString &name);

  /** Sets the object that should be represented by this extension. */
  virtual bool setObject(void *object, const QString &typeName) = 0;
  virtual bool setObject(QObject *object) = 0;
  virtual bool setMetaObject(const QMetaObject *metaObject) = 0;

  const QString &name() const;

private:
  QString m_name;
};

class PropertyControllerExtensionFactoryBase {
  public:
    explicit PropertyControllerExtensionFactoryBase() {}
    virtual PropertyControllerExtension *create(PropertyController *controller) = 0;
};

template <typename T>
class PropertyControllerExtensionFactory : public PropertyControllerExtensionFactoryBase
{
  public:
    explicit PropertyControllerExtensionFactory() {}
    PropertyControllerExtension *create(PropertyController *controller)
    {
      return new T(controller);
    }
};

}

#endif // PROPERTYCONTROLLEREXTENSION_H
