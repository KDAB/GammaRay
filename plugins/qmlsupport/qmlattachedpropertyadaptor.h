/*
  qmlattachedpropertyadaptor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QMLATTACHEDPROPERTYADAPTOR_H
#define GAMMARAY_QMLATTACHEDPROPERTYADAPTOR_H

#include <core/propertyadaptor.h>
#include <core/propertyadaptorfactory.h>

#include <qqmlprivate.h>

namespace GammaRay {
class QmlAttachedPropertyAdaptor : public PropertyAdaptor
{
    Q_OBJECT
public:
    explicit QmlAttachedPropertyAdaptor(QObject *parent = nullptr);
    ~QmlAttachedPropertyAdaptor() override;

    int count() const override;
    PropertyData propertyData(int index) const override;

protected:
    void doSetObject(const ObjectInstance &oi) override;

private:
    QVector<QQmlAttachedPropertiesFunc> m_attachedTypes;
};

/** QML attached property adaptor. */
class QmlAttachedPropertyAdaptorFactory : public AbstractPropertyAdaptorFactory
{
public:
    PropertyAdaptor *create(const ObjectInstance &oi, QObject *parent = nullptr) const override;
    static QmlAttachedPropertyAdaptorFactory *instance();

private:
    static QmlAttachedPropertyAdaptorFactory *s_instance;
};
}

#endif // GAMMARAY_QMLATTACHEDPROPERTYADAPTOR_H
