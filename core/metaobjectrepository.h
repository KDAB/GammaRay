/*
  metaobjectrepository.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_METAOBJECTREPOSITORY_H
#define GAMMARAY_METAOBJECTREPOSITORY_H

#include "gammaray_core_export.h"
#include <QHash>

class QString;

namespace GammaRay {

class MetaObject;

/**
 * Repository of compile-time introspection information for stuff
 * not covered by the Qt meta object system.
 */
class GAMMARAY_CORE_EXPORT MetaObjectRepository
{
  public:
    ~MetaObjectRepository();

    /** Singleton accessor. */
    static MetaObjectRepository *instance();

    /**
     * Adds object type information to the repository.
     */
    void addMetaObject(MetaObject *mo);

    /**
     * Returns the introspection information for the type with the given name.
     */
    MetaObject *metaObject(const QString &typeName) const;

    /**
     * Returns whether a meta object is known for the given type name.
     */
    bool hasMetaObject(const QString &typeName) const;

  protected:
    MetaObjectRepository();

  private:
    void initBuiltInTypes();
    void initQObjectTypes();
    void initIOTypes();
    void initNetworkTypes();
    void initGuiTypes();
    void initOpenGLTypes();

  private:
    QHash<QString, MetaObject*> m_metaObjects;
    bool m_initialized;
};

}

#define MO_ADD_BASECLASS(Base) \
  Q_ASSERT(MetaObjectRepository::instance()->hasMetaObject(QLatin1String(#Base))); \
  mo->addBaseClass(MetaObjectRepository::instance()->metaObject(QLatin1String(#Base)));

#define MO_ADD_METAOBJECT0(Class) \
  mo = new MetaObjectImpl<Class>; \
  mo->setClassName(QLatin1String(#Class)); \
  MetaObjectRepository::instance()->addMetaObject(mo);

#define MO_ADD_METAOBJECT1(Class, Base1) \
  mo = new MetaObjectImpl<Class, Base1>; \
  mo->setClassName(QLatin1String(#Class)); \
  MO_ADD_BASECLASS(Base1) \
  MetaObjectRepository::instance()->addMetaObject(mo);

#define MO_ADD_METAOBJECT2(Class, Base1, Base2) \
  mo = new MetaObjectImpl<Class, Base1, Base2>; \
  mo->setClassName(QLatin1String(#Class)); \
  MO_ADD_BASECLASS(Base1) \
  MO_ADD_BASECLASS(Base2) \
  MetaObjectRepository::instance()->addMetaObject(mo);

#define MO_ADD_PROPERTY(Class, Type, Getter, Setter) \
  mo->addProperty(new MetaPropertyImpl<Class, Type>( \
    QLatin1String(#Getter), \
    &Class::Getter, \
    static_cast<void (Class::*)(Type)>(&Class::Setter)) \
  );

#define MO_ADD_PROPERTY_CR(Class, Type, Getter, Setter) \
  mo->addProperty(new MetaPropertyImpl<Class, Type, const Type&>( \
    QLatin1String(#Getter), \
    &Class::Getter, \
    static_cast<void (Class::*)(const Type&)>(&Class::Setter)) \
  );

#define MO_ADD_PROPERTY_RO(Class, Type, Getter) \
  mo->addProperty(new MetaPropertyImpl<Class, Type>( \
    QLatin1String(#Getter), \
    &Class::Getter));

#endif // GAMMARAY_METAOBJECTREPOSITORY_H
