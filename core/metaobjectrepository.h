/*
  metaobjectrepository.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

/** @file metaobjectrepository.h
 *  @brief MetaObject repository macros.
 */

#ifndef GAMMARAY_METAOBJECTREPOSITORY_H
#define GAMMARAY_METAOBJECTREPOSITORY_H

#include "gammaray_core_export.h"
#include <QHash>

#include <unordered_map>
#include <vector>

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

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
    MetaObject *metaObject(const QString &typeName) const;

    /**
     * Returns the introspection information for the given object instance.
     * This behaves as the above function for non-polymorphic types, for polymorphic
     * types it tries to find the most specific derived type for the given instance.
     * @param typeName The name of the (base) type of @p obj.
     * @param obj The object pointer is modified if necessary for the corresponding type.
     * This is necessary to support multiple inheritance.
     */
    MetaObject *metaObject(const QString &typeName, void *&obj) const;
    /** Same as the above method, just using an already looked-up MetaObject. */
    MetaObject *metaObject(MetaObject *mo, void *&obj) const;

    /**
     * Returns whether a meta object is known for the given type name.
     */
    bool hasMetaObject(const QString &typeName) const;

    /*!
     * Clears the content of the meta object repository.
     * Call this when removing the probe from a target.
     * \internal
     */
    void clear();

protected:
    MetaObjectRepository() = default;

private:
    Q_DISABLE_COPY(MetaObjectRepository)
    void initBuiltInTypes();
    static void initQObjectTypes();
    static void initIOTypes();
    static void initQEventTypes();

private:
    QHash<QString, MetaObject *> m_metaObjects;
    std::unordered_map<MetaObject *, std::vector<MetaObject *>> m_derivedTypes;
    bool m_initialized = false;
};
}
///@cond internal
#define MO_ADD_BASECLASS(Base)                                                                  \
    Q_ASSERT(GammaRay::MetaObjectRepository::instance()->hasMetaObject(QStringLiteral(#Base))); \
    mo->addBaseClass(GammaRay::MetaObjectRepository::instance()->metaObject(QStringLiteral(#Base)));
///@endcond

/** Register @p Class with the MetaObjectRepository.
 *  Use this if @p Class has no base class.
 */
#define MO_ADD_METAOBJECT0(Class)             \
    mo = new GammaRay::MetaObjectImpl<Class>; \
    mo->setClassName(QStringLiteral(#Class)); \
    GammaRay::MetaObjectRepository::instance()->addMetaObject(mo);

/** Register @p Class with the MetaObjectRepository.
 *  Use this if @p Class has one base class.
 */
#define MO_ADD_METAOBJECT1(Class, Base1)             \
    mo = new GammaRay::MetaObjectImpl<Class, Base1>; \
    mo->setClassName(QStringLiteral(#Class));        \
    MO_ADD_BASECLASS(Base1)                          \
    GammaRay::MetaObjectRepository::instance()->addMetaObject(mo);

/** Register @p Class with the MetaObjectRepository.
 *  Use this if @p Class has two base classes.
 */
#define MO_ADD_METAOBJECT2(Class, Base1, Base2)             \
    mo = new GammaRay::MetaObjectImpl<Class, Base1, Base2>; \
    mo->setClassName(QStringLiteral(#Class));               \
    MO_ADD_BASECLASS(Base1)                                 \
    MO_ADD_BASECLASS(Base2)                                 \
    GammaRay::MetaObjectRepository::instance()->addMetaObject(mo);

/** Register a read/write property for class @p Class. */
#define MO_ADD_PROPERTY(Class, Getter, Setter) \
    mo->addProperty(GammaRay::MetaPropertyFactory::makeProperty(#Getter, &Class::Getter, &Class::Setter));

/** Register a read-only property for class @p Class. */
#define MO_ADD_PROPERTY_RO(Class, Getter) \
    mo->addProperty(GammaRay::MetaPropertyFactory::makeProperty(#Getter, &Class::Getter));

/** Register a non-const read-only property for class @p Class. */
#define MO_ADD_PROPERTY_NC(Class, Getter) \
    mo->addProperty(GammaRay::MetaPropertyFactory::makePropertyNonConst(#Getter, &Class::Getter));

#if !defined(Q_CC_MSVC) || _MSC_VER >= 1900 // krazy:exclude=cpp
/** Register a lambda property getter for class @p Class. */
#define MO_ADD_PROPERTY_LD(Class, Name, Func)                                                                                  \
    {                                                                                                                          \
        const auto ld = Func;                                                                                                  \
        mo->addProperty(GammaRay::MetaPropertyFactory::makeProperty<Class, decltype(ld(std::declval<Class *>()))>(#Name, ld)); \
    }

/** Register a read/write property for class @p Class.
 *  Use this for overloaded getters or setters that would confuse older MSVC versions.
 */
#define MO_ADD_PROPERTY_O2(Class, Getter, Setter) \
    mo->addProperty(GammaRay::MetaPropertyFactory::makeProperty(#Getter, &Class::Getter, &Class::Setter));

/** Register a read-only property for class @p Class.
 *  Use this for overloaded getters or setters that would confuse older MSVC versions.
 */
#define MO_ADD_PROPERTY_O1(Class, Getter) \
    mo->addProperty(GammaRay::MetaPropertyFactory::makeProperty(#Getter, &Class::Getter));

#else
#define MO_ADD_PROPERTY_LD(Class, Name, Func)
#define MO_ADD_PROPERTY_O2(Class, Getter, Setter)
#define MO_ADD_PROPERTY_O1(Class, Getter)
#endif

/** Register a static property for class @p Class. */
#define MO_ADD_PROPERTY_ST(Class, Getter) \
    mo->addProperty(GammaRay::MetaPropertyFactory::makeProperty(#Getter, &Class::Getter));

/** Register a member property for class @p Class. */
#define MO_ADD_PROPERTY_MEM(Class, Member) \
    mo->addProperty(GammaRay::MetaPropertyFactory::makeProperty(#Member, &Class::Member));

#endif // GAMMARAY_METAOBJECTREPOSITORY_H
