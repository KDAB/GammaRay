/*
  metaobjectrepository.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

/** @file metaobjectrepository.h
 *  @brief MetaObject repository macros.
 */

#ifndef GAMMARAY_METAOBJECTREPOSITORY_H
#define GAMMARAY_METAOBJECTREPOSITORY_H

#include "gammaray_core_export.h"
#include <QHash>

class QString;

namespace GammaRay {

class MetaObject;

/**
 * @brief MetaObject repository.
 *
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
    MetaObject *metaObject(const QString& typeName) const;

    /**
     * Returns whether a meta object is known for the given type name.
     */
    bool hasMetaObject(const QString &typeName) const;

  protected:
    MetaObjectRepository();

  private:
    Q_DISABLE_COPY(MetaObjectRepository)
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
///@cond internal
#define MO_ADD_BASECLASS(Base) \
  Q_ASSERT(GammaRay::MetaObjectRepository::instance()->hasMetaObject(QStringLiteral(#Base))); \
  mo->addBaseClass(GammaRay::MetaObjectRepository::instance()->metaObject(QStringLiteral(#Base)));
///@endcond

/** Register @p Class with the MetaObjectRepository.
 *  Use this if @p Class has no base class.
 */
#define MO_ADD_METAOBJECT0(Class) \
  mo = new GammaRay::MetaObjectImpl<Class>; \
  mo->setClassName(QStringLiteral(#Class)); \
  GammaRay::MetaObjectRepository::instance()->addMetaObject(mo);

/** Register @p Class with the MetaObjectRepository.
 *  Use this if @p Class has one base class.
 */
#define MO_ADD_METAOBJECT1(Class, Base1) \
  mo = new GammaRay::MetaObjectImpl<Class, Base1>; \
  mo->setClassName(QStringLiteral(#Class)); \
  MO_ADD_BASECLASS(Base1) \
  GammaRay::MetaObjectRepository::instance()->addMetaObject(mo);

/** Register @p Class with the MetaObjectRepository.
 *  Use this if @p Class has two base classes.
 */
#define MO_ADD_METAOBJECT2(Class, Base1, Base2) \
  mo = new GammaRay::MetaObjectImpl<Class, Base1, Base2>; \
  mo->setClassName(QStringLiteral(#Class)); \
  MO_ADD_BASECLASS(Base1) \
  MO_ADD_BASECLASS(Base2) \
  GammaRay::MetaObjectRepository::instance()->addMetaObject(mo);

/** Register a read/write property for class @p Class. */
#define MO_ADD_PROPERTY(Class, Type, Getter, Setter) \
  mo->addProperty(new GammaRay::MetaPropertyImpl<Class, Type>( \
    #Getter, \
    &Class::Getter, \
    static_cast<void (Class::*)(Type)>(&Class::Setter)) \
  );

/** Register a read/write property for class @p Class with a type that is passed as const reference. */
#define MO_ADD_PROPERTY_CR(Class, Type, Getter, Setter) \
  mo->addProperty(new GammaRay::MetaPropertyImpl<Class, Type, const Type&>( \
    #Getter, \
    &Class::Getter, \
    static_cast<void (Class::*)(const Type&)>(&Class::Setter)) \
  );

/** Register a read-only property for class @p Class. */
#define MO_ADD_PROPERTY_RO(Class, Type, Getter) \
  mo->addProperty(new GammaRay::MetaPropertyImpl<Class, Type>( \
    #Getter, \
    &Class::Getter));

/** Register a static property for class @p Class. */
#define MO_ADD_PROPERTY_ST(Class, Type, Getter) \
  mo->addProperty(new GammaRay::MetaStaticPropertyImpl<Class, Type>( \
    #Getter, \
    &Class::Getter));

#endif // GAMMARAY_METAOBJECTREPOSITORY_H
