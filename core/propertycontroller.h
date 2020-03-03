/*
  propertycontroller.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
