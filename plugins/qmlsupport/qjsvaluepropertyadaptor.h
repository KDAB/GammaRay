/*
  qjsvaluepropertyadaptor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef QJSVALUEPROPERTYADAPTOR_H
#define QJSVALUEPROPERTYADAPTOR_H

#include <core/propertyadaptor.h>
#include <core/propertyadaptorfactory.h>

namespace GammaRay {
class QJSValuePropertyAdaptor : public PropertyAdaptor
{
    Q_OBJECT
public:
    explicit QJSValuePropertyAdaptor(QObject *parent = nullptr);
    ~QJSValuePropertyAdaptor() override;

    int count() const override;
    PropertyData propertyData(int index) const override;
};

class QJSValuePropertyAdaptorFactory : public AbstractPropertyAdaptorFactory
{
public:
    PropertyAdaptor *create(const ObjectInstance &oi, QObject *parent = nullptr) const override;
    static QJSValuePropertyAdaptorFactory *instance();

private:
    static QJSValuePropertyAdaptorFactory *s_instance;
};
}

#endif // QJSVALUEPROPERTYADAPTOR_H
