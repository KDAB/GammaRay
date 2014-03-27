/*
  metaobject.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_METAOBJECT_H
#define GAMMARAY_METAOBJECT_H

#include "gammaray_core_export.h"

#include "metaproperty.h"

#include <QVector>

namespace GammaRay {

/** @brief Compile-time introspection adaptor for non-QObject classes. */
class GAMMARAY_CORE_EXPORT MetaObject
{
  public:
    MetaObject();
    virtual ~MetaObject();

    /**
     * Returns the amount of properties available in this class (including base classes).
     */
    int propertyCount() const;

    /**
     * Returns the property adaptor for index @p index.
     */
    MetaProperty *propertyAt(int index) const;

    /** Add a base class meta object. */
    void addBaseClass(MetaObject *baseClass);

    /** Add a property for this class. This transfers ownership. */
    void addProperty(MetaProperty *property);

    /// Returns the name of the class represented by this object.
    QString className() const;

    /** Casts a void pointer for an instance of this type to one appropriate
     * for use with the property at index @p index.
     * Make sure to use this when dealing with multi-inheritance.
     */
    void *castForPropertyAt(void *object, int index) const;

    void setClassName(const QString &className);

    MetaObject *superClass(int index = 0) const;
    bool inherits(const QString &className) const;

  protected:
    /** Casts down to base class @p baseClassIndex.
     * This is important when traversing multi-inheritance trees.
     */
    virtual void *castToBaseClass(void *object, int baseClassIndex) const = 0;

  protected:
    QVector<MetaObject*> m_baseClasses;

  private:
    QVector<MetaProperty*> m_properties;
    QString m_className;
};

/** @brief Template implementation of MetaObject. */
template <typename T, typename Base1 = void, typename Base2 = void, typename Base3 = void>
class MetaObjectImpl : public MetaObject
{
  public:
    void *castToBaseClass(void *object, int baseClassIndex) const
    {
      Q_ASSERT(baseClassIndex >= 0 && baseClassIndex < m_baseClasses.size());
      switch (baseClassIndex) {
      case 0:
        return static_cast<Base1*>(static_cast<T*>(object));
      case 1:
        return static_cast<Base2*>(static_cast<T*>(object));
      case 2:
        return static_cast<Base3*>(static_cast<T*>(object));
      }
      Q_ASSERT(!"WTF!?");
      return 0;
    }
};

}

#endif // GAMMARAY_METAOBJECT_H
