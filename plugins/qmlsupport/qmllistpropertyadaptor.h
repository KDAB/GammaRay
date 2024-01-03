/*
  qmllistpropertyadaptor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QMLLISTPROPERTYADAPTOR_H
#define GAMMARAY_QMLLISTPROPERTYADAPTOR_H

#include <core/propertyadaptor.h>
#include <core/propertyadaptorfactory.h>

namespace GammaRay {
class QmlListPropertyAdaptor : public PropertyAdaptor
{
    Q_OBJECT
public:
    explicit QmlListPropertyAdaptor(QObject *parent = nullptr);
    ~QmlListPropertyAdaptor() override;

    int count() const override;
    PropertyData propertyData(int index) const override;
};

class QmlListPropertyAdaptorFactory : public AbstractPropertyAdaptorFactory
{
public:
    PropertyAdaptor *create(const ObjectInstance &oi, QObject *parent = nullptr) const override;
    static QmlListPropertyAdaptorFactory *instance();

private:
    static QmlListPropertyAdaptorFactory *s_instance;
};
}

#endif // GAMMARAY_QMLLISTPROPERTYADAPTOR_H
