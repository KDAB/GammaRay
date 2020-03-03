/*
  metaproperty.h

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

#ifndef GAMMARAY_METAPROPERTY_H
#define GAMMARAY_METAPROPERTY_H

#include "gammaray_core_export.h"

#include <QVariant>

#include <functional>

namespace GammaRay {
class MetaObject;

/*! Introspectable adaptor to non-QObject properties. */
class GAMMARAY_CORE_EXPORT MetaProperty
{
public:
    /*! Create a new MetaProperty instance for a property named @p name. */
    explicit MetaProperty(const char *name);
    virtual ~MetaProperty();

    /// User-readable name of that property
    const char *name() const;

    /// Current value of the property for object @p object.
    virtual QVariant value(void *object) const = 0;

    /// Returns @c true if this property is read-only.
    virtual bool isReadOnly() const = 0;

    /// Allows changing the property value, assuming it's not read-only, for the instance @p object.
    virtual void setValue(void *object, const QVariant &value);

    /// Returns the name of the data type of this property.
    virtual const char *typeName() const = 0;

    /// Returns the class this property belongs to.
    MetaObject *metaObject() const;

private:
    Q_DISABLE_COPY(MetaProperty)
    friend class MetaObject;
    void setMetaObject(MetaObject *om);

    MetaObject *m_class;
    const char *m_name;
};

///@cond internal
namespace detail {
template<typename T>
struct strip_const_ref {
    typedef T type;
};

template<typename T>
struct strip_const_ref<const T &> {
    typedef T type;
};

template<typename T>
struct add_const_ref {
    typedef const typename strip_const_ref<T>::type & type;
};
}

/*! Template-ed implementation of MetaProperty for member properties. */
template<typename Class, typename GetterReturnType, typename SetterArgType = GetterReturnType, typename GetterSignature = GetterReturnType (Class::*)() const>
class MetaPropertyImpl : public MetaProperty
{
private:
    typedef typename detail::strip_const_ref<GetterReturnType>::type ValueType;

public:
    inline MetaPropertyImpl(
        const char *name, GetterSignature getter,
        void(Class::*setter)(SetterArgType) = nullptr)
        : MetaProperty(name)
        , m_getter(getter)
        , m_setter(setter)
    {
    }

    bool isReadOnly() const override
    {
        return m_setter == nullptr;
    }

    QVariant value(void *object) const override
    {
        Q_ASSERT(object);
        Q_ASSERT(m_getter);
        const ValueType v = (static_cast<Class *>(object)->*(m_getter))();
        return QVariant::fromValue(v);
    }

    void setValue(void *object, const QVariant &value) override
    {
        if (isReadOnly())
            return;
        Q_ASSERT(object);
        Q_ASSERT(m_setter);
        (static_cast<Class *>(object)->*(m_setter))(value.value<ValueType>());
    }

    const char *typeName() const override
    {
        return QMetaType::typeName(qMetaTypeId<ValueType>());
    }

private:
    GetterSignature m_getter;
    void (Class::*m_setter)(SetterArgType);
};

/*! Template-ed implementation of MetaProperty for static properties. */
template<typename GetterReturnType>
class MetaStaticPropertyImpl : public MetaProperty
{
private:
    typedef typename detail::strip_const_ref<GetterReturnType>::type ValueType;

public:
    inline MetaStaticPropertyImpl(const char *name, GetterReturnType(*getter)())
        : MetaProperty(name)
        , m_getter(getter)
    {
    }

    bool isReadOnly() const override
    {
        return true;
    }

    QVariant value(void *object) const override
    {
        Q_UNUSED(object);
        Q_ASSERT(m_getter);
        const ValueType v = m_getter();
        return QVariant::fromValue(v);
    }

    const char *typeName() const override
    {
        return QMetaType::typeName(qMetaTypeId<ValueType>());
    }

private:
    GetterReturnType (*m_getter)();
};

