/*
  propertycontrollerextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
    explicit PropertyControllerExtensionFactoryBase()
    {
    }
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
    explicit PropertyControllerExtensionFactory()
    {
    }
    static PropertyControllerExtensionFactory<T> *s_instance;
};

template<typename T>
PropertyControllerExtensionFactory<T> *PropertyControllerExtensionFactory<T>::s_instance = nullptr;
///@endcond
}

#endif // PROPERTYCONTROLLEREXTENSION_H
