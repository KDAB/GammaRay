/*
  metaobject.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_METAOBJECT_H
#define GAMMARAY_METAOBJECT_H

#include "gammaray_core_export.h"

#include "metaproperty.h"
#include "typetraits.h"

#include <QVector>

namespace GammaRay {
/*! Compile-time introspection adaptor for non-QObject classes. */
class GAMMARAY_CORE_EXPORT MetaObject
{
public:
    MetaObject();
    virtual ~MetaObject();

    /*!
     * Returns the amount of properties available in this class (including base classes).
     */
    int propertyCount() const;

    /*!
     * Returns the property adaptor for index @p index.
     */
    MetaProperty *propertyAt(int index) const;

    /*! Add a base class meta object. */
    void addBaseClass(MetaObject *baseClass);

    /*! Add a property for this class. This transfers ownership. */
    void addProperty(MetaProperty *property);

    /*! Returns the name of the class represented by this object. */
    QString className() const;

    /*! Casts a void pointer for an instance of this type to one appropriate
     * for use with the property at index @p index.
     * Make sure to use this when dealing with multi-inheritance.
     */
    void *castForPropertyAt(void *object, int index) const;

    /*! Casts to a void pointer for an instance of this type to one referring
     *  to the given base class type. If @p baseClass is not a base class
     *  of this type, @c nullptr is returned.
     */
    void *castTo(void *object, const QString &baseClass) const;

    /*! Sets the name of this class to @p className. */
    void setClassName(const QString &className);

    /*! Returns the MetaObject for the @p index base class. */
    MetaObject *superClass(int index = 0) const;
    /*! Returns @c true if this class inherits (directly or indirectly) a class named @p className. */
    bool inherits(const QString &className) const;

    /*! Returns @c true if this is a polymorphic type.
     *  @see std::is_polymorphic
     */
    bool isPolymorphic() const;

    /*! Casts to a void pointer for an instance of this type
     *  coming from the given base class. This performs the equivalent
     *  of a dynamic_cast and thus can return @p nullptr.
     */
    void* castFrom(void *object, MetaObject *baseClass) const;

protected:
    /*! Casts up to base class @p baseClassIndex.
     * This is important when traversing multi-inheritance trees.
     */
    virtual void *castToBaseClass(void *object, int baseClassIndex) const = 0;

    /*! Casts down from base class @p baseClassIndex.
     *  This performs a dynamic cast on polymorphic types, and is undefined for non-polymorphic types.
     */
    virtual void *castFromBaseClass(void *object, int baseClassIndex) const = 0;

    /*! Returns if this type is polymorphic. This can but does not require
     *  to return @c true for types derives from a polymorphic type.
     */
    virtual bool isClassPolymorphic() const = 0;

protected:
    ///@cond internal
    QVector<MetaObject *> m_baseClasses;
    ///@endcond

private:
    Q_DISABLE_COPY(MetaObject)
    QVector<MetaProperty *> m_properties;
    QString m_className;
};

///@cond internal
/*! Template implementation of MetaObject. */
template<typename T, typename Base1 = void, typename Base2 = void, typename Base3 = void>
class MetaObjectImpl : public MetaObject
{
protected:
    void *castToBaseClass(void *object, int baseClassIndex) const override
    {
        Q_ASSERT(baseClassIndex >= 0 && baseClassIndex < m_baseClasses.size());
        switch (baseClassIndex) {
        case 0:
            return static_cast<Base1 *>(static_cast<T *>(object));
        case 1:
            return static_cast<Base2 *>(static_cast<T *>(object));
        case 2:
            return static_cast<Base3 *>(static_cast<T *>(object));
        }
        Q_ASSERT_X(false, "MetaObjectImpl::castToBaseClass",
                   "Unexpected baseClassIndex encountered");
        return nullptr;
    }

    void *castFromBaseClass(void *object, int baseClassIndex) const override
    {
        Q_ASSERT(baseClassIndex >= 0 && baseClassIndex < m_baseClasses.size());
        Q_ASSERT(isPolymorphic());
        switch (baseClassIndex) {
            case 0:
                return DynamicCast<T*>(static_cast<Base1*>(object));
            case 1:
                return DynamicCast<T*>(static_cast<Base2*>(object));
            case 2:
                return DynamicCast<T*>(static_cast<Base3*>(object));
        }
        return nullptr;
    }

    bool isClassPolymorphic() const override
    {
        return IsPolymorphic<T>();
    }
};
///@endcond
}

#endif // GAMMARAY_METAOBJECT_H
