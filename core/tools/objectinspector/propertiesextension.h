/*
  propertiesextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_OBJECTINSPECTOR_PROPERTIESEXTENSION_H
#define GAMMARAY_OBJECTINSPECTOR_PROPERTIESEXTENSION_H

#include "common/tools/objectinspector/propertiesextensioninterface.h"
#include "core/propertycontrollerextension.h"

#include <QPointer>

namespace GammaRay {
class PropertyController;

class ObjectDynamicPropertyModel;
class ObjectStaticPropertyModel;
class AggregatedPropertyModel;

class PropertiesExtension : public PropertiesExtensionInterface, public PropertyControllerExtension
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::PropertiesExtensionInterface)

public:
    explicit PropertiesExtension(PropertyController *controller);
    ~PropertiesExtension() override;

    void setProperty(const QString &name, const QVariant &value) override;

    bool setObject(void *object, const QString &typeName) override;
    bool setQObject(QObject *object) override;
    bool setMetaObject(const QMetaObject *metaObject) override;

private:
    AggregatedPropertyModel *m_aggregatedPropertyModel;
    QPointer<QObject> m_object;
};
}

#endif // PROPERTIESEXTENSION_H
