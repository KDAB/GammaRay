/*
  propertycontrollerextension.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#ifndef PROPERTYCONTROLLEREXTENSION_H
#define PROPERTYCONTROLLEREXTENSION_H

#include "gammaray_core_export.h"

#include <QString>

QT_BEGIN_NAMESPACE
class QObject;
struct QMetaObject;
QT_END_NAMESPACE

namespace GammaRay {
class PropertyController;

/**
 * @brief Base-class for server-side property editor extensions.
 *
 * This can be used to add your own tabs to the property widget.
 * Re-implement the corresponding variant of setObject/setMetaObject
 * you can handle, the default implementations do nothing and return
 * @c false.
 *
 * @since 2.1
 */
class GAMMARAY_CORE_EXPORT PropertyControllerExtension
{
public:
    /** @brief Create a new property extension.
     *  @param name The extension identifier used for client/server communication.
     */
    explicit PropertyControllerExtension(const QString &name);
    virtual ~PropertyControllerExtension();

    /** @brief Sets the object that should be represented by this extension.
     *  This variant is used for non-QObject types using Gammaray::MetaObjectRepository.
     *  @return @c true if the extension can handle @p object, @c false otherwise.
     */
    virtual bool setObject(void *object, const QString &typeName);

    /** @brief Sets the QObject that should be represented by this extension.
     *  This variant is used for QObject-derived types.
     *  @return @c true if the extension can handle @p object, @c false otherwise.
     */
    virtual bool setQObject(QObject *object);

    /** @brief Sets the meta object that should be represented by this extension.
     *  This variant is used for QMetaObjects without a specific object instance.
     *  @return @c true if the extension can handle @p object, @c false otherwise.
     */
    virtual bool setMetaObject(const QMetaObject *metaObject);

    /** @brief Returns the identifier of this extension, used for client/server communication. */
    QString name() const;

private:
    Q_DISABLE_COPY(PropertyControllerExtension)
    QString m_name;
};

///@cond internal
class PropertyControllerExtensionFactoryBase
{
public:
    explicit PropertyControllerExtensionFactoryBase() {}
    virtual PropertyControllerExtension *create(PropertyController *controller) = 0;
private:
    Q_DISABLE_COPY(PropertyControllerExtensionFactoryBase)
};

template<typename T>
class PropertyControllerExtensionFactory : public PropertyControllerExtensionFactoryBase
{
public:
    static PropertyControllerExtensionFactoryBase *instance()
    {
        if (!s_instance)
            s_instance = new PropertyControllerExtensionFactory<T>();
        return s_instance;
    }

    PropertyControllerExtension *create(PropertyController *controller) override
    {
        return new T(controller);
    }

private:
    explicit PropertyControllerExtensionFactory() {}
    static PropertyControllerExtensionFactory<T> *s_instance;
};

template<typename T>
PropertyControllerExtensionFactory<T> *PropertyControllerExtensionFactory<T>::s_instance = nullptr;
///@endcond
}

#endif // PROPERTYCONTROLLEREXTENSION_H
