/*
  propertycontroller.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PROPERTYCONTROLLER_H
#define GAMMARAY_PROPERTYCONTROLLER_H

#include "gammaray_core_export.h"
#include "propertycontrollerextension.h"

#include <common/propertycontrollerinterface.h>

#include <QPointer>
#include <QVector>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
QT_END_NAMESPACE

namespace GammaRay {
/** @brief Non-UI part of the property widget. */
class GAMMARAY_CORE_EXPORT PropertyController : public PropertyControllerInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::PropertyControllerInterface)
public:
    explicit PropertyController(const QString &baseName, QObject *parent);
    ~PropertyController() override;

    const QString &objectBaseName();

    void setObject(QObject *object);
    void setObject(void *object, const QString &className);
    void setMetaObject(const QMetaObject *metaObject);

    void registerModel(QAbstractItemModel *model, const QString &nameSuffix);

    template<typename T>
    static void registerExtension()
    {
        registerExtension(PropertyControllerExtensionFactory<T>::instance());
    }

private slots:
    void objectDestroyed();

private:
    void loadExtension(PropertyControllerExtensionFactoryBase *factory);
    static void registerExtension(PropertyControllerExtensionFactoryBase *factory);

private:
    QString m_objectBaseName;

    QPointer<QObject> m_object;
    QVector<PropertyControllerExtension *> m_extensions;

    static QVector<PropertyControllerExtensionFactoryBase *> s_extensionFactories;
    static QVector<PropertyController *> s_instances;
};
}

#endif // GAMMARAY_PROPERTYCONTROLLER_H