/*! Template-ed implementation of MetaProperty for member variable "properties". */
template<typename Class, typename ValueType>
class MetaMemberPropertyImpl : public MetaProperty
{
public:
    inline MetaMemberPropertyImpl(const char *name, ValueType Class::*member)
        : MetaProperty(name)
        , m_member(member)
    {
    }

    bool isReadOnly() const override
    {
        return true; // we could technically implement write access too, if needed...
    }

    QVariant value(void *object) const override
    {
        Q_ASSERT(object);
        Q_ASSERT(m_member);
        return QVariant::fromValue(reinterpret_cast<Class*>(object)->*m_member);
    }

    const char *typeName() const override
    {
        return QMetaType::typeName(qMetaTypeId<ValueType>());
    }

private:
    ValueType Class::*m_member;
};

/*! Lambda "property" implementation of MetaProperty. */
template<typename Class, typename ValueType>
class MetaLambdaPropertyImpl : public MetaProperty
{
public:
    inline explicit MetaLambdaPropertyImpl(const char *name, const std::function<ValueType(Class*)> &func)
        : MetaProperty(name)
        , m_func(func)
    {
    }

    bool isReadOnly() const override
    {
        return true; // we could extend this to setters too, eventually
    }

    QVariant value(void *object) const override
    {
        return QVariant::fromValue(m_func(reinterpret_cast<Class*>(object)));
    }

    const char *typeName() const override
    {
        return QMetaType::typeName(qMetaTypeId<ValueType>());
    }

private:
    const std::function<ValueType(Class*)> m_func;
};

/*! Template argument deduction factory methods for the MetaXPropertyImpl classes. */
namespace MetaPropertyFactory
{
    // explicitly handle value and const ref setters, to deal with overloaded setters for arbitrary types
    template <typename Class, typename GetterReturnType>
    inline MetaProperty* makeProperty(const char *name, GetterReturnType(Class::*getter)() const, void(Class::*setter)(typename detail::strip_const_ref<GetterReturnType>::type))
    {
        return new MetaPropertyImpl<Class, GetterReturnType, typename detail::strip_const_ref<GetterReturnType>::type>(name, getter, setter);
    }

    template <typename Class, typename GetterReturnType>
    inline MetaProperty* makeProperty(const char *name, GetterReturnType(Class::*getter)() const, void(Class::*setter)(typename detail::add_const_ref<GetterReturnType>::type))
    {
        return new MetaPropertyImpl<Class, GetterReturnType, typename detail::add_const_ref<GetterReturnType>::type>(name, getter, setter);
    }

    // can't merge with the above function, since MSVC2010 can't do default template arguments for template functions...
    template <typename Class, typename GetterReturnType>
    inline MetaProperty* makeProperty(const char *name, GetterReturnType(Class::*getter)() const)
    {
        return new MetaPropertyImpl<Class, GetterReturnType>(name, getter, nullptr);
    }

    // non-const getters...
    template <typename Class, typename GetterReturnType>
    inline MetaProperty* makePropertyNonConst(const char *name, GetterReturnType(Class::*getter)())
    {
        return new MetaPropertyImpl<Class, GetterReturnType, GetterReturnType, GetterReturnType (Class::*)()>(name, getter, nullptr);
    }

    template <typename GetterReturnType>
    inline MetaProperty* makeProperty(const char *name, GetterReturnType(*getter)())
    {
        return new MetaStaticPropertyImpl<GetterReturnType>(name, getter);
    }

    template <typename Class, typename ValueType>
    inline MetaProperty* makeProperty(const char *name, ValueType Class::*member)
    {
        return new MetaMemberPropertyImpl<Class, ValueType>(name, member);
    }

    // lamda getters
    template <typename Class, typename GetterReturnType>
    inline MetaProperty* makeProperty(const char *name, const std::function<GetterReturnType(Class*)> &func)
    {
        return new MetaLambdaPropertyImpl<Class, GetterReturnType>(name, func);
    }
}
///@endcond

}
#endif
